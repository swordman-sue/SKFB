/****************************************************************************
Copyright (c) 2013-2014 Chukong Technologies Inc.

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

#include "ui/UIPageView.h"
#include "ui/UIPageViewIndicator.h"

NS_CC_BEGIN

namespace ui {
    
IMPLEMENT_CLASS_GUI_INFO(PageView)

PageView::PageView():
_indicator(nullptr),
_indicatorPositionAsAnchorPoint(Vec2(0.5f, 0.1f)),
_currentPageIndex(-1),
_childFocusCancelOffset(5.0f),
_pageViewEventListener(nullptr),
_pageViewEventSelector(nullptr),
_eventCallback(nullptr)
{
}

PageView::~PageView()
{
    _pageViewEventListener = nullptr;
    _pageViewEventSelector = nullptr;
}

PageView* PageView::create()
{
    PageView* widget = new (std::nothrow) PageView();
    if (widget && widget->init())
    {
        widget->autorelease();
        return widget;
    }
    CC_SAFE_DELETE(widget);
    return nullptr;
}
    
bool PageView::init()
{
    if (ListView::init())
    {
		// 过滤不可见节点 edit by swordman sue 2016-8-24
		_innerContainer->setFilterInvisible(true);
		// end by swordman sue

		setDirection(Direction::HORIZONTAL);
        setMagneticType(MagneticType::CENTER);
        setScrollBarEnabled(false);
        return true;
    }
    return false;
}

void PageView::doLayout()
{
    if(!_innerContainerDoLayoutDirty)
    {
        return;
    }

    ListView::doLayout();
    if(_indicator != nullptr)
    {
        ssize_t index = getIndex(getCenterItemInCurrentView());
        _indicator->indicate(index);
    }
    _innerContainerDoLayoutDirty = false;
}

void PageView::setDirection(PageView::Direction direction)
{
    ListView::setDirection(direction);
    if(direction == Direction::HORIZONTAL)
    {
        _indicatorPositionAsAnchorPoint = Vec2(0.5f, 0.1f);
    }
    else if(direction == Direction::VERTICAL)
    {
        _indicatorPositionAsAnchorPoint = Vec2(0.1f, 0.5f);
    }

    if(_indicator != nullptr)
    {
        _indicator->setDirection(direction);
        refreshIndicatorPosition();
    }
}

void PageView::addWidgetToPage(Widget *widget, ssize_t pageIdx, bool forceCreate)
{
    insertCustomItem(widget, pageIdx);
}

void PageView::addPage(Widget* page)
{
    pushBackCustomItem(page);
}

void PageView::insertPage(Widget* page, int idx)
{
    insertCustomItem(page, idx);
}

void PageView::removePage(Widget* page)
{
    removeItem(getIndex(page));
}

void PageView::removePageAtIndex(ssize_t index)
{
    removeItem(index);
}
    
void PageView::removeAllPages()
{
    removeAllItems();
}

void PageView::setCurPageIndex( ssize_t index )
{
    setCurrentPageIndex(index);
}

void PageView::setCurrentPageIndex(ssize_t index)
{
    jumpToItem(index, Vec2::ANCHOR_MIDDLE, Vec2::ANCHOR_MIDDLE);
}

void PageView::scrollToPage(ssize_t idx)
{
    scrollToItem(idx);
}

void PageView::scrollToItem(ssize_t itemIndex)
{
    ListView::scrollToItem(itemIndex, Vec2::ANCHOR_MIDDLE, Vec2::ANCHOR_MIDDLE);
}

void PageView::setCustomScrollThreshold(float threshold)
{
    CCLOG("PageView::setCustomScrollThreshold() has no effect!");
}

float PageView::getCustomScrollThreshold()const
{
    return 0;
}
    
void PageView::setUsingCustomScrollThreshold(bool flag)
{
    CCLOG("PageView::setUsingCustomScrollThreshold() has no effect!");
}
    
bool PageView::isUsingCustomScrollThreshold()const
{
    return false;
}

void PageView::moveInnerContainer(const Vec2& deltaMove, bool canStartBounceBack)
{
    ListView::moveInnerContainer(deltaMove, canStartBounceBack);
    _currentPageIndex = getIndex(getCenterItemInCurrentView());
    if(_indicator != nullptr)
    {
        _indicator->indicate(_currentPageIndex);
    }
}

void PageView::onItemListChanged()
{
    ListView::onItemListChanged();
    if(_indicator != nullptr)
    {
        _indicator->reset(_items.size());
    }
}

void PageView::onSizeChanged()
{
    ListView::onSizeChanged();
    refreshIndicatorPosition();
}

void PageView::refreshIndicatorPosition()
{
    if(_indicator != nullptr)
    {
        const Size& contentSize = getContentSize();
        float posX = contentSize.width * _indicatorPositionAsAnchorPoint.x;
        float posY = contentSize.height * _indicatorPositionAsAnchorPoint.y;
        _indicator->setPosition(Vec2(posX, posY));
    }
}

void PageView::handleReleaseLogic(Touch *touch)
{
    // Use `ScrollView` method in order to avoid `startMagneticScroll()` by `ListView`.
    ScrollView::handleReleaseLogic(touch);

    if(_items.empty())
    {
        return;
    }

    Vec2 touchMoveVelocity = flattenVectorByDirection(calculateTouchMoveVelocity());

    static const float INERTIA_THRESHOLD = 500;
    if(touchMoveVelocity.length() < INERTIA_THRESHOLD)
    {
        startMagneticScroll();
    }
    else
    {
        // Handle paging by inertia force.
        Widget* currentPage = getItem(_currentPageIndex);
        Vec2 destination = calculateItemDestination(Vec2::ANCHOR_MIDDLE, currentPage, Vec2::ANCHOR_MIDDLE);
        Vec2 deltaToCurrentpage = destination - getInnerContainerPosition();
        deltaToCurrentpage = flattenVectorByDirection(deltaToCurrentpage);

        // If the direction of displacement to current page and the direction of touch are same, just start magnetic scroll to the current page.
        // Otherwise, move to the next page of touch direction.
        if(touchMoveVelocity.x * deltaToCurrentpage.x > 0 || touchMoveVelocity.y * deltaToCurrentpage.y > 0)
        {
            startMagneticScroll();
        }
        else
        {
            if(touchMoveVelocity.x < 0 || touchMoveVelocity.y > 0)
            {
                ++_currentPageIndex;
            }
            else
            {
                --_currentPageIndex;
            }
            _currentPageIndex = MIN(_currentPageIndex, _items.size());
            _currentPageIndex = MAX(_currentPageIndex, 0);
            scrollToItem(_currentPageIndex);
        }
    }
}

void PageView::pageTurningEvent()
{
    this->retain();
    if (_pageViewEventListener && _pageViewEventSelector)
    {
        (_pageViewEventListener->*_pageViewEventSelector)(this, PAGEVIEW_EVENT_TURNING);
    }
    if (_eventCallback)
    {
        _eventCallback(this, EventType::TURNING);
    }
    if (_ccEventCallback)
    {
        _ccEventCallback(this, static_cast<int>(EventType::TURNING));
    }
    this->release();
}

void PageView::pageScrollingEvent()
{
	this->retain();
	if (_eventCallback)
	{
		_eventCallback(this, EventType::SCROLLING);
	}
	this->release();
}

void PageView::pageAutoScrollingEvent()
{
	this->retain();
	if (_eventCallback)
	{
		_eventCallback(this, EventType::AUTO_SCROLLING);
	}
	this->release();
}

void PageView::addEventListenerPageView(Ref *target, SEL_PageViewEvent selector)
{
    _pageViewEventListener = target;
    _pageViewEventSelector = selector;
}
    
void PageView::addEventListener(const ccPageViewCallback& callback)
{
    _eventCallback = callback;

	// 派发翻页事件 add by swordman sue 2016-8-18
	ccScrollViewCallback scrollViewCallback = [=](Ref* ref, ScrollView::EventType type) -> void{
		if (type == ScrollView::EventType::AUTOSCROLL_ENDED) 
		{
			pageTurningEvent();
		}
		//派发滚动事件 add by ldx 2018-9-7
		if (type == ScrollView::EventType::SCROLLING)
		{
			pageScrollingEvent();
		}
		//派发自动滚动事件 add by ldx 2018-9-28
		if (type == ScrollView::EventType::AUTO_SCROLL)
		{
			pageAutoScrollingEvent();
		}
	};
	ScrollView::addEventListener(scrollViewCallback);
}

ssize_t PageView::getCurPageIndex() const
{
    Widget* widget = ListView::getCenterItemInCurrentView();
    return getIndex(widget);
}

Vector<Layout*>& PageView::getPages()
{
    // Temporary code to keep backward compatibility.
    static Vector<Layout*> pages;
    pages.clear();
    for(Widget* widget : getItems())
    {
        pages.pushBack(dynamic_cast<Layout*>(widget));
    }
    return pages;
}

Layout* PageView::getPage(ssize_t index)
{
    if (index < 0 || index >= this->getPages().size())
    {
        return nullptr;
    }

    // Temporary code to keep backward compatibility.
    static Vector<Layout*> pages;
    pages.clear();
    for(Widget* widget : getItems())
    {
        pages.pushBack(dynamic_cast<Layout*>(widget));
    }
    return pages.at(index);
}

std::string PageView::getDescription() const
{
    return "PageView";
}

Widget* PageView::createCloneInstance()
{
    return PageView::create();
}

void PageView::copySpecialProperties(Widget *widget)
{
    PageView* pageView = dynamic_cast<PageView*>(widget);
    if (pageView)
    {
        ListView::copySpecialProperties(widget);
        _eventCallback = pageView->_eventCallback;
        _ccEventCallback = pageView->_ccEventCallback;
        _pageViewEventListener = pageView->_pageViewEventListener;
        _pageViewEventSelector = pageView->_pageViewEventSelector;
    }
}

void PageView::setIndicatorEnabled(bool enabled)
{
    if(enabled == (_indicator != nullptr))
    {
        return;
    }

    if(!enabled)
    {
        removeProtectedChild(_indicator);
        _indicator = nullptr;
    }
    else
    {
        _indicator = PageViewIndicator::create();
        _indicator->setDirection(getDirection());
        addProtectedChild(_indicator, 10000);
        setIndicatorSelectedIndexColor(Color3B(100, 100, 255));
        refreshIndicatorPosition();
    }
}

void PageView::setIndicatorPositionAsAnchorPoint(const Vec2& positionAsAnchorPoint)
{
    _indicatorPositionAsAnchorPoint = positionAsAnchorPoint;
    refreshIndicatorPosition();
}

const Vec2& PageView::getIndicatorPositionAsAnchorPoint() const
{
    return _indicatorPositionAsAnchorPoint;
}

void PageView::setIndicatorPosition(const Vec2& position)
{
    if(_indicator != nullptr)
    {
        const Size& contentSize = getContentSize();
        _indicatorPositionAsAnchorPoint.x = position.x / contentSize.width;
        _indicatorPositionAsAnchorPoint.y = position.y / contentSize.height;
        _indicator->setPosition(position);
    }
}

const Vec2& PageView::getIndicatorPosition() const
{
    CCASSERT(_indicator != nullptr, "");
    return _indicator->getPosition();
}

void PageView::setIndicatorSpaceBetweenIndexNodes(float spaceBetweenIndexNodes)
{
    if(_indicator != nullptr)
    {
        _indicator->setSpaceBetweenIndexNodes(spaceBetweenIndexNodes);
    }
}
float PageView::getIndicatorSpaceBetweenIndexNodes() const
{
    CCASSERT(_indicator != nullptr, "");
    return _indicator->getSpaceBetweenIndexNodes();
}

void PageView::setIndicatorSelectedIndexColor(const Color3B& color)
{
    if(_indicator != nullptr)
    {
        _indicator->setSelectedIndexColor(color);
    }
}

const Color3B& PageView::getIndicatorSelectedIndexColor() const
{
    CCASSERT(_indicator != nullptr, "");
    return _indicator->getSelectedIndexColor();
}

void PageView::remedyLayoutParameter(Widget *item)
{
    item->setContentSize(this->getContentSize());
    ListView::remedyLayoutParameter(item);
}

}

NS_CC_END
