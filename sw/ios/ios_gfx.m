/*
** PROLOGUE:
**
*/


/*
** PREREQUISITES
*/
#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import <UIKit/UIKit.h>

#import "AppDelegate.h"
#import "AppViewController.h"

#include "sw_gfx.h"


/*
** EXPORTED FUNCTIONS
*/


void swGfxSetFrameRate( int rate )
{
    AppDelegate * appDel = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    AppViewController * appVC = appDel.viewController;
    
    appVC.animationFrameInterval = rate;
}

