//
//  VoiceConverter.h
//  Jeans
//
//  Created by Jeans Huang on 12-7-22.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface VoiceConverter : NSObject

+(int) amrToWav:(NSString*)_amrPath :(NSString*)_savePath;

+(int) wavToAmr:(NSString*)_wavPath :(NSString*)_savePath;


@end
