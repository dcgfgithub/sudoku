/*
**  EAGLView.h
**
**  Created by Dan Fanthome on 11-06-08.
**  Copyright 2011 __MyCompanyName__. All rights reserved.
*/

#import <UIKit/UIKit.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

#import "AppViewController.h"

extern int AppViewShouldUseRetina;

/*
** This class wraps the CAEAGLLayer from CoreAnimation into 
** a convenient UIView subclass. The view content is basically 
** an EAGL surface you render your OpenGL scene into.
** Note that setting the view non-opaque will only work if the EAGL 
** surface has an alpha channel.
*/
@interface EAGLView : UIView
{
@private
    EAGLContext *context;
    
    GLint framebufferWidth;
    GLint framebufferHeight;
    
    GLuint viewFramebuffer, viewRenderbuffer;
    
    BOOL touchActive;
}

@property (nonatomic, assign) AppViewController * controller;

- (void)drawView;

- (void)disableTouch;
- (void)enableTouch;

- (void)startView;

@end
