#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <CoreAudio/CoreAudioTypes.h>
@interface VoiceOperation : NSObject
{
    
}
+(int) StartRecord:(NSString *) full_file_name :(int) secord;
+(int) StopRecord;
+(int) StartPlay:(NSString *) full_file_name :(unsigned int) voice_id :(unsigned int) lua_fun_id;
+(int) StopPlay;
+(int) ReleaseVoice;
+(void) PlayFinishCallBack:(int)code;
+(void)stopTimer;
+ (void)VoicePlayRelease;
@end
