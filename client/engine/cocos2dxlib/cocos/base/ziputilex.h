#ifndef ZIPUTILEX_H
#define ZIPUTILEX_H

#include "platform/CCPlatformMacros.h"


NS_CC_BEGIN

class CC_DLL ZipUtilEx
{
public:
	CC_DEPRECATED_ATTRIBUTE static int SDZInflateMemory(unsigned char *target, int target_size, unsigned char *source, int source_size);

};

NS_CC_END

#endif

