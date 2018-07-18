/*
** AppDelegate.h
**
*/

#import <UIKit/UIKit.h>


@class AppViewController;

@interface AppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow          * window;
    AppViewController * viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow * window;
@property (nonatomic, retain) IBOutlet AppViewController * viewController;

@end

