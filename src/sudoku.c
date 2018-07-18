//
//  sudoku.c
//  sudoku
//
//  Created by Dan Fanthome on 2018-07-15.
//  Copyright © 2018 qlik. All rights reserved.
//

#include <stdlib.h>

#include "sw_dbg.h"
#include "sw_def.h"

#include "sudoku.h"

/*
** DEFINITIONS
*/

/*
** Provide a handful of predefined sudoku boards that can be used to test the solver.
*/

const SUDOKU_S sudoku_random_3_01 =
{
    3,
    {
    { 0, 1, 0, 0, 0, 2, 7, 0, 6 },
    { 0, 6, 0, 0, 0, 0, 1, 0, 8 },
    { 0, 7, 9, 0, 6, 8, 0, 0, 0 },
    
    { 3, 0, 0, 0, 7, 0, 5, 0, 0 },
    { 4, 0, 0, 5, 0, 6, 0, 0, 3 },
    { 0, 0, 5, 0, 3, 0, 0, 0, 9 },
    
    { 0, 0, 0, 2, 4, 0, 9, 5, 0 },
    { 1, 0, 7, 0, 0, 0, 0, 4, 0 },
    { 9, 0, 2, 6, 0, 0, 0, 8, 0 }
    }
};

const SUDOKU_S sudoku_random_3_02 =
{
    3,
    {
    { 6, 0, 0, 7, 9, 8, 0, 0, 0 },
    { 0, 0, 8, 0, 0, 0, 0, 6, 5 },
    { 3, 7, 0, 0, 0, 1, 4, 0, 0 },
    
    { 0, 0, 1, 9, 0, 0, 0, 2, 0 },
    { 0, 0, 4, 5, 0, 2, 9, 0, 0 },
    { 0, 9, 0, 0, 0, 7, 8, 0, 0 },
    
    { 0, 0, 3, 2, 0, 0, 0, 9, 8 },
    { 8, 2, 0, 0, 0, 0, 6, 0, 0 },
    { 0, 0, 0, 8, 3, 9, 0, 0, 2 }
    }
};

const SUDOKU_S sudoku_random_3_03 =
{
    3,
    {
    { 8, 0, 4, 0, 0, 0, 0, 6, 3 },
    { 2, 7, 0, 1, 9, 0, 0, 0, 0 },
    { 0, 0, 0, 6, 0, 0, 2, 9, 0 },
    
    { 0, 0, 0, 0, 4, 1, 0, 0, 8 },
    { 1, 0, 5, 0, 0, 0, 3, 0, 4 },
    { 7, 0, 0, 3, 5, 0, 0, 0, 0 },
    
    { 0, 6, 7, 0, 0, 8, 0, 0, 0 },
    { 0, 0, 0, 0, 7, 5, 0, 8, 2 },
    { 5, 8, 0, 0, 0, 0, 7, 0, 9 }
    }
};

const SUDOKU_S sudoku_random_3_04 =
{
    3,
    {
    { 0, 1, 2, 0, 0, 7, 0, 0, 4 },
    { 0, 0, 0, 2, 4, 0, 1, 0, 0 },
    { 7, 0, 5, 0, 0, 0, 0, 8, 0 },
    
    { 1, 0, 0, 0, 9, 4, 0, 5, 0 },
    { 9, 0, 0, 3, 0, 1, 0, 0, 6 },
    { 0, 7, 0, 8, 2, 0, 0, 0, 9 },
    
    { 0, 8, 0, 0, 0, 0, 2, 0, 3 },
    { 0, 0, 3, 0, 8, 6, 0, 0, 0 },
    { 4, 0, 0, 5, 0, 0, 7, 9, 0 }
    }
};


const SUDOKU_S sudoku_random_3_05 =
{
    3,
    {
    { 6, 0, 1, 3, 0, 0, 0, 0, 4 },
    { 0, 0, 3, 0, 0, 4, 0, 6, 0 },
    { 0, 0, 0, 0, 0, 7, 9, 0, 0 },
    
    { 8, 0, 0, 0, 3, 0, 0, 2, 1 },
    { 0, 3, 0, 7, 0, 6, 0, 8, 0 },
    { 5, 6, 0, 0, 8, 0, 0, 0, 9 },
    
    { 0, 0, 7, 6, 0, 0, 0, 0, 0 },
    { 0, 5, 0, 1, 0, 0, 2, 0, 0 },
    { 2, 0, 0, 0, 0, 3, 5, 0, 7 }
    }
};

const SUDOKU_S sudoku_random_3_06 =
{
    3,
    {
    { 0, 1, 0, 3, 0, 0, 7, 0, 0 },
    { 6, 2, 0, 0, 1, 0, 9, 0, 0 },
    { 3, 0, 0, 7, 0, 0, 0, 1, 0 },
    
    { 1, 0, 0, 9, 0, 0, 5, 0, 0 },
    { 0, 8, 0, 4, 0, 1, 0, 9, 0 },
    { 0, 0, 5, 0, 0, 8, 0, 0, 6 },
    
    { 0, 6, 0, 0, 0, 7, 0, 0, 9 },
    { 0, 0, 8, 0, 4, 0, 0, 7, 5 },
    { 0, 0, 2, 0, 0, 3, 0, 8, 0 }
    }
};

const SUDOKU_S sudoku_random_3_07 =
{
    3,
    {
    { 0, 0, 5, 0, 8, 0, 9, 0, 0 },
    { 1, 0, 0, 5, 0, 0, 3, 0, 0 },
    { 0, 2, 0, 1, 0, 0, 0, 0, 5 },
    
    { 8, 0, 0, 0, 3, 2, 0, 9, 0 },
    { 5, 7, 0, 0, 0, 0, 0, 4, 2 },
    { 0, 4, 0, 6, 7, 0, 0, 0, 3 },
    
    { 3, 0, 0, 0, 0, 8, 0, 7, 0 },
    { 0, 0, 9, 0, 0, 1, 0, 0, 6 },
    { 0, 0, 1, 0, 6, 0, 4, 0, 0 }
    }
};


const SUDOKU_S sudoku_random_3_08 =
{
    3,
    {
    { 1, 0, 0, 0, 0, 0, 3, 6, 0 },
    { 2, 0, 3, 0, 0, 8, 0, 7, 0 },
    { 0, 0, 0, 9, 4, 0, 0, 0, 0 },
    
    { 0, 0, 0, 5, 0, 0, 1, 8, 0 },
    { 6, 0, 0, 7, 0, 2, 0, 0, 9 },
    { 0, 3, 4, 0, 0, 1, 0, 0, 0 },
    
    { 0, 0, 0, 0, 7, 6, 0, 0, 0 },
    { 0, 6, 0, 3, 0, 0, 2, 0, 7 },
    { 0, 5, 1, 0, 0, 0, 0, 0, 3 }
    }
};

const SUDOKU_S sudoku_random_3_09 =
{
    3,
    {
    { 7, 6, 0, 5, 0, 0, 0, 0, 0 },
    { 0, 0, 9, 0, 3, 0, 0, 7, 0 },
    { 0, 0, 0, 0, 0, 6, 4, 2, 0 },
    
    { 1, 0, 7, 0, 6, 0, 0, 0, 8 },
    { 0, 0, 0, 3, 4, 5, 0, 0, 0 },
    { 5, 0, 0, 0, 1, 0, 2, 0, 3 },
    
    { 0, 7, 8, 9, 0, 0, 0, 0, 0 },
    { 0, 1, 0, 0, 8, 0, 7, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 9, 4 }
    }
};


const SUDOKU_S sudoku_random_3_10 =
{
    3,
    {
    { 7, 0, 0, 2, 0, 0, 4, 0, 0 },
    { 0, 0, 8, 4, 0, 0, 0, 0, 0 },
    { 0, 1, 0, 0, 0, 0, 0, 0, 0 },
    
    { 3, 0, 0, 6, 0, 0, 0, 0, 0 },
    { 2, 0, 0, 0, 0, 0, 0, 5, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 1, 9 },
    
    { 0, 5, 0, 0, 0, 9, 0, 0, 0 },
    { 4, 0, 0, 0, 0, 0, 6, 0, 0 },
    { 0, 0, 0, 0, 1, 0, 0, 0, 0 }
    }
};

const SUDOKU_S sudoku_random_3_11 =
{
    3,
    {
    { 0, 0, 1, 0, 0, 0, 0, 0, 3 },
    { 0, 0, 0, 5, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 6, 0 },
    
    { 7, 1, 0, 0, 0, 0, 0, 4, 0 },
    { 5, 0, 0, 0, 3, 0, 0, 0, 0 },
    { 0, 0, 0, 8, 0, 0, 0, 0, 0 },
    
    { 2, 9, 0, 6, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 3, 0, 8 },
    { 0, 0, 0, 0, 0, 4, 1, 0, 0 }
    }
};

const SUDOKU_S sudoku_random_3_12 =
{
    3,
    {
    { 0, 6, 0, 0, 4, 0, 0, 2, 0 },
    { 0, 3, 5, 1, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    
    { 2, 7, 0, 0, 0, 0, 0, 4, 0 },
    { 0, 0, 0, 3, 0, 5, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 8, 0, 0 },
    
    { 4, 0, 0, 0, 2, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 3, 0, 6 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0 }
    }
};

static int sudoku_board_counter = 0;


const SUDOKU_S * sudokuGenerateRandomBoard( unsigned int n ) {
    /*
    ** Returns one of the predefined sudoku boards.
    ** For now, cycle between all of the boards in sequences.
    ** Currently, the value of n is ignored.
    */
    const SUDOKU_S * psudoku = NULL;

    if( sudoku_board_counter == 0  ) psudoku = &sudoku_random_3_01;
    if( sudoku_board_counter == 1  ) psudoku = &sudoku_random_3_02;
    if( sudoku_board_counter == 2  ) psudoku = &sudoku_random_3_03;
    if( sudoku_board_counter == 3  ) psudoku = &sudoku_random_3_04;
    if( sudoku_board_counter == 4  ) psudoku = &sudoku_random_3_05;
    if( sudoku_board_counter == 5  ) psudoku = &sudoku_random_3_06;
    if( sudoku_board_counter == 6  ) psudoku = &sudoku_random_3_07;
    if( sudoku_board_counter == 7  ) psudoku = &sudoku_random_3_08;
    if( sudoku_board_counter == 8  ) psudoku = &sudoku_random_3_09;
    if( sudoku_board_counter == 9  ) psudoku = &sudoku_random_3_10;
    if( sudoku_board_counter == 10 ) psudoku = &sudoku_random_3_11;
    if( sudoku_board_counter == 11 ) psudoku = &sudoku_random_3_12;

    sudoku_board_counter++;

    if( sudoku_board_counter > 11 )
        sudoku_board_counter = 0;
    
    return psudoku;
}

void sudokuClear( SUDOKU_S * psudoku )
{
    SW_ASSERT( psudoku );
    
    psudoku->n = 0;
    for( int row=0; row<16; row++ )
    for( int col=0; col<16; col++ )
    {
        psudoku->board[row][col] = 0;
    }
}

int sudokuValidate( SUDOKU_S * psudoku, int * row_e, int * col_e ) {
    /*
    ** Validate a given layout for a sudoku board.
    ** Used when creating a starting sudoku board manually.
    ** Returns non-zero if the board is in a vaid state.
    **
    ** If the board in determined to be invalid, the row_e and col_e parameters will
    ** be set to the row and column of the offending entry.
    */
    *row_e = -1;
    *col_e = -1;
    
    if( psudoku->n < 1 || psudoku->n > 4 )
        return 0;
    
    int size = psudoku->n * psudoku->n;
    
    for( int row=0; row<size; row++ )
    for( int col=0; col<size; col++ )
    {
        if( psudoku->board[row][col] > 0 )
        {
            for( unsigned int c=0; c<size; c++ )
            {
                if( c != col && psudoku->board[row][c] == psudoku->board[row][col] )
                {
                    *row_e = row;
                    *col_e = c;
                    return 0;
                }

            }
            for( unsigned int r=0; r<size; r++ )
            {
                if( r != row && psudoku->board[r][col] == psudoku->board[row][col] )
                {
                    *row_e = r;
                    *col_e = col;
                    return 0;
                }
            }
            
            int reg = ((row / psudoku->n) * psudoku->n) + (col / psudoku->n);
            int rr;
            int rc;
            
            switch( reg )
            {
                case 0: rr = 0; rc=0; break;
                case 1: rr = 0; rc=3; break;
                case 2: rr = 0; rc=6; break;
                case 3: rr = 1; rc=0; break;
                case 4: rr = 1; rc=3; break;
                case 5: rr = 1; rc=6; break;
                case 6: rr = 2; rc=0; break;
                case 7: rr = 2; rc=3; break;
                case 8: rr = 2; rc=6; break;
                default: rr= 0; rc=0; break;
            }
            for( int regr=rr; regr<rr+psudoku->n; regr++ )
            for( int regc=rc; regc<rc+psudoku->n; regc++ )
            {
                if( regr != row && regc != col && psudoku->board[regr][regc] == psudoku->board[row][col] )
                {
                    *row_e = regr;
                    *col_e = regc;
                    return 0;
                }
            }
        }
    }
    return 1;
}
