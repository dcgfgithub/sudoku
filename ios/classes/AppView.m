/*
**  AppView.m
*/

#import <QuartzCore/QuartzCore.h>
#import "AppView.h"

#include "sw_gfx.h"
#include "sw_app.h"

extern int appWasStarted;
int AppViewShouldUseRetina;

@interface EAGLView (PrivateMethods)
- (void)createContext;
- (void)deleteContext;
- (void)createFramebuffer;
- (void)deleteFramebuffer;
- (void)setupView;
@end

static float scaleFactor;

@implementation EAGLView


/*
** You must implement this method
*/
+ (Class)layerClass
{
    return [CAEAGLLayer class];
}


/*
** The EAGL view is stored in the nib file. 
** When it's unarchived it's sent -initWithCoder:
*/
- (id)initWithCoder:(NSCoder*)coder
{
    self = [super initWithCoder:coder];
	if (self)
    {
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        
        eaglLayer.opaque = TRUE;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                        [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking,
                        kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat,
                        nil];
        
        [self setContentScaleFactor:2.0];
        [self createContext];
        [self createFramebuffer];
        
        scaleFactor = [self contentScaleFactor];
    }
    return self;
}

- (void)dealloc
{   
    [self deleteFramebuffer];    
    [self deleteContext];
}

- (void)layoutSubviews
{
	[EAGLContext setCurrentContext:context];
    
	[self deleteFramebuffer];
	[self createFramebuffer];
}


- (void)createContext
{
    EAGLContext *aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
    
    if (!aContext)
        NSLog(@"Failed to create ES context");
    else if (![EAGLContext setCurrentContext:aContext])
        NSLog(@"Failed to set ES context current");
    
    context = aContext;
}

- (void)deleteContext
{
    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];
    
    context = nil;
}


- (void)createFramebuffer
{
    if (context && !viewFramebuffer)
    {
        [EAGLContext setCurrentContext:context];
        
        glGenFramebuffers(1, &viewFramebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, viewFramebuffer);
        
        glGenRenderbuffers(1, &viewRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, viewRenderbuffer);
        
        [context renderbufferStorage:GL_RENDERBUFFER 
        				fromDrawable:(CAEAGLLayer *)self.layer];                
        
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH,  &framebufferWidth);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &framebufferHeight);
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, viewRenderbuffer);
        
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            NSLog( @"Failed to make complete framebuffer object %x", 
            	   glCheckFramebufferStatus(GL_FRAMEBUFFER) );
        }
    }
}

- (void)deleteFramebuffer
{
    if (context)
    {
        [EAGLContext setCurrentContext:context];
        
        if (viewFramebuffer)
        {
            glDeleteFramebuffers(1, &viewFramebuffer);
            viewFramebuffer = 0;
        }
        if (viewRenderbuffer)
        {
            glDeleteRenderbuffers(1, &viewRenderbuffer);
            viewRenderbuffer = 0;
        }
    }
}

- (void)startView
{
    swAppDisplaySize( framebufferWidth, framebufferHeight );
}

- (void)drawView
{
    if( appWasStarted == 0 )
    {
        [self startView];
        swAppStart();
        appWasStarted = 1;
    }
        
	swAppProcess();
	
	[EAGLContext setCurrentContext:context];
	
	glBindFramebuffer(GL_FRAMEBUFFER, viewFramebuffer);
	glClear(GL_COLOR_BUFFER_BIT);

	swAppDisplay();
	
	glBindRenderbuffer(GL_RENDERBUFFER, viewRenderbuffer);
	[context presentRenderbuffer:GL_RENDERBUFFER];
}


- (void)enableTouch
{
    touchActive = TRUE;
}

- (void)disableTouch
{
    touchActive = FALSE;
}


- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event 
{
    if( touchActive == FALSE )
        return;
    
    if( [event allTouches].count > 1 )
        return;
        
    UITouch *theTouch = [touches anyObject];
    CGPoint pnt = [theTouch locationInView: self];
    
    swAppTouch( (scaleFactor * pnt.x), framebufferHeight - (scaleFactor * pnt.y) );
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event 
{
    if( touchActive == FALSE )
        return;
    
    if( [event allTouches].count > 1 )
        return;
                
    UITouch *theTouch = [touches anyObject];
    CGPoint pnt = [theTouch locationInView: self];
    
    swAppTouchMove( (scaleFactor * pnt.x), framebufferHeight - (scaleFactor * pnt.y) );
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event 
{
    if( touchActive == FALSE )
        return;
    
    if( [event allTouches].count > 1 )
        return;
        
    UITouch *theTouch = [touches anyObject];
    CGPoint pnt = [theTouch locationInView: self];
    
    swAppTouchUp( (scaleFactor * pnt.x), framebufferHeight - (scaleFactor * pnt.y) );
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event 
{
}

@end
