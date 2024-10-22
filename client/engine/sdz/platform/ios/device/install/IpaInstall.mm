//
//  IpaInstall.m
//  project1
//
//  Created by liwei on 13-9-8.
//  Copyright (c) 2013? shzx. All rights reserved.
//

#import "IpaInstall.h"

#ifndef __ZL_APPSTORE__

#import "dlfcn.h"
typedef int (*MobileInstallationInstall)(NSString *path, NSDictionary *dict, void *na, NSString *path2_equal_path_maybe_no_use);

#endif

@implementation IpaInstall

+ (bool)IPAInstall:(NSString *)path
{

#ifndef __ZL_APPSTORE__

    void *lib = dlopen("/System/Library/PrivateFrameworks/MobileInstallation.framework/MobileInstallation", RTLD_LAZY);
    if (lib)
    {
        MobileInstallationInstall pMobileInstallationInstall = (MobileInstallationInstall)dlsym(lib, "MobileInstallationInstall");
        if (pMobileInstallationInstall){
            int ret = pMobileInstallationInstall(path, [NSDictionary dictionaryWithObject:@"User" forKey:@"ApplicationType"], nil, path);
            dlclose(lib);
            return ret == 0;
        }
    }

#endif
    return false;
}

@end
