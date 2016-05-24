#import "HelloController.h"

 

@implementation HelloController

 

- (void)doAbout:(id)sender

{

    NSRunAlertPanel(@"About",@"Welcome to Hello World!",@"OK",NULL,NULL);

}

 

- (IBAction)switchMessage:(id)sender

{

        int which=[sender selectedRow]+1;

        [helloButton setAction:NSSelectorFromString([NSString stringWithFormat:@"%@%d:",@"message",which])];

}

 

- (void)awakeFromNib

{

    [[helloButton window] makeKeyAndOrderFront:self];

}

 

@end
