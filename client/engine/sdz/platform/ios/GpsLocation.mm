//
//  BaseSdk.m
//  basesdk
//
//  Created by zlgame on 7/31/13.
//  Copyright (c) 2013 zlgame. All rights reserved.
//

#import "GpsLocation.h"
#import "location/location.h"
#include "luaengine/luaengine.h"
#import <UIKit/UIKit.h>
#import "PlatformUtil.h"
@implementation GpsLocation


@synthesize locationManager;
@synthesize currentLocation;

-(id)init
{
    self = [super init];
    if (self)
    {
        isListern = NO;
    }
    return self;
}

-(void) StartLocation:(NSString *) param
{
    currentLocation = nil;
    
    if(isListern)
    {
        return ;
    }
    isListern = YES;
    
    if ([PlatformUtil IsOpenLocationService] == YES)
    {
     int timeout = [param intValue];
     [self StartTimer:timeout];
        
     if (locationManager == nil)
     {
         locationManager = [[CLLocationManager alloc] init];
     }
     if(locationManager)
     {
         //设置位置精度
         locationManager.desiredAccuracy = kCLLocationAccuracyBest;
         locationManager.delegate = self;
         //距离过滤，表示在地图上每隔50更新一次定位
         locationManager.distanceFilter = 50;
         //启动位置管理器，进行定位服务
         [locationManager startUpdatingLocation];
     }
    }else
    {
        [self OnChangedLocationCallback:-1:@"{\"msg\":\"定位服务未打开\"}"];
    }
    
    
}
-(void) TimeTask
{
    [self CancelTimer];
    
    [self OnChangedLocationCallback:-2:@"{\"msg\":\"定位超时\"}"];
    
}

-(void)StartTimer:(int) timeout
{
    [self CancelTimer];
    //时间间隔
    NSTimeInterval timeInterval = timeout ;
    //定时器    repeats 表示是否需要重复，NO为只重复一次
    timer = [NSTimer scheduledTimerWithTimeInterval:timeInterval target:self selector:@selector(TimeTask) userInfo:nil repeats:NO];
}
-(void)CancelTimer
{
    if (timer&& timer.isValid)
    {
        [timer invalidate];
        timer = nil;
    }
}

-(void) StopLocation
{
    [self CancelTimer];
    if (locationManager != nil)
    {
       [locationManager stopUpdatingLocation];
       [locationManager release];
       locationManager = nil;
    }
    isListern = NO;
    currentLocation = nil;
}
//获取位置信息
-(void)locationManager:(CLLocationManager *)manager didUpdateToLocation:(CLLocation *)newLocation fromLocation:(CLLocation *)oldLocation
{
    currentLocation = newLocation;
    
    NSString *latitude = [NSString stringWithFormat:@"%g",currentLocation.coordinate.latitude];
    NSString *longitude = [NSString stringWithFormat:@"%g",currentLocation.coordinate.longitude];
    NSString * data = [NSString stringWithFormat:@"{\"latitude\":\"%@\",\"longitude\":\"%@\"}",latitude,longitude];
    [self OnChangedLocationCallback:0:data];
   
}
//当设备无法定位当前我位置时候调用此方法
-(void)locationManager:(CLLocationManager *)manager didFailWithError:(NSError *)error
{
    if (error.code == kCLErrorDenied)
    {
        [self OnChangedLocationCallback:-1:@"{\"msg\":\"应用无定位权限\"}"];
    }else
    {
        [self OnChangedLocationCallback:-1:@"{\"msg\":\"未知原因\"}"];
    }
    
}
-(void) OnChangedLocationCallback:(int) code :(NSString *) data
{
    const char * datainfo = [data UTF8String];
    Location::GetInstance()->CallbackLocation(code,datainfo);
}
-(void) dealloc
{
    [self StopLocation];
    [super dealloc];
}


@end
