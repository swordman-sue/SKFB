#ifndef __SUPPORT_ZIPUTILSEX_H__
#define __SUPPORT_ZIPUTILSEX_H__

class ZipUtilsEx
{
public:
	static int InflateGZipMemory(unsigned char *target, int target_size, unsigned char *source, int source_size);
};

#endif

