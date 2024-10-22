//
//  MacroHelper.h
//  SDKTestDemo
//
//  Created by JorGames_Dingdedou on 14-4-17.
//  Copyright (c) 2014年 Jorgame. All rights reserved.
//

#ifndef SDKTestDemo_MacroHelper_h
#define SDKTestDemo_MacroHelper_h

#ifndef __IPHONE_5_0
#warning "This project uses features only available in iOS SDK 5.0 and later."
#endif

//#ifdef __OBJC__
//#import <UIKit/UIKit.h>
//#import <Foundation/Foundation.h>
//#endif

#ifdef DEBUG
#define __FileName__    [[NSString stringWithUTF8String:__FILE__] lastPathComponent]
#define __FUNCTION__      __PRETTY_FUNCTION__

#define DLog(fmt, ...) NSLog( @"<%p %@:%s(%d)> \n\n%@\n\n", self, __FileName__, __FUNCTION__, __LINE__, [NSString stringWithFormat:(fmt), ##__VA_ARGS__])
#else
#define DLog(...) {}
#endif


#ifndef MAIN_SCREEN_WIDTH
#define MAIN_SCREEN_WIDTH ([UIScreen mainScreen].bounds.size.width < [UIScreen mainScreen].bounds.size.height ? [UIScreen mainScreen].bounds.size.width : [UIScreen mainScreen].bounds.size.height)
#endif
#ifndef MAIN_SCREEN_HEIGHT
#define MAIN_SCREEN_HEIGHT ([UIScreen mainScreen].bounds.size.height > [UIScreen mainScreen].bounds.size.width ? [UIScreen mainScreen].bounds.size.height : [UIScreen mainScreen].bounds.size.width)
#endif


#ifndef IS_SYSTEM_VER_IOS6
#define IS_SYSTEM_VER_IOS6 ([UIDevice currentDevice].systemVersion.floatValue >= 6.0 && [UIDevice currentDevice].systemVersion.floatValue < 7.0)
#endif

#ifndef IS_SYSTEM_VER_IOS7
#define IS_SYSTEM_VER_IOS7 ([UIDevice currentDevice].systemVersion.floatValue >= 7.0 && [UIDevice currentDevice].systemVersion.floatValue < 8.0)
#endif

#ifndef IS_SYSTEM_IOS7_AND_LATER
#define IS_SYSTEM_IOS7_AND_LATER ([UIDevice currentDevice].systemVersion.floatValue >= 7.0)
#endif

#ifndef IS_DEVICE_IPAD
#define IS_DEVICE_IPAD ([UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPad)
#endif

#ifndef IS_DEVICE_DIRECTION_PORTRAIT
#define IS_DEVICE_DIRECTION_PORTRAIT ([UIApplication sharedApplication].statusBarOrientation == UIInterfaceOrientationPortrait || [UIApplication sharedApplication].statusBarOrientation == UIInterfaceOrientationPortraitUpsideDown)
#endif

#ifndef IS_DEVICE_IPHONE5_AND_LATER
#define IS_DEVICE_IPHONE5_AND_LATER ([UIDevice currentDevice].userInterfaceIdiom != UIUserInterfaceIdiomPad && [UIScreen mainScreen].currentMode.size.height >= 568*2)
#endif


#endif
