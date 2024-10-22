//
//  PushApplication.h
//  basesdk
//
//  Created by administrator on 13-9-7.
//  Copyright (c) 2013年 zlgame. All rights reserved.
//

#import <UIKit/UIKit.h>
#if !defined(__NO_JPUSH__)
#import "APService.h"
#endif
#import "PushManager.h"

@interface PushApplication : NSObject <UIAccelerometerDelegate, UIAlertViewDelegate, UITextFieldDelegate, UIApplicationDelegate>
{
    NSDictionary * globalLaunchOptions;
}
@property (nonatomic, retain) NSDictionary * globalLaunchOptions;

+ (void) PushInit:(id) delegate :(NSDictionary *)launchOptions;

@end
