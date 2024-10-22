
#include "nodeunity.h"
#include "scenemanager.h"
#include "view/widget/spriteex.h"

using namespace cocos2d;

COCOPoint NodeUnity::GetWorldPosition(Node *node)
{
	CCAssert(node != NULL, "a null node");
	Point pt;
	Node *parent = node->getParent();

	if(parent)
	{
		pt = node->getPosition();
		pt = parent->convertToWorldSpace(pt);
	}
	else
	{
		pt = node->convertToWorldSpace(ccp(0, 0));
	}
	
	COCOPoint cpt(pt.x, pt.y);
	return cpt;
}

COCOPoint NodeUnity::ConvertToNodePosition(Node *node, COCOPoint point)
{
	CCAssert(node != NULL, "a null node");
	Point pt = node->convertToNodeSpace(point);
	
	COCOPoint cpt(pt.x, pt.y);
	return cpt;
}

COCOPoint NodeUnity::ConvertToWorldPosition(cocos2d::Node *node, COCOPoint point)
{
	CCAssert(node != NULL, "a null node");
	Point pt = node->convertToWorldSpace(point);

	COCOPoint cpt(pt.x, pt.y);
	return cpt;
}

COCOPoint NodeUnity::ConvertToUISpace(cocos2d::Node *node, COCOPoint point)
{
	if (nullptr == node)
	{
		return COCOPoint(0, 0);
	}

	Vec2 worldPoint(node->convertToWorldSpace(point));
	Vec2 pt = CCDirector::getInstance()->convertToUI(worldPoint);
	Size designSize;
	SceneManager::GetInstance()->GetDesignSize(&designSize);

	COCOPoint cpt(pt.x, designSize.height - pt.y);
	return cpt;
}

COCOPoint NodeUnity::GetPosition(cocos2d::Node *node)
{
	CCAssert(node != NULL, "a null node");
	return node->getPosition();
}

void NodeUnity::SetPosition(cocos2d::Node *node, float x, float y)
{
	CCAssert(node != NULL, "a null node");
	node->setPosition(x, y);
}

COCOPoint NodeUnity::GetAnchorPoint(cocos2d::Node *node)
{
	CCAssert(node != NULL, "a null node");
	return node->getAnchorPoint();
}

void NodeUnity::SetAnchorPoint(cocos2d::Node *node, float x, float y)
{
	CCAssert(node != NULL, "a null node");
	node->setAnchorPoint(ccp(x, y));
}

void NodeUnity::SetScaleXFactor(cocos2d::Node *node, float scale_x)
{
	CCAssert(node != NULL, "a null node");
	node->setScaleX(scale_x);
}

void NodeUnity::SetScaleYFactor(cocos2d::Node *node, float scale_y)
{
	CCAssert(node != NULL, "a null node");
	node->setScaleY(scale_y);
}

void NodeUnity::SetScaleFactor(cocos2d::Node *node, float scale)
{
	CCAssert(node != NULL, "a null node");
	node->setScale(scale);
}

float NodeUnity::GetScaleXFactor(cocos2d::Node *node)
{
	CCAssert(node != NULL, "a null node");
	return node->getScaleX();
}

float NodeUnity::GetScaleYFactor(cocos2d::Node *node)
{
	CCAssert(node != NULL, "a null node");
	return node->getScaleY();
}

float NodeUnity::GetScaleFactor(cocos2d::Node *node)
{
	CCAssert(node != NULL, "a null node");
	return node->getScale();
}

float NodeUnity::GetWidth(cocos2d::Node *node)
{
	CCAssert(node != NULL, "a null node");
	return node->getContentSize().width;
}

float NodeUnity::GetHeight(cocos2d::Node *node)
{
	CCAssert(node != NULL, "a null node");
	return node->getContentSize().height;
}

void NodeUnity::SetWidth(cocos2d::Node *node, float width)
{
	CCAssert(node != NULL, "a null node");
	float height = GetHeight(node);
	node->setContentSize(Size(width, height));
}

void NodeUnity::SetHeight(cocos2d::Node *node, float height)
{
	CCAssert(node != NULL, "a null node");
	float width = GetWidth(node);
	node->setContentSize(Size(width, height));
}

void NodeUnity::SetRotation(cocos2d::CCNode *node, float degree)
{
	CCAssert(node != NULL, "a null node");
	node->setRotation(degree);
}

float NodeUnity::GetRotation(cocos2d::CCNode *node)
{
	CCAssert(node != NULL, "a null node");
	return node->getRotation();
}

bool NodeUnity::GetVisible(cocos2d::CCNode *node)
{
	CCAssert(node != NULL, "a null node");
	return node->isVisible();
}

void NodeUnity::SetVisible(cocos2d::CCNode *node, bool visible)
{
	CCAssert(node != NULL, "a null node");
	node->setVisible(visible);
}

//获取z轴深度
ZQ::UINT32 NodeUnity::GetZOrder(cocos2d::CCNode *node)
{
	return node->getLocalZOrder();
}
	
//设置z轴深度
void NodeUnity::SetZOrder(cocos2d::CCNode *node, ZQ::UINT32 order)
{
	CCAssert(node != NULL, "a null node");
	cocos2d::CCNode *parent = node->getParent();
	if(parent)
	{
		parent->reorderChild(node, order);
	}
}

ZQ::UINT32 NodeUnity::Retain(cocos2d::CCNode *node)
{
	CCAssert(node != NULL, "a null node");
	node->retain();
	return node->getReferenceCount();
}

ZQ::UINT32 NodeUnity::Release(cocos2d::CCNode *node)
{
	CCAssert(node != NULL, "a null node");
	int count = node->getReferenceCount() - 1;
	node->release();
	return count;
}

//设置灰度化
void NodeUnity::SetGrayShader(cocos2d::CCNode *node, bool grey)
{
	CCAssert(node != NULL, "a null node");
	if (!node->getGLProgram())
		return;

	GLint mvp_location = node->getGLProgram()->getUniformLocationForName("CC_MVPMatrix");

	if (grey)
	{
		if (mvp_location == -1)
		{
			node->setGLProgram(GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_POSITION_TEXTURE_COLOR_GRAY_NO_MVP));
		}
		else
		{
			node->setGLProgram(GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_POSITION_TEXTURE_COLOR_GRAY_MVP));
		}
	}
	else
	{
		if (mvp_location == -1)
		{
			node->setGLProgram(GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP));
		}
		else
		{
			node->setGLProgram(GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR));
		}
	}
}

//设置高亮
void NodeUnity::SetBrightShader(cocos2d::CCNode *node, bool bright)
{
	CCAssert(node != NULL, "a null node");
	if (!node->getGLProgram())
		return;
	GLint mvp_location = node->getGLProgram()->getUniformLocationForName("CC_MVPMatrix");

	if(bright)
	{
		GLProgram *gl_program;
		if (mvp_location == -1)
		{
			gl_program = GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_POSITION_TEXTURE_COLOR_BRIGHT_NO_MVP);
		}
		else
		{
			gl_program = GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_POSITION_TEXTURE_COLOR_BRIGHT_MVP);
		}

		auto gl_program_state = GLProgramState::getOrCreateWithGLProgram(gl_program);
		auto Location = gl_program->getUniformLocation("bright_value");
		if (Location != -1)
		{
			gl_program_state->setUniformFloat(gl_program->getUniformLocation("bright_value"), 1.6f);
		}
		node->setGLProgramState(gl_program_state);
	}
	else
	{
		GLProgram *gl_program;
		if (mvp_location == -1)
		{
			gl_program = GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP);
		}
		else
		{
			gl_program = GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR);
		}
		node->setGLProgram(gl_program);
	}
}

//调整高亮数值 0.0-2.0
void NodeUnity::SetBrightValue(cocos2d::CCNode *node, float bright_value)
{
	CCAssert(node != NULL, "a null node");
	SpriteEx *sprite = dynamic_cast<SpriteEx*>(node);

	if(sprite == NULL)
		return;

	sprite->SetBright(bright_value);
}

//获取高亮数值
float NodeUnity::GetBrightValue(cocos2d::CCNode *node)
{
	CCAssert(node != NULL, "a null node");
	SpriteEx *sprite = dynamic_cast<SpriteEx*>(node);

	if(sprite == NULL)
		return 0.0f;

	return sprite->GetBright();
}

//暂停所有节点
void PauseNode(CCNode* target_node)
{
	if(target_node == NULL)
		return;

	target_node->pauseAction();
	Vector<Node*> children_list = target_node->getChildren();

	if (children_list.empty())
		return;

	//遍历子树
	for (auto child : children_list)
	{
		CCNode *child_node = dynamic_cast<CCNode*>(child);

		if (child_node == NULL)
			continue;

		PauseNode(child_node);
	}
}

//恢复所有所有节点
void ResumeNode(CCNode* target_node)
{
	if(target_node == NULL)
		return;

	target_node->resumeAction();
	Vector<Node*> children_list = target_node->getChildren();

	if (children_list.empty())
		return;

	//遍历子树
	for (auto child : children_list)
	{
		CCNode *child_node = dynamic_cast<CCNode*>(child);

		if (child_node == NULL)
			continue;

		ResumeNode(child_node);
	}
}

//暂停
void NodeUnity::Pause(cocos2d::CCNode *node)
{
	CCAssert(node != NULL, "a null node");
	PauseNode(node);
}

//恢复
void NodeUnity::Resume(cocos2d::CCNode *node)
{
	CCAssert(node != NULL, "a null node");
	ResumeNode(node);
}

//两个节点是否相同
bool NodeUnity::IsNodeEqual(cocos2d::CCNode *node1, cocos2d::CCNode *node2)
{
	if(node1 == NULL || node2 == NULL)
		return false;

	return node1 == node2;
}
