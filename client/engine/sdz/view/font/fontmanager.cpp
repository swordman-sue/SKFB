#include "fontmanager.h"
#include "view/render/textureutil.h"
#include "2d/CCLabel.h"

static FontManager s_fontManager;

FontManager::FontManager():
	m_nMaxSize(50),
	m_nMinSize(10),
	m_pPixelTexture(NULL)
{
	initialize();
}

FontManager::~FontManager()
{
	finalize();
}

FontManager* FontManager::sharedFontManager()
{
	return &s_fontManager;
}

void FontManager::initialize()
{
	//注册字体
	registerFont("Arial");
	registerFont("HelveticaNeue-Bold");
	registerFont(cocos2d::Label::DEF_FONT_NAME);
}

void FontManager::finalize()
{
	for(FontMap::iterator it = m_fontMap.begin(); it != m_fontMap.end(); ++it)
	{
		MidFont* pFont = it->second;
		delete pFont;
	}

	m_fontMap.clear();
	CC_SAFE_RELEASE_NULL(m_pPixelTexture);
}

MidFont* FontManager::getFont(const std::string& strFontName, int nFontSize)
{
	char key[100];
	sprintf(key, "%s_%d", strFontName.c_str(), nFontSize);

	FontMap::iterator it = m_fontMap.find(key);
	if(it != m_fontMap.end())
	{
		return it->second;
	}
	else
	{
		return createFont(strFontName, nFontSize);
	}
}

MidFont* FontManager::getValidFont(const std::string& strFontName, int nFontSize)
{
	std::string strValid = strFontName;
	int nValid = nFontSize;

	//有效字体匹配
	getValidProp(strValid, nValid);

	return getFont(strValid, nValid);
}

MidFont* FontManager::createFont(const std::string& strFontName, int nFontSize)
{
	char key[100];
	sprintf(key, "%s_%d", strFontName.c_str(), nFontSize);

	MidFont* pFont = new MidFont(strFontName, nFontSize);

	m_fontMap[key] = pFont;

	return pFont;
}

void FontManager::getValidProp(std::string& strFontName, int& nFontSize)
{
	std::string strTmp = m_vValidFont[0];
	for(int i = 0; i < (int)m_vValidFont.size(); ++i)
	{
		if(m_vValidFont[i] == strFontName)
		{
			strTmp = strFontName;
			break;
		}
	}
	strFontName = strTmp;

	if(nFontSize > m_nMaxSize)
		nFontSize = m_nMaxSize;
	else if(nFontSize < m_nMinSize)
		nFontSize = m_nMinSize;
}

void FontManager::clearAllFonts()
{
	for(FontMap::iterator it = m_fontMap.begin(); it != m_fontMap.end(); ++it)
	{
		MidFont* pFont = it->second;
		pFont->clear();
	}
	CC_SAFE_RELEASE_NULL(m_pPixelTexture);
}

void FontManager::setFontSizeRange(int nMinSize, int nMaxSize)
{
	m_nMinSize = nMinSize;
	m_nMaxSize = nMaxSize;
}

void FontManager::registerFont(const std::string& strFont)
{
	m_vValidFont.push_back(strFont);
}

void FontManager::registerFilterChar(unsigned int code)
{
	m_filterCharSet.insert(code);
}

bool FontManager::isFilterChar(unsigned int code)
{
	return m_filterCharSet.find(code) != m_filterCharSet.end();
}

cocos2d::Texture2D* FontManager::getPixelTexture()
{
	if(m_pPixelTexture == NULL)
	{
		initPixelTexture();
	}

	return m_pPixelTexture;
}

void FontManager::initPixelTexture()
{
	CC_SAFE_RELEASE(m_pPixelTexture);

	m_pPixelTexture = createARGB8Texture(1, 1, 0xffffffff);
}