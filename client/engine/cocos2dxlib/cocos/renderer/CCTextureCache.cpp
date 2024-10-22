/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "renderer/CCTextureCache.h"

#include <errno.h>
#include <stack>
#include <cctype>
#include <list>

#include "renderer/CCTexture2D.h"
#include "base/ccMacros.h"
#include "base/ccUTF8.h"
#include "base/CCDirector.h"
#include "base/CCScheduler.h"
#include "platform/CCFileUtils.h"
#include "base/ccUtils.h"
#include "base/CCNinePatchImageParser.h"



using namespace std;

NS_CC_BEGIN

std::string TextureCache::s_etc1AlphaFileSuffix = "@alpha";

// implementation TextureCache

void TextureCache::setETC1AlphaFileSuffix(const std::string& suffix)
{
    s_etc1AlphaFileSuffix = suffix;
}

TextureCache * TextureCache::getInstance()
{
    return Director::getInstance()->getTextureCache();
}

TextureCache::TextureCache()
: _loadingThread(nullptr)
, _needQuit(false)
, _asyncRefCount(0)
, print_load_log(false)
, _custom_texture_cache(nullptr)
{
}

TextureCache::~TextureCache()
{
    CCLOGINFO("deallocing TextureCache: %p", this);


	//for (auto it = _textures.begin(); it != _textures.end(); ++it)
	//{
	//	(it->second)->release();
	//}

	// edit by swordman sue 2016-7-29
	for (auto it = _textures.begin(); it != _textures.end(); ++it)
	{
		(it->second)->setAutoClearCache(false);
		(it->second)->release();
	}
	_textures.clear();

    CC_SAFE_DELETE(_loadingThread);
}

void TextureCache::destroyInstance()
{
}

TextureCache * TextureCache::sharedTextureCache() 
{
    return Director::getInstance()->getTextureCache();
}

void TextureCache::purgeSharedTextureCache() 
{ 
}

std::string TextureCache::getDescription() const
{
    return StringUtils::format("<TextureCache | Number of textures = %d>", static_cast<int>(_textures.size()));
}

struct TextureCache::AsyncStruct
{
public:
    AsyncStruct(const std::string& fn,const std::function<void(Texture2D*)>& f) 
	: filename(fn)
	, callback(f)
	, pixelFormat(Texture2D::getDefaultAlphaPixelFormat())
	, loadSuccess(false) 
	, image(nullptr)
	, imageAlpha(nullptr)
	{}

	~AsyncStruct()
	{
		CC_SAFE_RELEASE_NULL(image);
		CC_SAFE_RELEASE_NULL(imageAlpha);
	}

    std::string filename;
    std::function<void(Texture2D*)> callback;
    Image* image;
    Image* imageAlpha;
    Texture2D::PixelFormat pixelFormat;
    bool loadSuccess;
};

/**
 The addImageAsync logic follow the steps:
 - find the image has been add or not, if not add an AsyncStruct to _requestQueue  (GL thread)
 - get AsyncStruct from _requestQueue, load res and fill image data to AsyncStruct.image, then add AsyncStruct to _responseQueue (Load thread)
 - on schedule callback, get AsyncStruct from _responseQueue, convert image to texture, then delete AsyncStruct (GL thread)
 
 the Critical Area include these members:
 - _requestQueue: locked by _requestMutex
 - _responseQueue: locked by _responseMutex
 
 the object's life time:
 - AsyncStruct: construct and destruct in GL thread
 - image data: new in Load thread, delete in GL thread(by Image instance)
 
 Note:
 - all AsyncStruct referenced in _asyncStructQueue, for unbind function use.
 
 How to deal add image many times?
 - At first, this situation is abnormal, we only ensure the logic is correct.
 - If the image has been loaded, the after load image call will return immediately.
 - If the image request is in queue already, there will be more than one request in queue,
 - In addImageAsyncCallback, will deduplicate the request to ensure only create one texture.
 
 Does process all response in addImageAsyncCallback consume more time?
 - Convert image to texture faster than load image from disk, so this isn't a problem.
 */
void TextureCache::addImageAsync(const std::string &path, const std::function<void(Texture2D*)>& callback)
{
    Texture2D *texture = nullptr;

    std::string fullpath = FileUtils::getInstance()->fullPathForFilename(path);

	// 纹理key add by swordman sue 2017-6-5
	std::string texKey = getTextureKey(fullpath.c_str());

	auto it = _textures.find(texKey);
    if( it != _textures.end() )
        texture = it->second;

    if (texture != nullptr)
    {
        if (callback) callback(texture);
        return;
    }

    // check if file exists
    if ( fullpath.empty() || ! FileUtils::getInstance()->isFileExist( fullpath ) ) {
        if (callback) callback(nullptr);
        return;
    }

    // lazy init
    if (_loadingThread == nullptr)
    {
        // create a new thread to load images
        _loadingThread = new (std::nothrow) std::thread(&TextureCache::loadImage, this);
        _needQuit = false;
    }

    if (0 == _asyncRefCount)
    {
        Director::getInstance()->getScheduler()->schedule(CC_SCHEDULE_SELECTOR(TextureCache::addImageAsyncCallBack), this, 0, false);
    }

    ++_asyncRefCount;

    // generate async struct
    AsyncStruct *data = new (std::nothrow) AsyncStruct(fullpath, callback);
    
    // add async struct into queue
    _asyncStructQueue.push_back(data);
    _requestMutex.lock();
    _requestQueue.push_back(data);
    _requestMutex.unlock();

    _sleepCondition.notify_one();

	// 动态解压随包资源 add by swordman sue 2017-6-5
	if (nullptr != _custom_texture_cache)
	{
		_custom_texture_cache->unzipInitResDynamically(path.c_str());
	}
}

void TextureCache::unbindImageAsync(const std::string& filename)
{
    if (_asyncStructQueue.empty())
    {
        return;
    }
    std::string fullpath = FileUtils::getInstance()->fullPathForFilename(filename);
    for (auto& asyncStruct : _asyncStructQueue)
    {
        if (asyncStruct->filename == fullpath)
        {
            asyncStruct->callback = nullptr;
        }
    }
}

void TextureCache::unbindAllImageAsync()
{
    if (_asyncStructQueue.empty())
    {
        return;

    }
    for (auto& asyncStruct : _asyncStructQueue)
    {
        asyncStruct->callback = nullptr;
    }
}

void TextureCache::loadImage()
{
    AsyncStruct *asyncStruct = nullptr;
    std::mutex signalMutex;
    std::unique_lock<std::mutex> signal(signalMutex);
    while (!_needQuit)
    {
        // pop an AsyncStruct from request queue
        _requestMutex.lock();
        if (_requestQueue.empty())
        {
            asyncStruct = nullptr;
        }
        else
        {
            asyncStruct = _requestQueue.front();
            _requestQueue.pop_front();
        }
        _requestMutex.unlock();
        
        if (nullptr == asyncStruct) {
            _sleepCondition.wait(signal);
            continue;
        }
        
        // load image
		asyncStruct->image = new (std::nothrow) Image();
		if (asyncStruct->image)
		{
			asyncStruct->loadSuccess = asyncStruct->image->initWithImageFileThreadSafe(asyncStruct->filename);
		}

        // ETC1 ALPHA supports.
        if (asyncStruct->loadSuccess && asyncStruct->image->getFileType() == Image::Format::ETC && !s_etc1AlphaFileSuffix.empty())
        { // check whether alpha texture exists & load it
            auto alphaFile = asyncStruct->filename + s_etc1AlphaFileSuffix;
			if (FileUtils::getInstance()->isFileExist(alphaFile))
			{
				asyncStruct->imageAlpha = new (std::nothrow) Image();
				if (asyncStruct->imageAlpha)
				{
					asyncStruct->imageAlpha->initWithImageFileThreadSafe(alphaFile);
				}
			}
        }
        // push the asyncStruct to response queue
        _responseMutex.lock();
        _responseQueue.push_back(asyncStruct);
        _responseMutex.unlock();
    }
}

void TextureCache::addImageAsyncCallBack(float /*dt*/)
{
    Texture2D *texture = nullptr;
    AsyncStruct *asyncStruct = nullptr;
    while (true)
    {
        // pop an AsyncStruct from response queue
        _responseMutex.lock();
        if(_responseQueue.empty())
        {
            asyncStruct = nullptr;
        }else
        {
            asyncStruct = _responseQueue.front();
            _responseQueue.pop_front();
            
            // the asyncStruct's sequence order in _asyncStructQueue must equal to the order in _responseQueue
            CC_ASSERT(asyncStruct == _asyncStructQueue.front());
            _asyncStructQueue.pop_front();
        }
        _responseMutex.unlock();
        
        if (nullptr == asyncStruct) {
            break;
        }

		// 纹理key add by swordman sue 2017-6-5
		std::string texKey = getTextureKey(asyncStruct->filename.c_str());

        // check the image has been convert to texture or not
		auto it = _textures.find(texKey);
        if(it != _textures.end())
        {
            texture = it->second;
        }
        else
        {
            // convert image to texture
            if (asyncStruct->loadSuccess)
            {
                Image* image = asyncStruct->image;
                // generate texture in render thread
                texture = new (std::nothrow) Texture2D();

                texture->initWithImage(image, asyncStruct->pixelFormat);
				texture->setTexturePath(texKey);

				//parse 9-patch info
                this->parseNinePatchImage(image, texture, asyncStruct->filename);

#if CC_ENABLE_CACHE_TEXTURE_DATA
                // cache the texture file name
				VolatileTextureMgr::addImage(texture, image);
                //VolatileTextureMgr::addImageTexture(texture, asyncStruct->filename);
#endif
                // cache the texture. retain it, since it is added in the map
				_textures.emplace(texKey, texture);
                texture->retain();
                texture->autorelease();
				
                // ETC1 ALPHA supports.
				if (asyncStruct->imageAlpha && asyncStruct->imageAlpha->getFileType() == Image::Format::ETC) {
                    auto alphaTexture = new(std::nothrow) Texture2D();
                    if(alphaTexture != nullptr && alphaTexture->initWithImage(asyncStruct->imageAlpha, asyncStruct->pixelFormat)) {
                        texture->setAlphaTexture(alphaTexture);
                    }
                    CC_SAFE_RELEASE(alphaTexture);
                }
            }
            else {
                texture = nullptr;
                CCLOG("cocos2d: failed to call TextureCache::addImageAsync(%s)", asyncStruct->filename.c_str());
            }
        }
        
        // call callback function
        if (asyncStruct->callback)
        {
            (asyncStruct->callback)(texture);
        }

        // release the asyncStruct
        delete asyncStruct;
        --_asyncRefCount;
    }

    if (0 == _asyncRefCount)
    {
        Director::getInstance()->getScheduler()->unschedule(CC_SCHEDULE_SELECTOR(TextureCache::addImageAsyncCallBack), this);
    }
}


const std::string AnsiToUtf8(const char *ansi_str)
{
#ifdef _WIN32
	if (!ansi_str)
	{
		return "";
	}
	//AnsiToUnicode
	int len;
	wchar_t * unicode_result;
	len = MultiByteToWideChar(CP_ACP, 0, ansi_str, -1, NULL, 0);
	unicode_result = (wchar_t *)malloc((len + 1)*sizeof(wchar_t));
	memset(unicode_result, 0, (len + 1)*sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, ansi_str, -1, (LPWSTR)unicode_result, len);

	//UnicodeToUtf8
	len = WideCharToMultiByte(CP_UTF8, 0, unicode_result, -1, NULL, 0, NULL, NULL);
	char *utf8_result = (char *)malloc((len + 1)*sizeof(char));
	memset(utf8_result, 0, sizeof(char)* (len + 1));
	WideCharToMultiByte(CP_UTF8, 0, unicode_result, -1, utf8_result, len, NULL, NULL);
	std::string utf8_str = utf8_result;
	free(unicode_result);
	free(utf8_result);
	return utf8_str;
#endif
	return ansi_str;
}

Texture2D * TextureCache::addImage(const std::string &path)
{
    Texture2D * texture = nullptr;
    Image* image = nullptr;
    // Split up directory and filename
    // MUTEX:
    // Needed since addImageAsync calls this method from a different thread

    std::string fullpath = FileUtils::getInstance()->fullPathForFilename(path);
    if (fullpath.size() == 0)
    {
        return nullptr;
    }

	// 纹理key add by swordman sue 2017-6-5
	std::string texKey = getTextureKey(fullpath.c_str());

	auto it = _textures.find(texKey);
    if( it != _textures.end() )
        texture = it->second;

    if (! texture)
	{
		if (print_load_log)
		{
			CCLOG("TextureCache::addImage file:%s", AnsiToUtf8(path.c_str()).c_str());
		}
        // all images are handled by UIImage except PVR extension that is handled by our own handler
        do 
        {
            image = new (std::nothrow) Image();
            CC_BREAK_IF(nullptr == image);

            bool bRet = image->initWithImageFile(fullpath);
            CC_BREAK_IF(!bRet);

            texture = new (std::nothrow) Texture2D();

            if( texture && texture->initWithImage(image) )
            {
#if CC_ENABLE_CACHE_TEXTURE_DATA
                // cache the texture file name
				VolatileTextureMgr::addImage(texture, image);
                //VolatileTextureMgr::addImageTexture(texture, fullpath);
#endif
				texture->setTexturePath(texKey);

                // texture already retained, no need to re-retain it
				_textures.emplace(texKey, texture);

                //-- ANDROID ETC1 ALPHA SUPPORTS.
                std::string alphaFullPath = path + s_etc1AlphaFileSuffix;
                if (image->getFileType() == Image::Format::ETC && !s_etc1AlphaFileSuffix.empty() && FileUtils::getInstance()->isFileExist(alphaFullPath))
                {
                    Image alphaImage;
                    if (alphaImage.initWithImageFile(alphaFullPath))
                    {
                        Texture2D *pAlphaTexture = new(std::nothrow) Texture2D;
                        if(pAlphaTexture != nullptr && pAlphaTexture->initWithImage(&alphaImage)) {
                            texture->setAlphaTexture(pAlphaTexture);
                        }
                        CC_SAFE_RELEASE(pAlphaTexture);
                    }
                }

                //parse 9-patch info
                this->parseNinePatchImage(image, texture, path);
            }
            else
            {
                CCLOG("cocos2d: Couldn't create texture for file:%s in TextureCache", path.c_str());
                CC_SAFE_RELEASE(texture);
                texture = nullptr;
            }
        } while (0);
    }

    CC_SAFE_RELEASE(image);

    return texture;
}

void TextureCache::parseNinePatchImage(cocos2d::Image *image, cocos2d::Texture2D *texture, const std::string& path)
{
    if(NinePatchImageParser::isNinePatchImage(path))
    {
        Rect frameRect = Rect(0,0,image->getWidth(), image->getHeight());
        NinePatchImageParser parser(image, frameRect, false);
        texture->addSpriteFrameCapInset(nullptr, parser.parseCapInset());
    }
}

Texture2D* TextureCache::addImage(Image *image, const std::string &key)
{
    CCASSERT(image != nullptr, "TextureCache: image MUST not be nil");
    CCASSERT(image->getData() != nullptr, "TextureCache: image MUST not be nil");

    Texture2D * texture = nullptr;

    do
    {
		// 纹理key add by swordman sue 2017-6-5
		std::string fullpath = FileUtils::getInstance()->fullPathForFilename(key);
		std::string texKey = getTextureKey(fullpath.c_str());
		texKey = texKey != "" ? texKey : key;

		auto it = _textures.find(texKey);
        if( it != _textures.end() ) {
            texture = it->second;
            break;
		}

		if (print_load_log)
		{
			CCLOG("TextureCache::addImage file:%s", AnsiToUtf8(texKey.c_str()).c_str());
		}

        // prevents overloading the autorelease pool
        texture = new (std::nothrow) Texture2D();
        if(texture)
        {
           if (texture->initWithImage(image))
            {
			    _textures.emplace(texKey, texture);
			    texture->setTexturePath(texKey);
	            texture->retain();
	            texture->autorelease();
            }
            else
            {
                CC_SAFE_RELEASE(texture);
                texture = nullptr;
                CCLOG("cocos2d: initWithImage failed!");
            }
					
        }
		else
		{
			CCLOG("cocos2d: Allocating memory for Texture2D failed!");
		}

    } while (0);
    
#if CC_ENABLE_CACHE_TEXTURE_DATA
    VolatileTextureMgr::addImage(texture, image);
#endif
    
    return texture;
}

bool TextureCache::reloadTexture(const std::string& fileName)
{
    Texture2D * texture = nullptr;
    Image * image = nullptr;

    std::string fullpath = FileUtils::getInstance()->fullPathForFilename(fileName);
    if (fullpath.size() == 0)
    {
        return false;
    }

	// 纹理key add by swordman sue 2017-6-5
	std::string texKey = getTextureKey(fullpath.c_str());

	auto it = _textures.find(texKey);
    if (it != _textures.end()) {
        texture = it->second;
    }

    bool ret = false;
    if (! texture) {
        texture = this->addImage(fullpath);
        ret = (texture != nullptr);
    }
    else
    {
        do {
            image = new (std::nothrow) Image();
            CC_BREAK_IF(nullptr == image);

            bool bRet = image->initWithImageFile(fullpath);
            CC_BREAK_IF(!bRet);
            
            ret = texture->initWithImage(image);
        } while (0);
    }
    
    CC_SAFE_RELEASE(image);

    return ret;
}

// TextureCache - Remove

void TextureCache::removeAllTextures()
{
    for( auto it=_textures.begin(); it!=_textures.end(); ++it )
	{
		if (print_load_log)
		{
			CCLOG("TextureCache::removeAllTextures file:%s", AnsiToUtf8(it->first.c_str()).c_str());
		}
        (it->second)->release();
    }
    _textures.clear();
}

void TextureCache::removeUnusedTextures()
{
    for( auto it=_textures.cbegin(); it!=_textures.cend(); /* nothing */) {
        Texture2D *tex = it->second;
        if( tex->getReferenceCount() == 1 )
		{
			if (print_load_log)
			{
				CCLOG("TextureCache::removeUnusedTextures file:%s", AnsiToUtf8(it->first.c_str()).c_str());
			}

            tex->release();
            it = _textures.erase(it);
        }
        else {
            ++it;
        }

    }
}

void TextureCache::removeTexture(Texture2D* texture)
{
    if( ! texture )
    {
        return;
    }

    for( auto it=_textures.cbegin(); it!=_textures.cend(); /* nothing */ ) {
        if( it->second == texture )
		{
			if (print_load_log)
			{
				CCLOG("TextureCache::removeTexture file:%s", AnsiToUtf8(it->first.c_str()).c_str());
			}

            it->second->release();
            it = _textures.erase(it);
            break;
        } else
            ++it;
    }
}

void TextureCache::removeTextureForKey(const std::string &textureKeyName)
{
    std::string key = textureKeyName;
    auto it = _textures.find(key);

    if( it == _textures.end() ) {
        key = FileUtils::getInstance()->fullPathForFilename(textureKeyName);

		// 纹理key add by swordman sue 2017-6-5
		key = getTextureKey(key.c_str());

        it = _textures.find(key);
    }

    if( it != _textures.end() )
	{
		if (print_load_log)
		{
			CCLOG("TextureCache::removeTextureForKey file:%s", AnsiToUtf8(it->first.c_str()).c_str());
		}
		//it->second->setAutoClearCache(false);
        it->second->release();
        _textures.erase(it);
    }
}

Texture2D* TextureCache::getTextureForKey(const std::string &textureKeyName) const
{
    std::string key = textureKeyName;
    auto it = _textures.find(key);

    if( it == _textures.end() ) {
        key = FileUtils::getInstance()->fullPathForFilename(textureKeyName);

		// 纹理key add by swordman sue 2017-6-5
		key = getTextureKey(key.c_str());

        it = _textures.find(key);
    }

    if( it != _textures.end() )
        return it->second;
    return nullptr;
}

void TextureCache::reloadAllTextures()
{
//will do nothing
// #if CC_ENABLE_CACHE_TEXTURE_DATA
//     VolatileTextureMgr::reloadAllTextures();
// #endif
}

std::string TextureCache::getTextureFilePath(cocos2d::Texture2D* texture) const
{
    for(auto& item : _textures)
    {
        if(item.second == texture)
        {
            return item.first;
            break;
        }
    }
    return "";
}

void TextureCache::waitForQuit()
{
    // notify sub thread to quick
    _needQuit = true;
    _sleepCondition.notify_one();
    if (_loadingThread) _loadingThread->join();
}

unsigned int TextureCache::getCachedTextureBytes(const std::string &filter)
{
	unsigned int totalBytes = 0;
	for (auto it = _textures.begin(); it != _textures.end(); ++it) {
		if (filter == "" || it->first.find(filter) != std::string::npos)
		{
			Texture2D* tex = it->second;
			unsigned int bpp = tex->getBitsPerPixelForFormat();
			// Each texture takes up width * height * bytesPerPixel bytes.
			auto bytes = tex->getPixelsWide() * tex->getPixelsHigh() * bpp / 8;
			totalBytes += bytes;
		}
	}

	return totalBytes;
}

void TextureCache::recordCurTextureInfo()
{
	_texturesRecord.clear();
	for (auto it = _textures.begin(); it != _textures.end(); ++it) {
		_texturesRecord.insert(make_pair(it->first, it->first));
	}
}

void TextureCache::printTextureChange()
{
	//新增的
	for (auto it = _textures.begin(); it != _textures.end(); ++it) {
		if (_texturesRecord.find(it->first) == _texturesRecord.end())
		{
			CCLOG("cocos2d: new texture file:%s", it->first.c_str());
		}
	}
	//销毁的
	for (auto it = _texturesRecord.begin(); it != _texturesRecord.end(); ++it) {
		if (_textures.find(it->first) == _textures.end())
		{
			CCLOG("cocos2d: remove texture file:%s", it->first.c_str());
		}
	}
}

std::string TextureCache::getCachedTextureInfo() const
{
    std::string buffer;
    char buftmp[4096];

    unsigned int count = 0;
    unsigned int totalBytes = 0;

    for( auto it = _textures.begin(); it != _textures.end(); ++it ) {

        memset(buftmp,0,sizeof(buftmp));


        Texture2D* tex = it->second;
        unsigned int bpp = tex->getBitsPerPixelForFormat();
        // Each texture takes up width * height * bytesPerPixel bytes.
        auto bytes = tex->getPixelsWide() * tex->getPixelsHigh() * bpp / 8;
        totalBytes += bytes;
        count++;
        snprintf(buftmp,sizeof(buftmp)-1,"\"%s\" rc=%lu id=%lu %lu x %lu @ %ld bpp => %lu KB\n",
               it->first.c_str(),
               (long)tex->getReferenceCount(),
               (long)tex->getName(),
               (long)tex->getPixelsWide(),
               (long)tex->getPixelsHigh(),
               (long)bpp,
               (long)bytes / 1024);
        
        buffer += buftmp;
    }

    snprintf(buftmp, sizeof(buftmp)-1, "TextureCache dumpDebugInfo: %ld textures, for %lu KB (%.2f MB)\n", (long)count, (long)totalBytes / 1024, totalBytes / (1024.0f*1024.0f));
    buffer += buftmp;

    return buffer;
}

void TextureCache::renameTextureWithKey(const std::string& srcName, const std::string& dstName)
{
    std::string key = srcName;
    auto it = _textures.find(key);

    if( it == _textures.end() ) {
        key = FileUtils::getInstance()->fullPathForFilename(srcName);

		// 纹理key add by swordman sue 2017-6-5
		key = getTextureKey(key.c_str());

        it = _textures.find(key);
    }

    if( it != _textures.end() ) {
        std::string fullpath = FileUtils::getInstance()->fullPathForFilename(dstName);

		// 纹理key add by swordman sue 2017-6-5
		key = getTextureKey(fullpath.c_str());

        Texture2D* tex = it->second;

        Image* image = new Image();
        if (image)
        {
            bool ret = image->initWithImageFile(dstName);
            if (ret)
            {
                tex->initWithImage(image);
				tex->setTexturePath(key);
				_textures.emplace(key, tex);
                _textures.erase(it);
            }
            CC_SAFE_DELETE(image);
        }
    }
}

std::string TextureCache::getTextureKey(const char* fullpath) const
{
	if (nullptr != _custom_texture_cache)
	{
		return _custom_texture_cache->getTextureKey(fullpath);
	}
	return fullpath;
}

#if CC_ENABLE_CACHE_TEXTURE_DATA

std::list<VolatileTexture*> VolatileTextureMgr::_textures;
bool VolatileTextureMgr::_isReloading = false;

VolatileTexture::VolatileTexture(Texture2D *t)
: _texture(t)
, _cashedImageType(kInvalid)
, _textureData(nullptr)
, _pixelFormat(Texture2D::PixelFormat::RGBA8888)
, _fileName("")
, _text("")
, _uiImage(nullptr)
, _hasMipmaps(false)
{
    _texParams.minFilter = GL_LINEAR;
    _texParams.magFilter = GL_LINEAR;
    _texParams.wrapS = GL_CLAMP_TO_EDGE;
    _texParams.wrapT = GL_CLAMP_TO_EDGE;
}

VolatileTexture::~VolatileTexture()
{
    CC_SAFE_RELEASE(_uiImage);
}

void VolatileTextureMgr::addImageTexture(Texture2D *tt, const std::string& imageFileName)
{
    if (_isReloading)
    {
        return;
    }

    VolatileTexture *vt = findVolotileTexture(tt);

    vt->_cashedImageType = VolatileTexture::kImageFile;
    vt->_fileName = imageFileName;
    vt->_pixelFormat = tt->getPixelFormat();
}

void VolatileTextureMgr::addImage(Texture2D *tt, Image *image)
{
    if (tt == nullptr || image == nullptr)
        return;
    
    VolatileTexture *vt = findVolotileTexture(tt);
    image->retain();
    vt->_uiImage = image;
    vt->_cashedImageType = VolatileTexture::kImage;
}

VolatileTexture* VolatileTextureMgr::findVolotileTexture(Texture2D *tt)
{
    VolatileTexture *vt = nullptr;
    for (const auto& texture : _textures)
    {
        VolatileTexture *v = texture;
        if (v->_texture == tt)
        {
            vt = v;
            break;
        }
    }
    
    if (! vt)
    {
        vt = new (std::nothrow) VolatileTexture(tt);
        _textures.push_back(vt);
    }
    
    return vt;
}

void VolatileTextureMgr::addDataTexture(Texture2D *tt, void* data, int dataLen, Texture2D::PixelFormat pixelFormat, const Size& contentSize)
{
    if (_isReloading)
    {
        return;
    }

    VolatileTexture *vt = findVolotileTexture(tt);

    vt->_cashedImageType = VolatileTexture::kImageData;
    vt->_textureData = data;
    vt->_dataLen = dataLen;
    vt->_pixelFormat = pixelFormat;
    vt->_textureSize = contentSize;
}

void VolatileTextureMgr::addStringTexture(Texture2D *tt, const char* text, const FontDefinition& fontDefinition)
{
    if (_isReloading)
    {
        return;
    }

    VolatileTexture *vt = findVolotileTexture(tt);

    vt->_cashedImageType = VolatileTexture::kString;
    vt->_text     = text;
    vt->_fontDefinition = fontDefinition;
}

void VolatileTextureMgr::setHasMipmaps(Texture2D *t, bool hasMipmaps)
{
    VolatileTexture *vt = findVolotileTexture(t);
    vt->_hasMipmaps = hasMipmaps;
}

void VolatileTextureMgr::setTexParameters(Texture2D *t, const Texture2D::TexParams &texParams)
{
    VolatileTexture *vt = findVolotileTexture(t);

    if (texParams.minFilter != GL_NONE)
        vt->_texParams.minFilter = texParams.minFilter;
    if (texParams.magFilter != GL_NONE)
        vt->_texParams.magFilter = texParams.magFilter;
    if (texParams.wrapS != GL_NONE)
        vt->_texParams.wrapS = texParams.wrapS;
    if (texParams.wrapT != GL_NONE)
        vt->_texParams.wrapT = texParams.wrapT;
}

void VolatileTextureMgr::removeTexture(Texture2D *t)
{
    for (auto& item : _textures)
    {
        VolatileTexture *vt = item;
        if (vt->_texture == t)
        {
            _textures.remove(vt);
            delete vt;
            break;
        }
    }
}

void VolatileTextureMgr::reloadAllTextures()
{
    _isReloading = true;

    // we need to release all of the glTextures to avoid collisions of texture id's when reloading the textures onto the GPU
    for (auto& item : _textures)
    {
        item->_texture->releaseGLTexture();
    }

    CCLOG("reload all texture");

    for (auto& texture : _textures)
    {
        VolatileTexture *vt = texture;

        switch (vt->_cashedImageType)
        {
        case VolatileTexture::kImageFile:
        {
            Image* image = new (std::nothrow) Image();

            Data data = FileUtils::getInstance()->getDataFromFile(vt->_fileName);

            if (image && image->initWithImageData(data.getBytes(), data.getSize()))
            {
                Texture2D::PixelFormat oldPixelFormat = Texture2D::getDefaultAlphaPixelFormat();
                Texture2D::setDefaultAlphaPixelFormat(vt->_pixelFormat);
                vt->_texture->initWithImage(image);
                Texture2D::setDefaultAlphaPixelFormat(oldPixelFormat);
            }

            CC_SAFE_RELEASE(image);
        }
        break;
        case VolatileTexture::kImageData:
        {
            vt->_texture->initWithData(vt->_textureData,
                vt->_dataLen,
                vt->_pixelFormat,
                vt->_textureSize.width,
                vt->_textureSize.height,
                vt->_textureSize);
        }
        break;
        case VolatileTexture::kString:
        {
            vt->_texture->initWithString(vt->_text.c_str(), vt->_fontDefinition);
        }
        break;
        case VolatileTexture::kImage:
        {
            vt->_texture->initWithImage(vt->_uiImage);
        }
        break;
        default:
            break;
        }
        if (vt->_hasMipmaps) {
            vt->_texture->generateMipmap();
        }
        vt->_texture->setTexParameters(vt->_texParams);
    }

    _isReloading = false;
}

#endif // CC_ENABLE_CACHE_TEXTURE_DATA

NS_CC_END

