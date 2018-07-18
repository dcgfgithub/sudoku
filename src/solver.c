/*
** General sudoku solver algorithm.
** Technically, this solver can support sudoku puzzles for n values between 1 and 4, although it
** is has only been tested with n = 3.
**
** The solver uses as a basis a basic backtracking method. The backtracking method follows the steps.
**
** - For each empty entry on the gameboard, try one value.
** - Check to see if adding that value creates and invalid gameboard.
** - If not, go to the next empty entry and repeat the process.
** - If it does create an invalid entry, backtrack. This involves removing the entry, going back to
**   the previous entry and trying the next possible value.
** - If backtracking takes you past the first entry, no solution exists.
** - If all empty entries have been filled, the solution exists.
**
** This is the basic brute-force method for solving and it is not particularly efficient.
** To improve on the performance, this algorithm incorporates a few optimizations.
**
** The first is to recognize that for each empty square it is possible to trim the number of candidates
** by performing a scan of the initial gameboard and eliminating any of them that would generate an
** invalid gameboard. A candidate list is therefore generated that stores for each empty square a list
** of possible candiates to try for that square.
**
** The second optimization is to perform a 'pruning' operation each time a candidate is used.
** It works as follows ..
**
** - When a valid candiate value is chosen for a candidate square, the remaining candidate squares in the list
**   of candidates will 'prune' or temporarily remove this value from their list of candidate values, if they
**   qualify for pruning. A candidate qualifies for pruning if it has these properties.
**      . Has as a candidate the same value
**      . Has the same row, column or region as the candidate square.
** - If the candidate value is 'pruned' from a candidate, the operation is stored.
** - If a backtrack is performed on the initial candidate square, any pruning operations are undone, and the
**   values that were removed from the candidates are restored.
**
** Using these two optimizations, a fairly quick solver is generated that can handle even hard problems in
** a few seconds, and most easy problems in less than a second.
**
*/

/*
** PREREQUISITES
*/
#include "sw_dbg.h"

#include "sudoku.h"
#include "solver.h"

/*
** DEFINITIONS
*/

#define SOLVER_BACKTRACK_STACK_SIZE 34000
#define SOLVER_CANDIDATE_ARRAY_SIZE 256
#define SOLVER_SOLUTION_STACK_SIZE  256

struct _SOLVER_GAMEBOARD_S
{
    /*
    ** Stores a representation of the initial gameboard.
    ** Used when determining if a possible candidate value is valid.
    **
    ** The gameboard is essentially stored twice, one in row-major and another in column-major order.
    ** This potentially can reduce the number of cache-misses when comparing values. A third array is also
    ** maintained to store the initial values for each region.
    **
    ** Values are stored as a bitmask, where each bit represents a single value. So, a value with a 1 in bit
    ** 5 represents the value of 5. Each entry is 16 bits, so this means it can store values from 1 to 16, which
    ** supports sudoku boards up to n = 4.
    */
    unsigned short rows[16][16];
    unsigned short cols[16][16];
    unsigned short regs[16];
};
typedef struct _SOLVER_GAMEBOARD_S SOLVER_GAMEBOARD_S;

struct _SOLVER_CANDITATE_S
{
    /*
    ** Structure representing a single entry in the candidate array.
    ** Each candidate represents a single candidate square from the gameboard. It stores its location
    ** in the gameboard, as well as the number and possible candidate values.
    **
    ** A bitfield is used as an optimizing step.
    */
    unsigned int row : 4;
    unsigned int col : 4;
    unsigned int reg : 4;
    unsigned int num : 4;
    unsigned int val : 16;
        /*
        ** The list of possible candidate values.
        ** The candidate values are stored as a bitfield, where if a 1 appears at a particular position, that
        ** value is a candidate. So, for example, if this value was 0000100010010000, ones appear in positions 5, 8 and
        ** 12, so 5, 8 and 12 are possible candidate values for this candidate square.
        */
};
typedef struct _SOLVER_CANDITATE_S SOLVER_CANDITATE_S;

union _SOLVER_BACKTRACK_S
{
    /*
    ** Represents a single entry on the backtracking stack.
    ** Each entry stores a 'pruning' operation that was performed.
    ** It can also store other information which is discussed with the backtrack stack.
    */
    struct
    {
        unsigned int row : 16; /* The row from the candidate array where this pruning operation occured. */
        unsigned int val : 16; /* The value pruned. Again, the value is stored as a bitmask. */
    } bt;
    unsigned int item;
    unsigned int count;

};
typedef union _SOLVER_BACKTRACK_S SOLVER_BACKTRACK_S;


/*
** LOCAL VARIABLES
*/

static SOLVER_GAMEBOARD_S solver_gameboard;

static SOLVER_CANDITATE_S solver_candidate_array[ SOLVER_CANDIDATE_ARRAY_SIZE ];
static SOLVER_BACKTRACK_S solver_backtrack_stack[ SOLVER_BACKTRACK_STACK_SIZE ];
        /*
        ** The backtracking stack stores backtracking operations to undo any pruning operations
        ** performed earlier. As pruning operations are performed, the undo operations are stored on the
        ** backtracking stack. Immediately following the operations, the number of operations are pushed,
        ** and the current candidate bitfield is also stored.
        */

static SOLVER_CANDITATE_S solver_solution_stack [ SOLVER_SOLUTION_STACK_SIZE  ];
        /*
        ** The solution stack stores the currently working solution.
        ** As new candidate values are tried, these are added to the solution stack.
        ** When a backtracking operation is performed, possible solutions are removed.
        */

static unsigned int solver_backtrack_stack_top;
static unsigned int solver_solution_stack_top;

static unsigned int solver_candidate_current;
static unsigned int solver_candidate_count;

static unsigned int solver_n;
static unsigned int solver_nn;


/*
** LOCAL FUNCTIONS
*/

void solverSetGameboard( const SUDOKU_S * psudoku )
{
    solver_n  = psudoku->n;
    solver_nn = solver_n * solver_n;
    
    for( unsigned int reg=0; reg<solver_nn; reg++ )
    {
        solver_gameboard.regs[reg] = 0;
    }
    
    for( unsigned int row=0; row<solver_nn; row++ )
    {
        for( unsigned int col=0; col<solver_nn; col++ )
        {
            unsigned int val = 0;
            unsigned int reg = ((row / solver_n) * solver_n) + (col / solver_n);
            unsigned int bv  = psudoku->board[row][col];
            
            if( bv > 0 ) {
                /*
                ** Non-zero board entries are valid values.
                ** Determine the bitfield representation for the value, which moves the 1
                ** bit to the ordered position representing the value.
                */
                val = 1 << (bv - 1);
            }
    
            solver_gameboard.rows[row][col] = val;
            solver_gameboard.cols[col][row] = val;
            solver_gameboard.regs[reg]     |= val;
        }
    }
}

int solverRowHasValue( unsigned int row, unsigned int cand_mask )
{
    for( unsigned int col=0; col<solver_nn; col++ )
    {
        if( solver_gameboard.rows[row][col] & cand_mask )
            return 1;
    }
    return 0;
}

int solverColHasValue( unsigned int col, unsigned int cand_mask )
{
    for( unsigned int row=0; row<solver_nn; row++ )
    {
        if( solver_gameboard.cols[col][row] & cand_mask )
            return 1;
    }
    return 0;
}

int solverRegHasValue( unsigned int reg, unsigned int cand_mask )
{
    return solver_gameboard.regs[reg] & cand_mask;
}

void solverGenerateCandiates( const SUDOKU_S * psudoku )
{
    solver_candidate_count = 0;
    
    for( unsigned int row=0; row<solver_nn; row++ )
    {
        for( unsigned int col=0; col<solver_nn; col++ )
        {
            unsigned int reg = ((row / solver_n) * solver_n) + (col / solver_n);
            
            if( psudoku->board[row][col] == 0 )
            {
                solver_candidate_array[solver_candidate_count].row = row;
                solver_candidate_array[solver_candidate_count].col = col;
                solver_candidate_array[solver_candidate_count].reg = reg;
                solver_candidate_array[solver_candidate_count].num = 0;
                solver_candidate_array[solver_candidate_count].val = 0;
                
                for( unsigned int cand_shift=0; cand_shift<solver_nn; cand_shift++ ) {
                    /*
                    ** Generates the list of possible candidate values for this candidate square.
                    ** Uses the fact that the initial gameboard has already been setup earlier using bitfield values.
                    **
                    ** Each possible candidate value is tried by first generating the bitfield representation of
                    ** that value and seeing if it is a valid value, and if so, adding it to the list of candidates.
                    */
                    unsigned int candidate = 1 << cand_shift;
                    
                    if( solverRowHasValue(row, candidate) == 0 &&
                        solverColHasValue(col, candidate) == 0 &&
                        solverRegHasValue(reg, candidate) == 0 )
                    {
                        solver_candidate_array[solver_candidate_count].val |= candidate;
                        solver_candidate_array[solver_candidate_count].num++;
                    }
                }
                
                solver_candidate_count++;
            }
        }
    }
}

void solverSortCandidates( void ) {
    /*
    ** Sorts the array of candidates by the number of possible candidate values.
    ** Uses a basic insertion sort.
    */
    unsigned int i = 1;
    unsigned int j;
    
    while( i < solver_candidate_count )
    {
        j = i;
        while( j > 0 && solver_candidate_array[j-1].num > solver_candidate_array[j].num )
        {
            SOLVER_CANDITATE_S temp = solver_candidate_array[j];
            solver_candidate_array[j] = solver_candidate_array[j-1];
            solver_candidate_array[j-1] = temp;
            j--;
        }
        i++;
    }
}

void solverPrune(
    unsigned int row,
    unsigned int col,
    unsigned int reg,
    unsigned int value
    ) {
    /*
    ** Perform a 'pruning' operation on all remaining candidate squares.
    ** Given the row, col, region and candidate value to prune, checks if the candidate square
    ** has the same, row, column or region (which would cause an invalid board), and removes the candidate
    ** value from its list of candidate values.
    */
    unsigned int count = 0;
    
    for( unsigned int cand=solver_candidate_current+1; cand<solver_candidate_count; cand++ )
    {
        if( solver_candidate_array[cand].row == row ||
            solver_candidate_array[cand].col == col ||
            solver_candidate_array[cand].reg == reg )
        {
            if( solver_candidate_array[cand].val & value )
            {
                solver_candidate_array[cand].val &= ~value;
                solver_candidate_array[cand].num--; /* Remove the value from the list of candidate values */
                
                solver_backtrack_stack[solver_backtrack_stack_top].bt.row = cand;
                solver_backtrack_stack[solver_backtrack_stack_top].bt.val = value; /* Add the prune operation to the backtrack stack */
                
                solver_backtrack_stack_top++;
                count++;
            }
        }
    }
    /*
    ** Once all of the backtrack operations have been added, store the number of operations, as well as
    ** the next possible value to check. This means that when a restore operation is performed, and the pruning
    ** operations are undone, the backtracked candidate square knows where to resume is candidate value search.
    */
    solver_backtrack_stack[solver_backtrack_stack_top++].count = count;
    solver_backtrack_stack[solver_backtrack_stack_top++].item  = value << 1;
}

unsigned short solverRestore( void ) {
    /*
    ** Undoes the last set of pruning operations.
    ** Returns the next possible candidate value stored from the previous prune operation.
    */
    unsigned short item = solver_backtrack_stack[--solver_backtrack_stack_top].item;
    unsigned int  count = solver_backtrack_stack[--solver_backtrack_stack_top].count;
    
    for( unsigned int bt=0; bt<count; bt++ )
    {
        SOLVER_BACKTRACK_S bt_s = solver_backtrack_stack[--solver_backtrack_stack_top];
        
        solver_candidate_array[bt_s.bt.row].val |= bt_s.bt.val;
        solver_candidate_array[bt_s.bt.row].num++; /* Restores the candidate value */
    }
    return item;
}

unsigned char solverGetValue( unsigned short val_mask )
{
    unsigned char  val  = 1;
    unsigned short mask = 1;
    
    while( (val_mask & mask) == 0 )
    {
        mask <<= 1;
        val++;
    }
    if( mask > 0 )
        return val;
    else
        return 0;
}


/*
** EXPORTED FUNCTIONS
*/

int solverSolve( const SUDOKU_S * psudoku, SUDOKU_S * solution ) {
    /*
    ** Attempts to find a solution to the initial board configuration given in psudoku.
    ** The solution is provided in a second sudoku instance. This allows the caller to
    ** differentiate the values generated by the algorithm from the initial values.
    **
    ** Returns non-zero if a solution was found, zero otherwise.
    **
    ** The algorithm currently only generates the first possible solution found.
    ** It would be possible to adjust it to generate all solutions by forcing a backtrack operation
    ** on the last candidate square and continuing the algorithm.
    */
    solverSetGameboard( psudoku );
    solverGenerateCandiates( psudoku );
    solverSortCandidates();
        /*
        ** Sort the candidates by number of possible candidate values.
        ** Initially done as a possible optimization by processing candidates with a small number of
        ** candidate values (possibly one). However, this would only work if you resorted the candidates after
        ** each candidate value is chosen. This could potentially mean (for simpler puzzles) that most candidates
        ** would have only one possible value, which would simplify the search. However, the cost of sorting the
        ** entries regularily would be detrimental to the running time.
        ** The initial sort is left in anyway.
        */
    
    solver_backtrack_stack_top = 0;
    solver_solution_stack_top  = 0;
    solver_candidate_current   = 0;
    
    unsigned short cand_mask = 1;
    
    while( solver_candidate_current < solver_candidate_count )
    {
        SOLVER_CANDITATE_S cand = solver_candidate_array[solver_candidate_current];
        /*
        ** Go through each candidate in order, as search for possible candidate values for each.
        ** The cand_mask is the bitfield representing the current value being tested.
        */
        int searching = 1;
        
        while( searching )
        {
            while( cand_mask > 0 && (cand_mask & cand.val) == 0 ) cand_mask <<= 1; /* Skip until candidate value is found */
            
            if( cand_mask > 0 )
            {
                /*
                ** Possible candidate found.
                ** Check to see if it is a valid candidate, and if so, prune it from the remaining candidates and
                ** move on to the next candidate square.
                */
                if( solverRowHasValue(cand.row, cand_mask) == 0 &&
                    solverColHasValue(cand.col, cand_mask) == 0 &&
                    solverRegHasValue(cand.reg, cand_mask) == 0 )
                {
                    solverPrune(cand.row, cand.col, cand.reg, cand_mask);
                    
                    solver_solution_stack[solver_solution_stack_top].row = cand.row;
                    solver_solution_stack[solver_solution_stack_top].col = cand.col;
                    solver_solution_stack[solver_solution_stack_top].val = cand_mask;
                    solver_solution_stack_top++;
                    
                    solver_candidate_current++;
                    cand_mask = 1;
                    searching = 0;
                }
                else
                {
                    cand_mask <<= 1; /* Move to next possible candidate value */
                }
            }
            else
            {
                /*
                ** All candidate values have been searched.
                ** This will happen when the cand_mask 1 bit is shifted past the length of the value.
                */
                if( solver_candidate_current > 0 )
                {
                    /*
                    ** Were not on the first candidate, so perform a backtrack, restoring any pruned values.
                    */
                    cand_mask = solverRestore();
                    solver_solution_stack_top--;
                    solver_candidate_current--;
                }
                else
                {
                    SW_INFO("No solution exists");
                    return 0;
                }
                searching = 0;
            }
        }
    }
    /*
    ** If we reach this point, all candidate squares have been assigned values, so a solution exists.
    ** Fill in the solution with the values stored on the solution stack.
    */
    for( int x=0; x<solver_solution_stack_top; x++ )
    {
        solution->board[solver_solution_stack[x].row][solver_solution_stack[x].col] = solverGetValue( solver_solution_stack[x].val );
    }
    
    SW_INFO("Algorithm complete");
    
    return 1;
}



