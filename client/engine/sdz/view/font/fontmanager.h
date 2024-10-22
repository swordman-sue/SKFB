#ifndef _CC_FONT_MANAGER_H_
#define _CC_FONT_MANAGER_H_

#include "font.h"
#include "zqcommon/typedef.h"
#include <set>

class FontManager
{
public:
	FontManager();

	~FontManager();

	static FontManager* sharedFontManager();

	MidFont* getFont(const std::string& strFontName, int nFontSize);

	MidFont* getValidFont(const std::string& strFontName, int nFontSize);

	void clearAllFonts();

	GameType::StrVect_CIter BeginIterator() { return m_vValidFont.begin(); }

	GameType::StrVect_CIter EndIterator() { return m_vValidFont.end(); }

	//设置有效字体大小范围
	void setFontSizeRange(int nMinSize, int nMaxSize);

	//注册有效字体
	void registerFont(const std::string& strFont);

	//匹配有效字体
	void getValidProp(std::string& strFontName, int& nFontSize);

	void registerFilterChar(unsigned int code);
	bool isFilterChar(unsigned int code);

	cocos2d::Texture2D* getPixelTexture();

private:
	void initialize();

	void finalize();

	MidFont* createFont(const std::string& strFontName, int nFontSize);

	void initPixelTexture();

private:
	typedef std::map<std::string, MidFont*> FontMap;
	FontMap m_fontMap;
	typedef std::set<unsigned int> FilterCharSet;
	FilterCharSet m_filterCharSet;
	
	GameType::StrVect m_vValidFont;
	int m_nMinSize;
	int m_nMaxSize;
	cocos2d::Texture2D* m_pPixelTexture;
};

#endif