/*
** AppViewController.h
**
*/

#import <UIKit/UIKit.h>

#import <OpenGLES/EAGL.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

@interface AppViewController : UIViewController
{
    CADisplayLink *	displayLink;    
    NSInteger 		animationFrameInterval;
    BOOL 			animating;
    BOOL            wasStarted;
}

@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;

- (void)startView;

- (void)activate;
- (void)deactivate;

- (void)startAnimation;
- (void)stopAnimation;

@end
