//
//  IpaInstallController.m
//  project1
//
//  Created by liwei on 13-9-8.
//  Copyright (c) 2013? shzx. All rights reserved.
//

#import "IpaInstallController.h"
#import "IpaInstall.h"

@implementation IpaInstallController

- (id)initwithPath:(NSString *)path WithTitleText:(NSString *)titleText
{
	if ((self = [super init])) {
        self.titleText = titleText;
		self.path = path;
    }
    return self;
}

- (void)viewDidLoad
{
	[super viewDidLoad];
}

- (IBAction)installShowIndicator
{
//    AppController * appDelegate = (AppController *)[[UIApplication sharedApplication] delegate];
//    UIViewController * viewControler = (UIViewController *)[appDelegate viewController];
//    
//    UIActivityIndicatorView * activityIndicatorView = [[[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge] autorelease];
//    activityIndicatorView.center = CGPointMake(viewControler.view.frame.size.width/2, viewControler.view.frame.size.height/2 - activityIndicatorView.frame.size.height);
//    activityIndicatorView.hidesWhenStopped = YES;
//    [activityIndicatorView startAnimating];
//    [viewControler.view addSubview:activityIndicatorView];
//    self.activityIndicator = activityIndicatorView;
//    
//    UILabel * titleIndicator = [[[UILabel alloc] initWithFrame:CGRectMake(0, 0, viewControler.view.frame.size.width, 24)] autorelease];
//    [titleIndicator setBackgroundColor:[UIColor clearColor]];
//    [titleIndicator setTextColor:[UIColor whiteColor]];
//    [titleIndicator setText:[NSString stringWithFormat:@"%@", self.titleText]];
//    [titleIndicator setCenter:CGPointMake(viewControler.view.frame.size.width/2, viewControler.view.frame.size.height/2 + activityIndicatorView.frame.size.height/2)];
//    [titleIndicator setTextAlignment:NSTextAlignmentCenter];
//    [viewControler.view addSubview:titleIndicator];
//    self.titleIndicator = titleIndicator;
//    
//    UIButton * button = [[UIButton alloc] initWithFrame:viewControler.view.frame];
//    [button setBackgroundColor:[UIColor clearColor]];
//    [viewControler.view addSubview:button];
//    self.buttonIndicator = button;
    
    [self install];
}

- (IBAction)install
{
    [NSThread detachNewThreadSelector:@selector(threadInMainMethod:) toTarget:self withObject:self.path];
}

- (void) threadInMainMethod:(id)sender
{
	NSString * path = (NSString *)sender;
    int ret = [IpaInstall IPAInstall:path];
    NSLog(@"ret %d, install:%@", ret, path);
    
    [self.activityIndicator stopAnimating];
    [self.activityIndicator removeFromSuperview];
    [self.titleIndicator removeFromSuperview];
    [self.buttonIndicator removeFromSuperview];
    // PlatformTools.callback(ret);
}

- (void)dealloc
{
    self.titleIndicator = nil;
    self.titleText = nil;
    self.activityIndicator = nil;
    self.buttonIndicator = nil;
    self.path = nil;
    [super dealloc];
}


@end
