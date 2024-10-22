//
//  AdWebViewManager.h
//  AdWebView
//
//  Created by liwei on 13-9-9.
//  Copyright (c) 2013 shzx. All rights reserved.
//
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@interface AdWebViewManager: NSObject <UIWebViewDelegate>
{
    NSError *error;
}

+ (AdWebViewManager *)GetInstance;

- (int)ShowWebUrl:(NSString *)url WithCacheType:(int)cachepolicy WithTimeout:(NSTimeInterval)timeout WithRectString:(NSString *)rectString;

- (void)Refresh:(int)tag;
	
- (bool)HideWebUrl:(int)tag isdistory:(bool)isdestory;

- (BOOL)removeUrlCache:(NSString *)host;

@property (nonatomic, retain) NSMutableDictionary * mShowingWebViewDictionary;


@end


