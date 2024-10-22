#ifndef NODE_UNITY_H
#define NODE_UNITY_H

#include "cocos2d.h"
#include "zqcommon/typedef.h"
#include "view/struct.h"

//��Ϸ�ؼ��Ĺ��ú�����
class NodeUnity
{
public:

	//��ȡĳ�ڵ����������
	static COCOPoint GetWorldPosition(cocos2d::Node *node);
	//����������ת����ĳ�ڵ��������
	static COCOPoint ConvertToNodePosition(cocos2d::Node *node, COCOPoint point);
	//��ĳ�ڵ��������ת������������
	static COCOPoint ConvertToWorldPosition(cocos2d::Node *node, COCOPoint point);
	//��ĳ�ڵ��������ת���ɴ�������
	static COCOPoint ConvertToUISpace(cocos2d::Node *node, COCOPoint point);

	/*------------����---------------*/
	
	//��ȡ�ڵ��ڸ��ڵ��λ��
	static void SetPosition(cocos2d::Node *node, float x, float y);
	static COCOPoint GetPosition(cocos2d::Node *node);
	
	//��ȡ���
	static void SetAnchorPoint(cocos2d::Node *node, float x, float y);
	static COCOPoint GetAnchorPoint(cocos2d::Node *node);
	
	//x�����ű���
	static void SetScaleXFactor(cocos2d::Node *node, float scale_x);
	static float GetScaleXFactor(cocos2d::Node *node);

	//y�����ű���
	static void SetScaleYFactor(cocos2d::Node *node, float scale_y);
	static float GetScaleYFactor(cocos2d::Node *node);
	
	//x-y��ͬ�����ű���
	static void SetScaleFactor(cocos2d::Node *node, float scale);
	static float GetScaleFactor(cocos2d::Node *node);

	//��ȡ���ÿ��
	static void SetWidth(cocos2d::Node *node, float width);
	static void SetHeight(cocos2d::Node *node, float height);
	static float GetWidth(cocos2d::Node *node);
	static float GetHeight(cocos2d::Node *node);
	
	//��ת
	static void SetRotation(cocos2d::Node *node, float degree);
	static float GetRotation(cocos2d::Node *node);
	
	//�Ƿ�ɼ�
	static bool GetVisible(cocos2d::Node *node);
	static void SetVisible(cocos2d::Node *node, bool visible);

	//�Ƿ�ɼ�
	static ZQ::UINT32 GetZOrder(cocos2d::Node *node);
	static void SetZOrder(cocos2d::Node *node, ZQ::UINT32 order);

	//���ӽڵ����
	static ZQ::UINT32 Retain(cocos2d::Node *node);
	//���ٽڵ����
	static ZQ::UINT32 Release(cocos2d::Node *node);

	//���ûҶȻ�
	static void SetGrayShader(cocos2d::Node *node, bool grey);
	
	//���ø���
	static void SetBrightShader(cocos2d::Node *node, bool bright);
	//����������ֵ 0.0-2.0
	static void SetBrightValue(cocos2d::Node *node, float bright_value);
	//��ȡ������ֵ
	static float GetBrightValue(cocos2d::Node *node);
	
	//��ͣ
	static void Pause(cocos2d::Node *node);
	
	//�ָ�
	static void Resume(cocos2d::Node *node);

	//�����ڵ��Ƿ���ͬ
	static bool IsNodeEqual(cocos2d::Node *node1, cocos2d::Node *node2);
};

#endif