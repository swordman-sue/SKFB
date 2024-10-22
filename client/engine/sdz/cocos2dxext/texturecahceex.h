#ifndef __TEXTURE_CACHE_EX_H__
#define __TEXTURE_CACHE_EX_H__

#include "cocos2d.h"

typedef void (*LUA_CALLBACK_DATA)(void*);

//������չCCTextureCahce
class TextureCacheEx : public cocos2d::CustomTextureCache
{
public:
	~TextureCacheEx();
	static TextureCacheEx* GetInstance();
	static void DelInstance();

	void AddSdzImageAsync(const char* path, LUA_CALLBACK_DATA callback_func, void* callback_data);

	bool Init();
	bool Stop();

	// ��̬��ѹ�����Դ
	virtual void unzipInitResDynamically(const char* file_name);

	// ��ȡ����key
	virtual std::string getTextureKey(const char* full_path);

private:
	TextureCacheEx();
	void AddSdzImageAsyncCallBack(float dt);
};

#endif