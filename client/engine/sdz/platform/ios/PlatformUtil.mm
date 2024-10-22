//
//  PlatformUtil.m
//  basesdk
//
//  Created by zlgame on 7/31/13.
//  Copyright (c) 2013 zlgame. All rights reserved.
//

#import "PlatformUtil.h"
#import "platform/sdktools.h"
#import "filesystem/filesystemex.h"
#import "gameengine/gameengine.h"
#import "BaseSdk.h"
#import "SBJson.h"

@interface AlertViewDelegate : NSObject<UIAlertViewDelegate>
{
}
@property BOOL bForNetworkAlert;
@property BOOL bNoCancel;

-(void) alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex;
@end

@implementation AlertViewDelegate

-(void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if(_bNoCancel == YES)
    {
        buttonIndex = 1;
    }
    
    if (_bForNetworkAlert == YES)
    {
        if(SdkTools::network_dialog_call_back_pointer != 0)
        {
            SdkTools::network_dialog_call_back_pointer(buttonIndex);
        }
    }
    else
    {
        if(SdkTools::dialog_call_back_pointer != 0)
        {
            SdkTools::dialog_call_back_pointer(buttonIndex);
        }
        if (SdkTools::dlg_certain_fun_id != 0 && buttonIndex == 1)
        {
            LuaEngine::GetInstance()->CallFunction(SdkTools::dlg_certain_fun_id, "");
        }
        if (SdkTools::dlg_cancel_fun_id != 0 && buttonIndex == 0)
        {
            LuaEngine::GetInstance()->CallFunction(SdkTools::dlg_cancel_fun_id, "");
        }
    }
    
    [self release];
}

@end


@implementation PlatformUtil


+(void) ShowDialog:(NSString *)title setMessage: (NSString *) message setCancelText:(NSString *)cancelText
   setCertainText :(NSString *) certainText
{
    AlertViewDelegate *alertViewDelegate = [[AlertViewDelegate alloc] init];
    if ([cancelText isEqual:@""] == YES)
    {
        cancelText = nil;
        alertViewDelegate.bNoCancel = YES;
    }else
    {
        alertViewDelegate.bNoCancel = NO;
    }
        
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:title
                                                    message:message
                                                   delegate:alertViewDelegate
                                          cancelButtonTitle:cancelText
                                          otherButtonTitles:certainText,nil];
    [alert show];
    [alert release];
}

// 专用于网络检测提示框（避免与ShowDialog冲突）
+(void) ShowNetworkDialog:(NSString *)title setMessage: (NSString *) message setCancelText:(NSString *)cancelText
   setCertainText :(NSString *) certainText
{
    AlertViewDelegate *alertViewDelegate = [[AlertViewDelegate alloc] init];
    alertViewDelegate.bForNetworkAlert = YES;
    if ([cancelText isEqual:@""] == YES)
    {
        cancelText = nil;
        alertViewDelegate.bNoCancel = YES;
    }else
    {
        alertViewDelegate.bNoCancel = NO;
    }
    
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:title
                                                    message:message
                                                   delegate:alertViewDelegate
                                          cancelButtonTitle:cancelText
                                          otherButtonTitles:certainText,nil];
    [alert show];
    [alert release];
}

+(void) GotoBrowser:(NSString *) url
{
    [[UIApplication sharedApplication] openURL:
     [NSURL URLWithString:[NSString stringWithFormat:@"%@", url]]];
}

+(BOOL) IsOpenLocationService
{
    if([CLLocationManager locationServicesEnabled] && [CLLocationManager authorizationStatus]
       != kCLAuthorizationStatusDenied)
    {
        return YES;
    }else
    {
        return  NO;
    }
}

+(BOOL) isEndWith:(NSString *) src_str :(NSString *) end_str
{
    if(!src_str || !end_str)
    {
        return NO;
    }
    int src_str_len = [src_str length];
    int end_str_len = [end_str length];
    NSString *temp_str = [src_str substringFromIndex:src_str_len-end_str_len];
    if([end_str isEqualToString:temp_str])
    {
        return YES;
    }else
    {
        return NO;
    }
    
}

// 打开相册
+(void) OpenAlbum:(BaseSdk*)sdk
{
    UIImagePickerController * picker = [[UIImagePickerController alloc]init];
    if ([UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypePhotoLibrary])
    {
        // 打开图库
        picker.delegate = sdk;
        picker.sourceType = UIImagePickerControllerSourceTypePhotoLibrary;
        [[sdk viewController] presentViewController:picker animated:YES completion:nil];
    }
    else if([UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypeSavedPhotosAlbum])
    {
        // 打开相册
        picker.delegate = sdk;
        picker.sourceType = UIImagePickerControllerSourceTypeSavedPhotosAlbum;
        [[sdk viewController] presentViewController:picker animated:YES completion:nil];
    }
    else
    {
        NSLog(@"[SystemTools::OpenAlbum] 打开相册失败");
    }
}

// 相册图片选中回调
+(void) OnAlbumResult:(BaseSdk*)sdk :(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info
{
    // 获取图片数据并保存到沙盒目录
    NSString* imgSuffix = @"jpg";
    UIImage* image = info[UIImagePickerControllerOriginalImage];
    NSData* imgData = UIImageJPEGRepresentation(image, 0.5);
    if (imgData == nil)
    {
        imgSuffix = @"png";
        imgData = UIImagePNGRepresentation(image);
    }
    std::string tmpPath = FileSystemEx::GetInstance()->GetTmpPath();
    unsigned int nowTimeMilli = GameEngine::GetInstance()->GetNowTimeMilli();
    NSString* tmpImgPath = [NSString stringWithFormat:@"%s%u.%@", tmpPath.c_str(), nowTimeMilli, imgSuffix];
    BOOL ret = [imgData writeToFile:tmpImgPath atomically:NO];
    if (ret)
    {
        // 回调给游戏并上传图片
        SBJsonWriter* jsonWriter = [[SBJsonWriter alloc] init];
        [jsonWriter autorelease];
        NSMutableDictionary* jsonDict = [NSMutableDictionary dictionary];
        [jsonDict setObject:FuncType_PICTRUE_EVENT_SEND forKey:@"type"];
        [jsonDict setObject:tmpImgPath forKey:@"path"];
        NSString* retData = [jsonWriter stringWithObject:jsonDict];
        [BaseSdk SDKCommonHandleCallBack:SDK_COMMON_CALLFUNC_SUCCESS :retData];
    }

    // 跳转回游戏
    [[sdk viewController] dismissViewControllerAnimated:YES completion:nil];
}

@end
