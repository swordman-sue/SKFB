//
//  BaseSdk.h
//  basesdk
//
//  Created by zlgame on 7/31/13.
//  Copyright (c) 2013 zlgame. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

@interface GpsLocation : NSObject<CLLocationManagerDelegate>
{
    NSTimer *timer;
    BOOL isListern;
}
@property (retain,nonatomic) CLLocationManager *locationManager;
@property (retain,nonatomic) CLLocation *currentLocation;
-(id) init;
-(void) StartLocation:(NSString *) param;
-(void) StopLocation;
-(void) OnChangedLocationCallback:(int) code :(NSString *) data;
-(void) StartTimer:(int) timeout;
-(void) CancelTimer;
-(void) TimeTask;
@end
