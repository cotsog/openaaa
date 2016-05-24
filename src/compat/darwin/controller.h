#import <AppKit/AppKit.h>

 

@interface HelloController : NSObject

{

    id helloButton;

    id messageRadio;

}

- (void)doAbout:(id)sender;

- (void)switchMessage:(id)sender;

@end
