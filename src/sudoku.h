#ifndef __SUDOKU_H__
#define __SUDOKU_H__
/*
** Provides a basic representation for a sudoku board.
** The representation in theory can support any sudoku board with n <= 4, but for simplicity
** sake, only n = 3 is supported in its current incarnation.
*/

/*
** DEFINITIONS
*/


struct _SUDOKU_S
{
    unsigned int n;
    unsigned char board[16][16];
        /*
        ** Gameboard entries.
        ** In all cases, these will be the number of entry, or 0 indicating
        ** the entry is blank.
        */
};
typedef struct _SUDOKU_S SUDOKU_S;


/*
** PUBLIC FUNCTIONS
*/

const SUDOKU_S * sudokuGenerateRandomBoard( unsigned int n );

void sudokuClear   ( SUDOKU_S * psudoku );
int  sudokuValidate( SUDOKU_S * psudoku, int * row_e, int * col_e );

#endif
