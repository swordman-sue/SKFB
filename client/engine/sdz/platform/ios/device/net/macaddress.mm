//
//  MacAddress.m
//  macaddres
//
//  Created by liwei on 13-9-9.
//  Copyright (c) 2013Äê shzx. All rights reserved.
//

#include "MacAddress.h"
#include <stdlib.h>
#include <sys/socket.h> // Per msqr
#include <sys/sysctl.h>
#include <net/if.h>
#include <net/if_dl.h>
#include "zqcommon/logutil.h"


#pragma mark MAC
// Return the local MAC addy
// Courtesy of FreeBSD hackers email list
// Accidentally munged during previous update. Fixed thanks to mlamb.
std::string MacAddress::GetMacAddress()
{
    int                 mib[6];
	size_t              len;
	char                *buf;
	unsigned char       *ptr;
	struct if_msghdr    *ifm;
	struct sockaddr_dl  *sdl;
	
	mib[0] = CTL_NET;
	mib[1] = AF_ROUTE;
	mib[2] = 0;
	mib[3] = AF_LINK;
	mib[4] = NET_RT_IFLIST;
	
	if ((mib[5] = if_nametoindex("en0")) == 0) {
		LogUtil::LogError("Error: if_nametoindex error/n");
		return "";
	}
	
	if (sysctl(mib, 6, NULL, &len, NULL, 0) < 0) {
		LogUtil::LogError("Error: sysctl, take 1/n");
		return "";
	}
	
	if ((buf = (char *)malloc(len)) == NULL) {
		LogUtil::LogError("Could not allocate memory. error!/n");
		return "";
	}
	
	if (sysctl(mib, 6, buf, &len, NULL, 0) < 0) {
		LogUtil::LogError("Error: sysctl, take 2");
		return "";
	}
	
	ifm = (struct if_msghdr *)buf;
	sdl = (struct sockaddr_dl *)(ifm + 1);
	ptr = (unsigned char *)LLADDR(sdl);
	NSString *outstring = [NSString stringWithFormat:@"%02x-%02x-%02x-%02x-%02x-%02x", *ptr, *(ptr+1), *(ptr+2), *(ptr+3), *(ptr+4), *(ptr+5)];
	free(buf);
	return std::string([[outstring uppercaseString] UTF8String]);
}
