#include "richtext.h"

#include "base/CCDirector.h"
#include "2d/CCLabel.h"

#include "view/font/fontmanager.h"
#include "view/font/stringconv.h"
#include "view/render/textureutil.h"
#include "view/render/batchrenderer.h"
#include "view/gamelayer.h"
#include "view/viewhelper.h"
#include "luaengine/luaengine.h"
#include "filesystem/filesystemex.h"
#include "filesystem/setting.h"
#include "zqcommon/gamedef.h"

USING_NS_CC;

unsigned int RichText::defaultOutlineCol = 0;
unsigned int RichText::defaultShadowCol = 0;
std::string RichText::newlineCharStr = "";
#ifdef WP8
int RichText::horizon_font_space = 2;
#else
int RichText::horizon_font_space = 0;
#endif // _WP8

RichText::RichText() :
	m_strFontName(Label::DEF_FONT_NAME),
	m_nFontSize(24),
	m_pRoot(NULL),
	m_nTextWidth(0),
	m_nTextHeight(0),
	m_bHeightFixed(false),
	m_nItemHeight(0),
	m_ptItemPos(0.0f, 0.0f),
	m_ptCapture(0.0f, 0.0f),
	m_pTexture(NULL),
	m_fOffsetX(0.0f), m_fOffsetY(0.0f), m_fWidthEx(0.0f), m_fHeightEx(0.0f),
	m_rcTexInner(CCRect(0.3f, 0.3f, 0.3f, 0.3f)),
	m_lua_link_fun_id(0),
	m_bCapture(false),
	m_nFontSpacing(0),
	m_swallow(true),
	m_horizon_font_space(horizon_font_space),
	m_reading_order(RichItemParser::ReadingOrder::RO_LTR)
{
	BatchRendererManager::sharedRendererManager()->registerRenderer(&m_renderer);
	BatchRendererManager::sharedRendererManager()->registerRenderer(&m_faceRenderer);
}

RichText::~RichText()
{
	if(m_pRoot)
	{
		clearItem(m_pRoot);
		m_pRoot = NULL;
	}

	CC_SAFE_RELEASE(m_pTexture);

	LuaEngine::GetInstance()->DelLuaFuncID(m_lua_link_fun_id);

	BatchRendererManager::sharedRendererManager()->unregisterRenderer(&m_renderer);
	BatchRendererManager::sharedRendererManager()->unregisterRenderer(&m_faceRenderer);
}

bool RichText::init()
{
	// shader program
	auto gl_program = GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_RICHTEXT_POSITION_TEXTURE_COLOR);
	auto gl_program_state = GLProgramState::getOrCreateWithGLProgram(gl_program);
	setGLProgramState(gl_program_state);
	m_uEntityTypeLoc = glGetUniformLocation(getGLProgram()->getProgram(), "u_entityType");

	m_renderer.setShaderKey(GLProgram::SHADER_RICHTEXT_POSITION_TEXTURE_COLOR);
	m_faceRenderer.setShaderKey(GLProgram::SHADER_RICHTEXT_POSITION_TEXTURE_COLOR);
	m_renderer.setUniformInfo(m_uEntityTypeLoc, 1);
	m_faceRenderer.setUniformInfo(m_uEntityTypeLoc, 2);

	Node::setAnchorPoint(Vec2(0.5f, 0.5f));

	return true;
}

void RichText::clearItem(RichItem* pItem)
{
	RichItem* pIt = pItem;
	while(pIt)
	{
		pIt->m_pPre = NULL;
		RichItem* pNext = pIt->m_pNext;
		delete pIt;
		pIt = pNext;
	}
}

RichText * RichText::Node()
{
	RichText *pRet = new RichText();
	if(pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return NULL;
}

RichText * RichText::CreateRichText(int width, int height, const std::string& fontName, int fontSize)
{
	RichText *pRet = new RichText();
	if(pRet && pRet->init())
	{
		pRet->SetFontName(fontName);
		pRet->SetFontSize(fontSize);
		pRet->SetTextHeight(height);
		pRet->SetTextWidth(width);
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return NULL;
}

void RichText::SetFontName(const std::string& font_name)
{
	m_strFontName = font_name;
}

void RichText::SetFontSize(int font_size)
{
	m_nFontSize = font_size;
}

void RichText::SetTextHeight(int height)
{
	height = height >= 0 ? height : 0;

	if (m_nTextHeight == height)
		return;
	m_nTextHeight = height;
	SetHeightFixed(height > 0 ? true : false);

	if(m_bHeightFixed)
		setContentSize(CCSize((float)m_nTextWidth, (float)m_nTextHeight));
}

void RichText::SetTextWidth(int width)
{
	width = width >= 0 ? width : 0;

	if (m_nTextWidth == width)
		return;
	m_nTextWidth = width;

	if(m_bHeightFixed)
		setContentSize(CCSize((float)m_nTextWidth, (float)m_nTextHeight));
}

void RichText::SetHeightFixed(bool val)
{
	m_bHeightFixed = val;
}

void RichText::draw(cocos2d::Renderer* renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
	_customCommand.init(_globalZOrder);
	_customCommand.func = CC_CALLBACK_0(RichText::onDraw, this, transform, flags);
	renderer->addCommand(&_customCommand);
}

void RichText::onDraw(const cocos2d::Mat4 &transform, uint32_t transformFlags)
{
	CC_PROFILER_START_CATEGORY(kCCProfilerCategorySprite, "RichText - draw");

	auto glview = Director::getInstance()->getOpenGLView();
	bool bScissor = glview->isScissorEnabled();
	Rect stScissorRect = glview->getScissorRect();

	if (m_bHeightFixed)
	{
		CCPoint pt1 = ccp(0, 0);
		CCPoint pt2 = ccp(getContentWidth(), getContentHeight());
		pt1 = convertToWorldSpace(pt1);
		pt2 = convertToWorldSpace(pt2);

#if defined(WP8)
		CCPoint translate_pt1, translate_pt2;
		cocos2d::CCSize win_size = cocos2d::CCDirector::sharedDirector()->getWinSize();
		translate_pt1.x = pt1.y;
		translate_pt1.y = win_size.width - pt1.x - (pt2.x - pt1.x);
		translate_pt2.x = pt2.y - pt1.y;
		translate_pt1.y = pt2.x - pt1.x;
		pt1 = translate_pt1;
		pt2 = translate_pt2;
#endif

		GLint sx, sy, tx, ty;
		if (bScissor)
		{
			sx = stScissorRect.origin.x > GLint(pt1.x) ? stScissorRect.origin.x : GLint(pt1.x);
			sy = stScissorRect.origin.y > GLint(pt1.y) ? stScissorRect.origin.y : GLint(pt1.y);
			tx = (stScissorRect.origin.x + stScissorRect.size.width) < GLint(pt2.x) ? (stScissorRect.origin.x + stScissorRect.size.width) : GLint(pt2.x);
			ty = (stScissorRect.origin.y + stScissorRect.size.height) < GLint(pt2.y) ? (stScissorRect.origin.y + stScissorRect.size.height) : GLint(pt2.y);
		}
		else
		{
			sx = GLint(pt1.x);
			sy = GLint(pt1.y);
			tx = GLint(pt2.x);
			ty = GLint(pt2.y);
		}

		if (tx - sx > 0 && ty - sy > 0)
		{
			glEnable(GL_SCISSOR_TEST);
			glview->setScissorInPoints(sx, sy, tx - sx, ty - sy);
		}
	}

	CC_NODE_DRAW_SETUP();

	GL::blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (m_pTexture)
	{
		Color4B col(255, 255, 255, 255);
		drawScale9Texture(m_pTexture, Rect(m_imageData.x, m_imageData.y, m_imageData.width, m_imageData.height),
			Rect(m_fOffsetX, m_fOffsetY, Node::getContentSize().width + m_fWidthEx, m_nItemHeight + m_fHeightEx),
			m_rcTexInner, col, transform, false);
	}

	if (m_pRoot)
	{
		Vec2 pt;
		pt.x = m_ptItemPos.x;
		pt.y = m_ptItemPos.y + Node::getContentSize().height;

		//文本
		BATCH_RENDERER_BEGIN(m_renderer);
		RichItemContext context;
		context.m_nFontSize = m_nFontSize;
		context.m_strFontName = m_strFontName;
		context.m_fScale = 1.0f / CC_CONTENT_SCALE_FACTOR();
		context.m_nFontSpacing = m_nFontSpacing;
		context.m_nShadowColor = defaultShadowCol;
		context.m_nOutlineColor = defaultOutlineCol;
		for (RichItem* pItem = m_pRoot; pItem != NULL; pItem = pItem->m_pNext)
		{
			if (pItem->isEntityItem())
			{
				TextRichItem* pItemEntity = dynamic_cast<TextRichItem*>(pItem);
				if (pItemEntity)
				{
					pItemEntity->draw(&context, (int)pt.x, (int)pt.y, NULL, getDisplayedOpacity());
				}
			}
			else if (pItem->isPropertyItem())
			{
				RichItemProperty* pItemProperty = dynamic_cast<RichItemProperty*>(pItem);
				if (pItemProperty)
				{
					pItemProperty->updateContext(&context);
				}
			}
		}
		BATCH_RENDERER_END(m_renderer);

		//表情
		BATCH_RENDERER_BEGIN(m_faceRenderer);
		for (RichItem* pItem = m_pRoot; pItem != NULL; pItem = pItem->m_pNext)
		{
			if (pItem->isEntityItem())
			{
				FaceImgRichItem* pItemEntity = dynamic_cast<FaceImgRichItem*>(pItem);
				if (pItemEntity)
				{
					pItemEntity->draw(nullptr, (int)pt.x, (int)pt.y, NULL, getDisplayedOpacity());
				}
			}
		}
		BATCH_RENDERER_END(m_faceRenderer);
	}

	if (m_bHeightFixed)
	{
		if (bScissor)
		{
			glEnable(GL_SCISSOR_TEST);
			glview->setScissorInPoints(stScissorRect.origin.x, stScissorRect.origin.y, stScissorRect.size.width, stScissorRect.size.height);
		}
		else
		{
			glDisable(GL_SCISSOR_TEST);
		}
	}

	CHECK_GL_ERROR_DEBUG();

	CC_INCREMENT_GL_DRAWS(1);

	CC_PROFILER_STOP_CATEGORY(kCCProfilerCategorySprite, "RichText - draw");
}

void RichText::setText(const char* str)
{
	if(m_strText == str)
		return;

	m_strText = str;

	m_renderer.setDirty(true);
	m_faceRenderer.setDirty(true);

	if(m_pRoot)
	{
		clearItem(m_pRoot);
	}

	RichItemContext context;
	context.m_nFontSize = m_nFontSize;
	context.m_strFontName = m_strFontName;
	context.m_fScale = 1.0f;
	context.m_nFontSpacing = m_nFontSpacing;
	m_pRoot = RichItemParser::sharedRichItemParser()->parse(m_strText.c_str(), m_strText.length(), &context);
	Size szItem = RichItemParser::sharedRichItemParser()->layoutItem(m_pRoot, m_nTextWidth, m_vItemRow, m_horizon_font_space, m_reading_order);
	m_nItemHeight = (int)szItem.height;
	if(m_nItemHeight <= 0)
		return;

	float visualWidth = m_nTextWidth > 0 ? m_nTextWidth : szItem.width;
	float visualHeight = m_bHeightFixed ? m_nTextHeight : m_nItemHeight;
	setContentSize(Size(visualWidth, visualHeight));
}

const char* RichText::getText() const
{
	return m_strText.c_str();
}

void RichText::setItemPosition(const cocos2d::Vec2& pos)
{
	m_ptItemPos = pos;
}

Vec2 RichText::convertToWindowSpace(const Vec2& nodePoint)
{
	Vec2 worldPoint = this->convertToWorldSpace(nodePoint);
	return Director::getInstance()->convertToUI(worldPoint);
}

void RichText::onEnter()
{
	Node::onEnter();
	SetTouchEnabled(true);
}

void RichText::onExit()
{
	SetTouchEnabled(false);
	Node::onExit();
}

bool RichText::onTouchBegan(Touch* touch, Event* event)
{
	if (!Node::isVisible())
		return false;

	for (CCNode *c = this->_parent; c != NULL; c = c->getParent())
	{
		if (c->isVisible() == false)
		{
			return false;
		}
	}

	Vec2 pos = touch->getLocation();
	Vec2 local = convertToNodeSpace(pos);
	Rect r = Rect(getPositionX(), getPositionY(), Node::getContentSize().width, Node::getContentSize().height);
	r.origin = Vec2::ZERO;

	LinkRichItem* pLinkItem = getLinkItem(local);
	if(pLinkItem)
	{
		return true;
	}

	bool bContainPt = r.containsPoint(local);
	if (bContainPt
		&& m_bHeightFixed
		&& m_nItemHeight > Node::getContentSize().height)
	{
		m_ptCapture = local;
		m_bCapture = true;
		return true;
	}

	return false;
}

LinkRichItem* RichText::getLinkItem(const CCPoint& local)
{
	Size size = Node::getContentSize();
	if (local.x < 0 || local.x > size.width
		|| local.y < 0 || local.y > size.height)
		return NULL;

	Vec2 pt;
	pt.x = m_ptItemPos.x;
	pt.y = m_ptItemPos.y + size.height;

	MidFont* pFont = FontManager::sharedFontManager()->getValidFont(m_strFontName, m_nFontSize);
	for(RichItem* pItem = m_pRoot; pItem != NULL; pItem = pItem->m_pNext)
	{
		if(pItem->isEntityItem())
		{
			RichItemEntity* pItemEntity = dynamic_cast<RichItemEntity*>(pItem);
			if(pItemEntity)
			{
				LinkRichItem* pLinkItem = pItemEntity->m_pLinkItem;
				if(pLinkItem)
				{
					Rect rcItem = Rect(pt.x + pItemEntity->m_pos.x, pt.y + pItemEntity->m_pos.y - pFont->getFontHeight() - RICH_ITEM_FONT_LINE_HEIGHT, pItemEntity->m_size.width, pItemEntity->m_size.height);
					if(rcItem.containsPoint(local))
					{
						return pLinkItem;
					}
				}
			}
		}
	}

	return NULL;
}

void RichText::onTouchEnded(Touch* touch, Event* event)
{
	m_bCapture = false;
	
	//点击链接回调
	Vec2 pos = touch->getLocation();
	Vec2 local = convertToNodeSpace(pos);

	LinkRichItem* pLinkItem = getLinkItem(local);
	if(pLinkItem)
	{
		LuaEngine::GetInstance()->CallFunction(m_lua_link_fun_id, "ddiis", local.x, local.y, pLinkItem->m_nLinkType, pLinkItem->m_nUserData, pLinkItem->m_strUrl.c_str());
	}
}

void RichText::onTouchMoved(Touch* touch, Event* event)
{
	if(!m_bCapture)
		return;

	Vec2 pos = touch->getLocation();
	Vec2 local = convertToNodeSpace(pos);
	float delta = m_nItemHeight - Node::getContentSize().height;
	m_ptItemPos.y += (local.y - m_ptCapture.y);
	
	if(m_ptItemPos.y >= delta)
		m_ptItemPos.y = delta;
	else if(m_ptItemPos.y < 0.0f)
		m_ptItemPos.y = 0.0f;

	m_ptCapture.y = local.y;

	m_renderer.setDirty(true);
	m_faceRenderer.setDirty(true);
}

void RichText::SetText(const std::string& str)
{
	setText(str.c_str());
}

void RichText::ScrollToStart()
{
	if(!m_bHeightFixed)
		return;

	m_ptItemPos.y = 0.0f;

	m_renderer.setDirty(true);
	m_faceRenderer.setDirty(true);
}

void RichText::ScrollToBottom()
{
	if(!m_bHeightFixed)
		return;

	float delta = m_nItemHeight - Node::getContentSize().height;
	if(delta > 0.0f)
		m_ptItemPos.y = delta;
	else
		m_ptItemPos.y = 0.0f;

	m_renderer.setDirty(true);
	m_faceRenderer.setDirty(true);
}

void RichText::SetImageData(const ImageData& image_data, float x, float y, float w, float h,
	float fOffsetX, float fOffsetY, float fWidthEx, float fHeightEx)
{
	m_imageData = image_data;

	std::string full_name = FileSystemEx::GetInstance()->GetResourcePath(m_imageData.image_path);

	if(!ViewHelper::LoadImageResource(image_data.image_path))
		return;

	Texture2D *pTexture = Director::getInstance()->getTextureCache()->getTextureForKey(full_name.c_str());

	if(pTexture == NULL)
		return;

	m_fOffsetX = fOffsetX;
	m_fOffsetY = fOffsetY;
	m_fWidthEx = fWidthEx;
	m_fHeightEx = fHeightEx;

	m_rcTexInner.setRect(x, y, w, h);

	CC_SAFE_RETAIN(pTexture);
	CC_SAFE_RELEASE(m_pTexture);
	m_pTexture = pTexture;
}

float RichText::GetContentWidth() const
{
	return Node::getContentSize().width;
}

float RichText::GetContentHeight() const
{
	return Node::getContentSize().height;
}

void RichText::SetDefaultOutlineColor(unsigned int outlineColor)
{
	defaultOutlineCol = outlineColor;
}

void RichText::SetDefaultShadowColor(unsigned int shadowColor)
{
	defaultShadowCol = shadowColor;
}

//設置是否吞噬觸屏消息
void RichText::SetSwallow(bool swallow)
{
	if (swallow == m_swallow)
		return;

	m_swallow = swallow;
	SetTouchEnabled(false);
	SetTouchEnabled(true);
}

void RichText::SetTouchEnabled(bool enabled)
{
	if (m_touchEnabled == enabled)
	{
		return;
	}

	m_touchEnabled = enabled;

	if (enabled)
	{
		auto listener = EventListenerTouchOneByOne::create();

		listener->setSwallowTouches(m_swallow);

		listener->onTouchBegan = CC_CALLBACK_2(RichText::onTouchBegan, this);
		listener->onTouchMoved = CC_CALLBACK_2(RichText::onTouchMoved, this);
		listener->onTouchEnded = CC_CALLBACK_2(RichText::onTouchEnded, this);

		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
		m_touchListener = listener;
	}
	else
	{
		_eventDispatcher->removeEventListener(m_touchListener);
		m_touchListener = nullptr;
	}
}

void RichText::setOpacity(GLubyte opacity)
{
	cocos2d::Node::setOpacity(opacity);

	m_renderer.setDirty(true);
	m_faceRenderer.setDirty(true);
}