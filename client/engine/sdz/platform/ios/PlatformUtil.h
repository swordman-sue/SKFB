//
//  PlatformUtil.h
//  basesdk
//
//  Created by zlgame on 7/31/13.
//  Copyright (c) 2013 zlgame. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>
#import <UIKit/UIKit.h>

@class BaseSdk;

@interface PlatformUtil : NSObject
+(void) GotoBrowser:(NSString *) url;
+(void) ShowDialog:(NSString *)title setMessage: (NSString *) message setCancelText:(NSString *)cancelText setCertainText :(NSString *)certainText;
+(void) ShowNetworkDialog:(NSString *)title setMessage: (NSString *) message setCancelText:(NSString *)cancelText setCertainText :(NSString *)certainText;
+(BOOL) IsOpenLocationService;
+(BOOL) isEndWith:(NSString *) src_str :(NSString *) end_str;
+(void) OpenAlbum:(BaseSdk*)sdk;
+(void) OnAlbumResult:(BaseSdk*)sdk :(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info;
@end

