/*
** AppDelegate.m
**
*/

#import "AppDelegate.h"
#import "AppViewController.h"
#import "AppView.h"

#import "sw_app.h"

int appWasStarted = 0;

@implementation AppDelegate

@synthesize window;
@synthesize viewController;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
//    int isRetina = 0;
//
//    if( [[UIScreen mainScreen] respondsToSelector:@selector(displayLinkWithTarget:selector:)] &&
//        ([UIScreen mainScreen].scale == 2.0))
//    {
//        isRetina = 1;
//    }
//
//    self.window = [[[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]] autorelease];
//
//    if( [[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone )
//    {
//        AppViewShouldUseRetina = isRetina;
//        self.viewController = [[[AppViewController alloc] initWithNibName:@"iphone" bundle:nil] autorelease];
//    }
//    else
//    {
//        AppViewShouldUseRetina = 0;
//        self.viewController = [[[AppViewController alloc] initWithNibName:@"ipad"   bundle:nil] autorelease];
//    }
//
//    self.window.rootViewController = self.viewController;
//    [self.window makeKeyAndVisible];
//
    swAppInit();
    
    return YES;
}

- (void)hitDispatched:(NSString *)hitString
{
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    if( appWasStarted == 0 )
    {
//        [self.viewController startView];
        swAppStart();
        appWasStarted = 1;
    }
    
    swAppActive();
//    [self.viewController activate];
}

- (void)applicationWillResignActive:(UIApplication *)application
{
//    [self.viewController deactivate];
    swAppInactive();
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    swAppSuspend();
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    swAppResume();
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    //[self.viewController stopAnimation];
    swAppFree();
}

//- (void)dealloc
//{
//    //[viewController release];
//    //[window release];
//    
//    [super dealloc];
//}

@end
