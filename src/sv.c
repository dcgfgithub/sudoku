/*
** Main implementation for the solver app.
*/

#include "sw_def.h"
#include "sw_dbg.h"
#include "sw_spr.h"
#include "sw_gfx.h"
#include "sw_msg.h"

#include "sv.h"

#include "tex_main.h"

#include "sudoku.h"
#include "solver.h"


/*
** DEFINITIONS
*/

enum
{
    SV_SPR_BG = 0,
    SV_SPR_BOARD,

    SV_SPR_PEG_FIRST,
    SV_SPR_PEG_COUNT = 81,
    SV_SPR_PEG_LABEL_FIRST = SV_SPR_PEG_FIRST + SV_SPR_PEG_COUNT,
    SV_SPR_PEG_SETUP_FIRST = SV_SPR_PEG_LABEL_FIRST + SV_SPR_PEG_COUNT,
    SV_SPR_PEG_SETUP_COUNT = 9,
    SV_SPR_PEG_SETUP_LABEL_FIRST = SV_SPR_PEG_SETUP_FIRST + SV_SPR_PEG_SETUP_COUNT,
    
    SV_SPR_BTN_BACK = SV_SPR_PEG_SETUP_LABEL_FIRST + SV_SPR_PEG_SETUP_COUNT,
    SV_SPR_BTN_GENERATE,
    SV_SPR_BTN_CREATE,
    SV_SPR_BTN_SOLVE,
    SV_SPR_BTN_SUBMIT,
    
    SV_SPR_COUNT
};

static SW_MSG_ID sv_msg_as[] =
{
    SW_MSG_TOUCH,
    SW_MSG_TOUCH_UP,
    SW_MSG_TOUCH_MOVE,
    SW_MSG_NONE
};

enum
{
    SV_HIT_GENERATE = 0,
    SV_HIT_CREATE,
    SV_HIT_BACK,
    SV_HIT_SOLVE,
    SV_HIT_SUBMIT,
    
    SV_HIT_PEG_FIRST,
    SV_HIT_PEG_COUNT = 81,
    
    SV_HIT_SETUP_PEG_FIRST = SV_HIT_PEG_FIRST + SV_HIT_PEG_COUNT,
    SV_HIT_SETUP_PEG_COUNT = 9,
    
    SV_HIT_COUNT = SV_HIT_SETUP_PEG_FIRST + SV_HIT_SETUP_PEG_COUNT
};

enum
{
    SV_STATE_NONE = 0,
    
    SV_STATE_MAIN,
    SV_STATE_CREATE,
    SV_STATE_SUBMIT,
    SV_STATE_SOLVE,
    SV_STATE_DRAG_PEG,
    
    SV_STATE_COUNT
};


/*
** LOCAL VARIABLES
*/

static int   sv_state = SV_STATE_NONE;

static int   sv_peg;
static float sv_peg_restore_x;
static float sv_peg_restore_y;
static float sv_peg_x;
static float sv_peg_y;

static SWsprite sv_spr_a[ SV_SPR_COUNT ];

static float    sv_hit_a[SV_HIT_COUNT][4] = {0};

static SUDOKU_S sv_sudoku_s = {0};

static int sv_peg_lbl_r_a[] =
{
    TEX_MAIN_IMG_peg_1_r_FIRST,
    TEX_MAIN_IMG_peg_2_r_FIRST,
    TEX_MAIN_IMG_peg_3_r_FIRST,
    TEX_MAIN_IMG_peg_4_r_FIRST,
    TEX_MAIN_IMG_peg_5_r_FIRST,
    TEX_MAIN_IMG_peg_6_r_FIRST,
    TEX_MAIN_IMG_peg_7_r_FIRST,
    TEX_MAIN_IMG_peg_8_r_FIRST,
    TEX_MAIN_IMG_peg_9_r_FIRST
};

static int sv_peg_lbl_b_a[] =
{
    TEX_MAIN_IMG_peg_1_b_FIRST,
    TEX_MAIN_IMG_peg_2_b_FIRST,
    TEX_MAIN_IMG_peg_3_b_FIRST,
    TEX_MAIN_IMG_peg_4_b_FIRST,
    TEX_MAIN_IMG_peg_5_b_FIRST,
    TEX_MAIN_IMG_peg_6_b_FIRST,
    TEX_MAIN_IMG_peg_7_b_FIRST,
    TEX_MAIN_IMG_peg_8_b_FIRST,
    TEX_MAIN_IMG_peg_9_b_FIRST
};


/*
** LOCAL FUNCTIONS
*/

static void sv_Reset()
{
    SWsprite s;
    
    float w = swGfxGetDisplayW();
    float h = swGfxGetDisplayH();
    
    s = sv_spr_a[ SV_SPR_BG ];
    
    swSprClear    ( s );
    swSprSetImage ( s, tex_main_images[TEX_MAIN_IMG_bg_FIRST] );
    swSprSetAnchor( s, SW_ANCHOR_LL );
    swSprSetSize  ( s, w, h, 1 );
    swSprSetPos   ( s, 0, 0 );
    swSprShow     ( s, 1 );
    
    s = sv_spr_a[ SV_SPR_BOARD ];
    
    swSprClear    ( s );
    swSprSetImage ( s, tex_main_images[TEX_MAIN_IMG_board_FIRST] );
    swSprSetAnchor( s, SW_ANCHOR_UM );
    swSprSetPos   ( s, w*0.5, h-150 );
    swSprShow     ( s, 0 );
    
    float bc_x = w * 0.5;
    float bc_y = h - 470;
    
    float x_pos[9] = { -272, -206, -140, -67, -2, 65, 136, 203, 270 };
    float y_pos[9] = {  270,  202,  137,  64, -1,-68,-138,-206,-271 };
    
    for( int peg=0; peg<SV_SPR_PEG_COUNT; peg++ )
    {
        int x = peg % 9;
        int y = peg / 9;
        
        s = sv_spr_a[ SV_SPR_PEG_FIRST+peg ];
        
        swSprClear    ( s );
        swSprSetAnchor( s, SW_ANCHOR_CENTER );
        swSprSetImage ( s, tex_main_images[TEX_MAIN_IMG_peg_FIRST] );
        swSprSetPos   ( s, bc_x+x_pos[x], bc_y+y_pos[y] );
        swSprShow     ( s, 0 );
        
        s = sv_spr_a[ SV_SPR_PEG_LABEL_FIRST+peg ];
        
        swSprClear    ( s );
        swSprSetAnchor( s, SW_ANCHOR_CENTER );
        swSprSetImage ( s, tex_main_images[TEX_MAIN_IMG_peg_1_b_FIRST] );
        swSprSetPos   ( s, bc_x+x_pos[x], bc_y+y_pos[y] );
        swSprShow     ( s, 0 );
        
        swSprGetBoundingBox2( s, sv_hit_a[SV_HIT_PEG_FIRST+peg] );
    }
    
    float sx = (w*0.5) - 280.0;
    float sy = h-150-640-50;
    
    for( int peg=0; peg<SV_SPR_PEG_SETUP_COUNT; peg++ )
    {
        s = sv_spr_a[ SV_SPR_PEG_SETUP_FIRST+peg ];
        
        swSprClear    ( s );
        swSprSetAnchor( s, SW_ANCHOR_CENTER );
        swSprSetImage ( s, tex_main_images[TEX_MAIN_IMG_peg_FIRST] );
        swSprSetPos   ( s, sx, sy );
        swSprShow     ( s, 0 );
        
        s = sv_spr_a[ SV_SPR_PEG_SETUP_LABEL_FIRST+peg ];
        
        swSprClear    ( s );
        swSprSetAnchor( s, SW_ANCHOR_CENTER );
        swSprSetImage ( s, tex_main_images[sv_peg_lbl_b_a[peg]] );
        swSprSetPos   ( s, sx, sy );
        swSprShow     ( s, 0 );
        
        swSprGetBoundingBox2( s, sv_hit_a[SV_HIT_SETUP_PEG_FIRST+peg] );
        
        sx = sx + 70;
    }
    
    s = sv_spr_a[ SV_SPR_BTN_BACK ];
    
    swSprClear    ( s );
    swSprSetAnchor( s, SW_ANCHOR_CENTER );
    swSprSetImage ( s, tex_main_images[TEX_MAIN_IMG_btn_back_FIRST] );
    swSprSetPos   ( s, 75, h-75 );
    swSprShow     ( s, 0 );
    
    swSprGetBoundingBox2(s, sv_hit_a[SV_HIT_BACK]);
    
    s = sv_spr_a[ SV_SPR_BTN_GENERATE ];
    
    swSprClear    ( s );
    swSprSetAnchor( s, SW_ANCHOR_CENTER );
    swSprSetImage ( s, tex_main_images[TEX_MAIN_IMG_btn_generate_FIRST] );
    swSprSetPos   ( s, w*0.5, (h*0.5) + 150 );
    swSprShow     ( s, 0 );
    
    swSprGetBoundingBox2(s, sv_hit_a[SV_HIT_GENERATE]);
    
    s = sv_spr_a[ SV_SPR_BTN_CREATE ];
    
    swSprClear    ( s );
    swSprSetAnchor( s, SW_ANCHOR_CENTER );
    swSprSetImage ( s, tex_main_images[TEX_MAIN_IMG_btn_create_FIRST] );
    swSprSetPos   ( s, w*0.5, (h*0.5) - 150 );
    swSprShow     ( s, 0 );
    
    swSprGetBoundingBox2(s, sv_hit_a[SV_HIT_CREATE]);
    
    s = sv_spr_a[ SV_SPR_BTN_SOLVE ];
    
    swSprClear    ( s );
    swSprSetAnchor( s, SW_ANCHOR_CENTER );
    swSprSetImage ( s, tex_main_images[TEX_MAIN_IMG_btn_solve_FIRST] );
    swSprSetPos   ( s, w*0.5, 150 );
    swSprShow     ( s, 0 );
    
    swSprGetBoundingBox2(s, sv_hit_a[SV_HIT_SOLVE]);
    
    s = sv_spr_a[ SV_SPR_BTN_SUBMIT ];
    
    swSprClear    ( s );
    swSprSetAnchor( s, SW_ANCHOR_CENTER );
    swSprSetImage ( s, tex_main_images[TEX_MAIN_IMG_btn_submit_FIRST] );
    swSprSetPos   ( s, w*0.5, 150 );
    swSprShow     ( s, 0 );
    
    swSprGetBoundingBox2(s, sv_hit_a[SV_HIT_SUBMIT]);
}

static void sv_Scene_Hide( void )
{
    for( int s=SV_SPR_BG+1; s<SV_SPR_COUNT; s++ )
    {
        swSprShow( sv_spr_a[s], 0 );
    }
}

static void sv_Scene_Main( void )
{
    sv_Scene_Hide();
    
    swSprShow( sv_spr_a[SV_SPR_BTN_GENERATE], 1 );
    swSprShow( sv_spr_a[SV_SPR_BTN_CREATE  ], 1 );
    
    sv_state = SV_STATE_MAIN;
}

static void sv_Scene_Create( void )
{
    sv_Scene_Hide();
    
    swSprShow( sv_spr_a[SV_SPR_BOARD], 1 );
    swSprShow( sv_spr_a[SV_SPR_BTN_BACK], 1 );
    swSprShow( sv_spr_a[SV_SPR_BTN_SUBMIT], 1 );
    
    for( int s=0; s<SV_SPR_PEG_SETUP_COUNT; s++ )
    {
        swSprShow( sv_spr_a[SV_SPR_PEG_SETUP_FIRST+s], 1 );
        swSprShow( sv_spr_a[SV_SPR_PEG_SETUP_LABEL_FIRST+s], 1 );
    }
    
    sudokuClear( &sv_sudoku_s );
    sv_sudoku_s.n = 3;
    
    sv_state = SV_STATE_CREATE;
}

static void sv_Scene_Solve( void )
{
    sv_Scene_Hide();
    
    swSprShow( sv_spr_a[SV_SPR_BOARD], 1 );
    swSprShow( sv_spr_a[SV_SPR_BTN_BACK], 1 );
    swSprShow( sv_spr_a[SV_SPR_BTN_SOLVE], 1 );
    
    const SUDOKU_S * psudoku = sudokuGenerateRandomBoard( 3 );

    sudokuClear( &sv_sudoku_s );
    sv_sudoku_s.n = psudoku->n;

    for( int row=0; row<9; row++ )
    {
        for( int col=0; col<9; col++ )
        {
            sv_sudoku_s.board[row][col] = psudoku->board[row][col];

            if( psudoku->board[row][col] > 0 )
            {
                int peg = (row * 9) + col;
                int val = psudoku->board[row][col]-1;

                swSprSetImage( sv_spr_a[SV_SPR_PEG_LABEL_FIRST+peg], tex_main_images[sv_peg_lbl_b_a[val]] );
                swSprShow    ( sv_spr_a[SV_SPR_PEG_LABEL_FIRST+peg], 1 );
                swSprShow    ( sv_spr_a[SV_SPR_PEG_FIRST+peg], 1 );
            }
        }
    }
    
    sv_state = SV_STATE_SOLVE;
}

static void sv_Evaluate( void )
{
    int row_e;
    int col_e;
    int result = sudokuValidate( &sv_sudoku_s, &row_e, &col_e );
    if( result )
    {
        swSprShow( sv_spr_a[SV_SPR_BTN_SUBMIT], 0 );
        swSprShow( sv_spr_a[SV_SPR_BTN_SOLVE ], 1 );
        
        sv_state = SV_STATE_SOLVE;
    }
    else
    {
        if( row_e >= 0 && col_e >= 0 )
        {
            int peg = (row_e * 9) + col_e;
            int val = sv_sudoku_s.board[row_e][col_e];
            if( val > 0 )
            {
                swSprSetImage( sv_spr_a[SV_SPR_PEG_LABEL_FIRST+peg], tex_main_images[sv_peg_lbl_r_a[val-1]] );
            }
        }
    }
}

static void sv_Solve( void )
{
    SUDOKU_S solution = {0};
    
    int result = solverSolve( &sv_sudoku_s, &solution );
    if( result )
    {
        for( int row=0; row<9; row++ )
        {
            for( int col=0; col<9; col++ )
            {
                if( solution.board[row][col] > 0 )
                {
                    int peg = (row * 9) + col;
                    int val = solution.board[row][col]-1;
                    
                    swSprSetImage( sv_spr_a[SV_SPR_PEG_LABEL_FIRST+peg], tex_main_images[sv_peg_lbl_r_a[val]] );
                    swSprShow    ( sv_spr_a[SV_SPR_PEG_LABEL_FIRST+peg], 1 );
                    swSprShow    ( sv_spr_a[SV_SPR_PEG_FIRST+peg], 1 );
                }
            }
        }
    }
}

static void sv_DropPeg( void )
{
    float xo = swSprGetOffsX( sv_spr_a[SV_SPR_PEG_SETUP_FIRST+sv_peg] );
    float yo = swSprGetOffsY( sv_spr_a[SV_SPR_PEG_SETUP_FIRST+sv_peg] );
    
    float x = sv_peg_restore_x + xo;
    float y = sv_peg_restore_y + yo;
    
    for( int peg=0; peg<SV_SPR_PEG_COUNT; peg++ )
    {
        if( swIsTouchingv(x, y, sv_hit_a[SV_HIT_PEG_FIRST+peg]) )
        {
            swSprSetImage( sv_spr_a[SV_SPR_PEG_LABEL_FIRST+peg], tex_main_images[sv_peg_lbl_b_a[sv_peg]] );
            swSprShow    ( sv_spr_a[SV_SPR_PEG_LABEL_FIRST+peg], 1 );
            swSprShow    ( sv_spr_a[SV_SPR_PEG_FIRST+peg], 1 );
            
            int row = peg / 9;
            int col = peg % 9;
            
            sv_sudoku_s.board[row][col] = sv_peg+1;
        }
    }
}

static int sv_Touch( float x, float y )
{
    switch( sv_state )
    {
        case SV_STATE_MAIN:
            if( swIsTouchingv(x, y, sv_hit_a[SV_HIT_CREATE]) )
            {
                swSprSetOffs( sv_spr_a[ SV_SPR_BTN_CREATE ], 5, -5 );
            }
            if( swIsTouchingv(x, y, sv_hit_a[SV_HIT_GENERATE]) )
            {
                swSprSetOffs( sv_spr_a[ SV_SPR_BTN_GENERATE ], 5, -5 );
            }
            break;
            
        case SV_STATE_CREATE:
            if( swIsTouchingv(x, y, sv_hit_a[SV_HIT_BACK]) )
            {
                swSprSetOffs( sv_spr_a[SV_SPR_BTN_BACK], 5, -5 );
            }
            if( swIsTouchingv(x, y, sv_hit_a[SV_HIT_SUBMIT]) )
            {
                swSprSetOffs( sv_spr_a[SV_SPR_BTN_SUBMIT], 5, -5 );
            }
            for( int peg=0; peg<SV_SPR_PEG_SETUP_COUNT; peg++ )
            {
                if( swIsTouchingv(x, y, sv_hit_a[SV_HIT_SETUP_PEG_FIRST+peg]) )
                {
                    sv_peg   = peg;
                    sv_peg_x = swSprGetPosX( sv_spr_a[SV_SPR_PEG_SETUP_FIRST+sv_peg] );
                    sv_peg_y = swSprGetPosY( sv_spr_a[SV_SPR_PEG_SETUP_FIRST+sv_peg] );
                    sv_peg_restore_x = sv_peg_x;
                    sv_peg_restore_y = sv_peg_y;
                    
                    sv_state = SV_STATE_DRAG_PEG;
                }
            }
            break;
            
        case SV_STATE_SOLVE:
            if( swIsTouchingv(x, y, sv_hit_a[SV_HIT_BACK]) )
            {
                swSprSetOffs( sv_spr_a[SV_SPR_BTN_BACK], 5, -5 );
            }
            if( swIsTouchingv(x, y, sv_hit_a[SV_HIT_SOLVE]) )
            {
                swSprSetOffs( sv_spr_a[SV_SPR_BTN_SOLVE], 5, -5 );
            }
            break;
            
        default:
            return 0;
    }

    return 1;
}

static int sv_TouchUp( float x, float y )
{
    switch( sv_state )
    {
        case SV_STATE_MAIN:
        {
            if( swIsTouchingv(x, y, sv_hit_a[SV_HIT_CREATE]) )
            {
                swSprSetOffs( sv_spr_a[ SV_SPR_BTN_CREATE ], 0, 0 );
                sv_Scene_Create();
            }
            if( swIsTouchingv(x, y, sv_hit_a[SV_HIT_GENERATE]) )
            {
                swSprSetOffs( sv_spr_a[ SV_SPR_BTN_GENERATE ], 0, 0 );
                sv_Scene_Solve();
            }
        }
            
        case SV_STATE_CREATE:
            if( swIsTouchingv(x, y, sv_hit_a[SV_HIT_BACK]) )
            {
                swSprSetOffs( sv_spr_a[SV_SPR_BTN_BACK], 0, 0 );
                sv_Scene_Main();
            }
            if( swIsTouchingv(x, y, sv_hit_a[SV_HIT_SUBMIT]) )
            {
                swSprSetOffs( sv_spr_a[ SV_SPR_BTN_SUBMIT ], 0, 0 );
                sv_Evaluate();
            }
            break;
            
        case SV_STATE_SOLVE:
            if( swIsTouchingv(x, y, sv_hit_a[SV_HIT_BACK]) )
            {
                swSprSetOffs( sv_spr_a[SV_SPR_BTN_BACK], 0, 0 );
                sv_Scene_Main();
            }
            if( swIsTouchingv(x, y, sv_hit_a[SV_HIT_SOLVE]) )
            {
                swSprSetOffs( sv_spr_a[ SV_SPR_BTN_SOLVE ], 0, 0 );
                sv_Solve();
            }
            break;
            
        case SV_STATE_DRAG_PEG:
            sv_DropPeg();
            
            swSprSetOffs( sv_spr_a[SV_SPR_PEG_SETUP_FIRST+sv_peg],       0, 0 );
            swSprSetOffs( sv_spr_a[SV_SPR_PEG_SETUP_LABEL_FIRST+sv_peg], 0, 0 );
            
            sv_state = SV_STATE_CREATE;
            break;
            
        default:
            return 0;
    }
    
    return 1;
}

static int sv_TouchMove( float x, float y )
{
    switch( sv_state )
    {
        case SV_STATE_DRAG_PEG:
        {
            float dx = x - sv_peg_restore_x;
            float dy = y - sv_peg_restore_y;
            
            swSprSetOffs( sv_spr_a[SV_SPR_PEG_SETUP_FIRST+sv_peg], dx, dy );
            swSprSetOffs( sv_spr_a[SV_SPR_PEG_SETUP_LABEL_FIRST+sv_peg], dx, dy );
        }
        default:
            return 0;
    }
    
    return 1;
}

static int sv_MsgHandler( SW_MSG_S sMsgData )
{
    switch( sMsgData.msg )
    {
        case SW_MSG_TOUCH:
            return sv_Touch( sMsgData.fVal[0], sMsgData.fVal[1] );
            
        case SW_MSG_TOUCH_UP:
            return sv_TouchUp( sMsgData.fVal[0], sMsgData.fVal[1] );
            
        case SW_MSG_TOUCH_MOVE:
            return sv_TouchMove( sMsgData.fVal[0], sMsgData.fVal[1] );
            
        default:
            return 0;
    }
}


/*
** EXPORTED FUNCTIONS
*/

void sv_Start()
{
    tex_main_LoadImages();
    
    swSprGen( SV_SPR_COUNT, sv_spr_a );
    sv_Reset();
    sv_Scene_Main();
    
    swMsgAttach( sv_msg_as,  sv_MsgHandler, 0 );
}

void sv_Stop()
{
    swMsgDetach( sv_msg_as, sv_MsgHandler );
    
    swSprDel( SV_SPR_COUNT, sv_spr_a );
    
    tex_main_UnloadImages();
}

void sv_Display()
{
    swSprRender( SV_SPR_COUNT, sv_spr_a );
}



