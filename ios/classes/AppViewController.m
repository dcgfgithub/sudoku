/*
**  AppViewController.m
*/

#import <QuartzCore/QuartzCore.h>

#import "AppViewController.h"
#import "AppView.h"

#include "sw_gfx.h"
#include "sw_app.h"
#include "sw_app_msg.h"
#include "sw_msg.h"


//@interface AppViewController (PrivateMethods)
//- (SW_APP_ORIENT_E)translateOrientationFrom:(UIDeviceOrientation)inOrientation;
//@end


@interface AppViewController ()

@end

@implementation AppViewController

@synthesize animating;

- (BOOL)prefersStatusBarHidden
{
    return YES;
}

- (void)awakeFromNib
{
    [super awakeFromNib];
    
    animating = FALSE;
    animationFrameInterval = 1;
    displayLink = nil;
}

- (void)viewWillAppear:(BOOL)animated
{
    [self startAnimation];
    [super viewWillAppear:animated];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    
    if( wasStarted == NO )
    {
        wasStarted = YES;
        [self startView];
    }
    
    [self activate];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [self stopAnimation];
    [self deactivate];
    [super viewWillDisappear:animated];
}

- (void)viewDidLoad
{
    EAGLView* eglv = (EAGLView *)self.view;
    eglv.controller = self;
    
//    [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
//    [[NSNotificationCenter defaultCenter] addObserver:self
//                                             selector:@selector(orientationChanged:)
//                                                 name:UIDeviceOrientationDidChangeNotification
//                                               object:nil];
    
    UIDeviceOrientation orientation = [[UIDevice currentDevice] orientation];
    SW_APP_ORIENT_E app_orientation = [self translateOrientationFrom:orientation];

    swAppOrientation( app_orientation );
    
    wasStarted = NO;
}

//- (void)viewWill
//{
//    [[NSNotificationCenter defaultCenter] removeObserver:self];
//    [[UIDevice currentDevice] endGeneratingDeviceOrientationNotifications];
//
//    [super viewDidUnload];
//}

- (SW_APP_ORIENT_E)translateOrientationFrom:(UIDeviceOrientation)inOrientation
{
    SW_APP_ORIENT_E sw_orient = SW_APP_ORIENT_PORTRAIT;

    if( inOrientation == UIDeviceOrientationPortraitUpsideDown )
        sw_orient = SW_APP_ORIENT_PORTRAIT_INV;

    if( inOrientation == UIDeviceOrientationLandscapeLeft )
        sw_orient = SW_APP_ORIENT_LANDSCAPE_LEFT;

    if( inOrientation == UIDeviceOrientationLandscapeRight )
        sw_orient = SW_APP_ORIENT_LANDSCAPE_RIGHT;

    return sw_orient;
}

- (void)orientationChanged:(NSNotification *)notification
{
    UIDeviceOrientation orientation = [[UIDevice currentDevice] orientation];
    SW_APP_ORIENT_E app_orientation = [self translateOrientationFrom:orientation];

    swAppOrientation( app_orientation );
}

- (void)startView
{
    [(EAGLView *)self.view startView];
}

- (void)activate
{
    [(EAGLView *)self.view enableTouch];
    [self startAnimation];
}

- (void)deactivate
{
    [self stopAnimation];
    [(EAGLView *)self.view disableTouch];
}

//- (void)willRotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation
//                                duration:(NSTimeInterval)duration
//{
//    [self deactivate];
//}
//
//- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation
//{
//    [self activate];
//}

- (NSInteger)animationFrameInterval
{
    return animationFrameInterval;
}

- (void)setAnimationFrameInterval:(NSInteger)frameInterval
{
    /*
	** Frame interval defines how many display frames must pass between
    ** each time the display link fires.
    ** The display link will only fire 30 times a second when the frame internal
    ** is two on a display that refreshes 60 times a second. The default frame 
    ** interval setting of one will fire 60 times a second when the display 
    ** refreshes at 60 times a second. A frame interval setting of less than
    ** one results in undefined behavior.
    */
    if (frameInterval >= 1)
    {
        animationFrameInterval = frameInterval;
        
        if (animating)
        {
            [self stopAnimation];
            [self startAnimation];
        }
    }
}

- (void)startAnimation
{
    if( ! animating )
    {
        CADisplayLink *aDisplayLink = 
        		[CADisplayLink displayLinkWithTarget:self 
                                            selector:@selector(drawFrame)];
        [aDisplayLink preferredFramesPerSecond];
        [aDisplayLink addToRunLoop:[NSRunLoop currentRunLoop] 
                           forMode:NSDefaultRunLoopMode];
        displayLink = aDisplayLink;
        
        animating = TRUE;
    }
}

- (void)stopAnimation
{
    if (animating)
    {
        [displayLink invalidate];
        displayLink = nil;
        animating = FALSE;
    }
}

- (void)drawFrame
{
    [(EAGLView *)self.view drawView];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

@end
