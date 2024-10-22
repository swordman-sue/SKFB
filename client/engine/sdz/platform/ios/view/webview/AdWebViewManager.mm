//
//  AdWebViewManager.mm
//  AdWebView
//
//  Created by liwei on 13-9-9.
//  Copyright (c) 2013 shzx. All rights reserved.
//

#import "AdWebViewManager.h"
#import "AppController.h"
#import "RootViewController.h"
#include "zqcommon/logutil.h"


@interface NSURLRequest (Cache)
+ (int)GetCachePolicyByValue:(int)cachepolicy;
+ (int)GetTimeoutByValue:(int)timeout;
@end

@implementation NSURLRequest (Cache)

+ (int)GetCachePolicyByValue:(int)cachepolicy
{
    return cachepolicy;
}
+ (int)GetTimeoutByValue:(int)timeout
{
    return timeout;
}

@end

@interface NSString(code)

+ (NSString*)jsonStringWithCode:(int)code WithType:(NSString *)type WithMessage:(NSString *)message;
@end

@implementation NSString(code)
+ (NSString *)jsonStringWithCode:(int)code WithType:(NSString *)type WithMessage:(NSString *)message
{
    return [NSString stringWithFormat:@"{\"code\":\"%d\", \"tpye\":\"%@\", \"message\":\"%@\"}",
            code, type, message];
}
@end

@interface AdWebViewManager (PrivateMethods)

- (void)PrintDir:(NSString *)dirname;

- (void)resetWebMemoryCache;
- (int)showWebView:(NSData *)data baseRequest:(NSURLRequest *)theRequest rect:(CGRect)rect;
@end

@implementation AdWebViewManager (PrivateMethods)


static AppController * appDelegate = nil;
static RootViewController * theRootViewController = nil;


- (void)PrintDir:(NSString *)dirname
{
    if ([[NSFileManager defaultManager] fileExistsAtPath:dirname]) {
        //NSLog(@"Â·¾¶²»´æÔÚ: %@", dirname);
		return;
	}
}

- (void)resetWebMemoryCache {
    self.mShowingWebViewDictionary = [NSMutableDictionary dictionary];
    
}

- (int)showWebView:(NSData *)data baseRequest:(NSURLRequest *)theRequest rect:(CGRect)rect
{
    UIWebView * webview = [[UIWebView alloc] initWithFrame:rect];
    
    // create the UIWebView
    CGRect webFrame = rect;
    float kTweenMargin = 1.0;
    float kTextFieldHeight = 1.0;
    // leave room for the URL input field
    webFrame.origin.y += (kTweenMargin * 2.0) + kTextFieldHeight;
	webFrame.size.height -= 40.0;
    
	webview.backgroundColor = [UIColor clearColor];
	[webview setOpaque:YES];
	webview.scalesPageToFit = YES;
	webview.autoresizingMask = (UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleBottomMargin);
	webview.delegate = self;
    
	[webview loadRequest:[NSURLRequest requestWithURL:theRequest.URL]];
    
    int tag;
    do {
        tag = arc4random() % 100000000;
    } while (tag == 0 || [self.mShowingWebViewDictionary objectForKey:[NSNumber numberWithInt:tag]]);

    webview.tag = tag;
    [self.mShowingWebViewDictionary setObject:webview forKey:[NSNumber numberWithInt:tag]];
    [webview release];
    return tag;
}

- (BOOL)webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType
{
    return true;
}
- (void)webViewDidStartLoad:(UIWebView *)webView
{
    [UIApplication sharedApplication].networkActivityIndicatorVisible = YES;
}
- (void)webViewDidFinishLoad:(UIWebView *)webView
{
    [UIApplication sharedApplication].networkActivityIndicatorVisible = NO;
    if (![theRootViewController.view viewWithTag:webView.tag]) {
        appDelegate = (AppController *)[[UIApplication sharedApplication] delegate];
        theRootViewController = (RootViewController *)appDelegate.viewController;
        [theRootViewController.view addSubview:webView];
    }
}
- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error
{
    [UIApplication sharedApplication].networkActivityIndicatorVisible = NO;
    LogUtil::LogError("ERROR: Webview load fail, [%s]", [[error localizedDescription] UTF8String]);
}

@end

@implementation AdWebViewManager

@synthesize mShowingWebViewDictionary;
static AdWebViewManager * pInstance = nil;

+ (AdWebViewManager *)GetInstance
{
    if (!pInstance) {
        pInstance = [[AdWebViewManager alloc] init];
    }
    return pInstance;
}

- (id)init
{
	if ((self = [super init]))
    {
        [self resetWebMemoryCache];
    }
    return self;
}

- (int)ShowWebUrl:(NSString *)url WithCacheType:(int)cachepolicy WithTimeout:(NSTimeInterval)timeout WithRectString:(NSString *)rectString
{
    if (!url || [url isEqualToString:@""]) {
        return false;
    }
    
    NSURL * OjbcUrl = [NSURL URLWithString:url];
    NSURLRequest * request = [NSURLRequest requestWithURL:OjbcUrl
                                              cachePolicy:(NSURLRequestCachePolicy)[NSURLRequest GetCachePolicyByValue:cachepolicy]
                                          timeoutInterval:[NSURLRequest GetTimeoutByValue:timeout]];
    int tag = [self showWebView:NULL baseRequest:request rect:CGRectFromString(rectString)];
    return tag;
}

- (void)Refresh:(int)tag
{
    for (UIWebView * theWebview in [self.mShowingWebViewDictionary allValues]) {
        if (theWebview.tag == tag ) {
        	[theWebview reload];
            LogUtil::LogInfo("Info: Webview Refresh by tag: %d", tag);
        }
    }
}
	
- (bool)HideWebUrl:(int)tag isdistory:(bool)isdestory
{
    BOOL ret_val = false;
    BOOL forceclear = false;
    if (tag == 0) {
        forceclear = YES;
    }
    NSMutableArray * needRemoveKeys = [NSMutableArray array];
	for (NSNumber * theKey in [self.mShowingWebViewDictionary allKeys]) {
        UIWebView * webView = [self.mShowingWebViewDictionary objectForKey:theKey];
        if (![webView isKindOfClass:[UIWebView class]])
        {
            continue;
        }
        if (forceclear || webView.tag == tag) {
            if (webView.superview) {
                [webView removeFromSuperview];
                ret_val = true;
            }
            [needRemoveKeys addObject:theKey];
        }
    }
    
    for (NSNumber * theKey in needRemoveKeys) {
        [self.mShowingWebViewDictionary removeObjectForKey:theKey];
    }
    return ret_val;
}

- (void)NativeWebError:(NSString *)jsonError
{
	NSLog(@"%@", jsonError);
}

- (BOOL)removeUrlCache:(NSString *)host
{
    return false;
}

- (void)dealloc
{
    self.mShowingWebViewDictionary = nil;
    [super dealloc];
}

@end
