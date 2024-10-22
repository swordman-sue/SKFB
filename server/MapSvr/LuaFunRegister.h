#pragma once
#include "../LuaEngine/LuaString.h"

class GameObject;

class LuaFunRegister
{
public:
	LuaFunRegister(void);
	~LuaFunRegister(void);

public:

	//����ע��
	//�������Ƿ�ΪNULL
	static DWORD IsNull(VOID* pVoid);
	static DWORD IsNotNull(VOID* pVoid);

	//���һ�������(1-100)
	static DWORD GetRandom();

	//��ö����ObjectKey
	static DWORD GetObjectKey(VOID* pVoid);

	//��ö��������
	static DWORD GetObjectType(VOID* pVoid);

	//����ObjectKey���Ҷ���
	static VOID* FindObject(VOID* pVoid, DWORD dwObjectKey);

	//��ö��������
	static DWORD GetRace(VOID* pVoid);

	//��ö�����ɫ(ϡ�ж�)
	static DWORD GetColor(VOID* pVoid);

	//��ö���ȼ�(�Ǽ�)
	static DWORD GetLevel(VOID* pVoid);

	//�����ӵ�ID
	static DWORD GetRetinueID(VOID* pVoid);

	//�����ӵĿ���ID
	static DWORD GetReitnueCardID(VOID* pVoid);

	//�����ӵ�����
	static DWORD GetRetinueType(VOID* pVoid);

	//�����ӵ�������
	static DWORD GetRetinueSubType(VOID* pVoid);

	//��ö��������ֵ
	static INT	 GetAttr(VOID* pVoid, DWORD dwAttrType, DWORD dwAttrKind);

	//���ö��������ֵ
	static VOID	 SetAttr(VOID* pVoid, DWORD dwAttrType, DWORD dwAttrKind, INT nValue);

	//���Ӷ��������ֵ
	static VOID	 AddAttr(VOID* pVoid, DWORD dwAttrType, DWORD dwAttrKind, INT nValue);

	//������������ֵ
	static VOID	 ClearAttr(VOID* pVoid, DWORD dwAttrType);

	//�����Ƿ����ĳ��״̬
	static DWORD HasStatus(VOID* pVoid, DWORD dwStatus);

	//���ö����״̬
	static VOID	 SetStatus(VOID* pVoid, DWORD dwStatus);

	//��������״̬
	static VOID	 ClearStatus(VOID* pVoid, DWORD dwStatus);

	//�����Ƿ�Ϊս�����
	static DWORD IsBattleRetinue(VOID* pVoid);

	//�����Ƿ�Ϊħ�����
	static DWORD IsMagicRetinue(VOID* pVoid);

	//�����Ƿ�Ϊ�������
	static DWORD IsSpellRetinue(VOID* pVoid);

	//�����Ƿ�Ϊ�������
	static DWORD IsTrapRetinue(VOID* pVoid);

	//�����Ƿ�Ϊװ�����
	static DWORD IsEquipmentRetinue(VOID* pVoid);

	//�����Ƿ�Ϊ�������
	static DWORD IsEnvRetinue(VOID* pVoid);

	//�����Ƿ���
	static DWORD IsAlive(VOID* pVoid);

	//�����Ƿ�����
	static DWORD IsDead(VOID* pVoid);

	//�����Ƿ��ѷ���Ӫ
	static DWORD IsFriend(VOID* pVoid, VOID* pTarget);

	//���ö�������
	static VOID  SetDead(VOID* pVoid, VOID* pMurderer);

	//��ü����������ָ��
	static VOID* GetChief(VOID* pVoid);

	//��õз��������ָ��
	static VOID* GetEnemyChief(VOID* pVoid);

	//���Ӽ�������HP
	static VOID	 AddChiefHp(VOID* pVoid, INT nVal);

	//���ӵз�����HP
	static VOID	 AddEnemyChiefHp(VOID* pVoid, INT nVal);

	//����ҷ�����ս���������
	static DWORD GetBattleRetinueNum(VOID* pVoid);

	//����ҷ�����ħ���������
	static DWORD GetMagicRetinueNum(VOID* pVoid);

	//��õз�����ս���������
	static DWORD GetEnemyBattleRetinueNum(VOID* pVoid);

	//��õз�����ħ���������
	static DWORD GetEnemyMagicRetinueNum(VOID* pVoid);

	//����ҷ�ս�����ָ��
	static VOID* GetBattleRetinueByIndex(VOID* pVoid, DWORD dwIndex);

	//����ҷ�ħ�����ָ��
	static VOID* GetMagicRetinueByIndex(VOID* pVoid, DWORD dwIndex);

	//��õз�ս�����ָ��
	static VOID* GetEnemyBattleRetinueByIndex(VOID* pVoid, DWORD dwIndex);

	//��õз�ħ�����ָ��
	static VOID* GetEnemyMagicRetinueByIndex(VOID* pVoid, DWORD dwIndex);

	//����ҷ�Ĺ�ؿ�������
	static DWORD GetGraveCardNum(VOID* pVoid);

	//����ҷ�Ĺ�ؿ���ָ��
	static VOID* GetGraveCardByIndex(VOID* pVoid, DWORD dwIndex);

	//��õз�Ĺ�ؿ�������
	static DWORD GetEnemyGraveCardNum(VOID* pVoid);

	//��õз�Ĺ�ؿ���ָ��
	static VOID* GetEnemyGraveCardByIndex(VOID* pVoid, DWORD dwIndex);

	//����ҷ���ǰ��������
	static DWORD GetHandCardNum(VOID* pVoid);

	//����ҷ�����ָ��
	static VOID* GetHandCardByIndex(VOID* pVoid, DWORD dwIndex);

	//����ҷ���ǰ���鿨������
	static DWORD GetGroupCardNum(VOID* pVoid);

	//��õз���ǰ��������
	static DWORD GetEnemyHandCardNum(VOID* pVoid);

	//��õз���ǰ���鿨������
	static DWORD GetEnemyGroupCardNum(VOID* pVoid);

	//��ð�Ŀ�����
	static VOID* GetBindObject(VOID* pVoid);

	//��һ��Ŀ�����
	static VOID  SetBindObject(VOID* pVoid, VOID* pTarget);

	//���Ŀ���
	static VOID  ClearBind(VOID* pVoid);

	//�Ƿ񱻴�����
	static DWORD IsTrigger(VOID* pVoid);

	//���ô�����־
	static VOID	 SetTrigger(VOID* pVoid);

	//��ýű�������
	static INT   GetScriptData(VOID* pVoid, BYTE byIndex);

	//���ýű�������
	static VOID  SetScriptData(VOID* pVoid, BYTE byIndex, INT nVal);

	//����һ���⻷
	static VOID  AddAura(VOID* pVoid, DWORD dwAuraID);

	//����׼��������
	static DWORD GetSacrificeNum(VOID* pVoid);

	//��������
	static VOID	 DiscardHandCard(VOID* pVoid, BYTE byCardNum);

	//���Ŀ���Ƿ���������
	static DWORD IsConformCond(VOID* pVoid, BYTE byCondType, BYTE byCondSign, DWORD dwCondVal);

	//���ָ�������Ŀ��鿨������
	static BYTE	 GetGroupCardNumByCond(VOID* pVoid, BYTE byCondType, BYTE byCondSign, DWORD dwCondVal);

	//���ָ����������������
	static BYTE	 GetHandCardNumByCond(VOID* pVoid, BYTE byCondType, BYTE byCondSign, DWORD dwCondVal);

	//���ָ��������Ĺ�ؿ�������
	static BYTE	 GetGraveCardNumByCond(VOID* pVoid, BYTE byCondType, BYTE byCondSign, DWORD dwCondVal);

	//��ָ����������
	static VOID	 DrawCardByCond(VOID* pVoid, BYTE byCardNum, BYTE byCondType, BYTE byCondSign, DWORD dwCondVal);

	//����ָ������������
	static VOID	 DiscardHandCardByCond(VOID* pVoid, BYTE byCardNum, BYTE byCondType, BYTE byCondSign, DWORD dwCondVal);

	//����ָ�������Ŀ����ڿ���
	static VOID	 DiscardGroupCardByCond(VOID* pVoid, BYTE byCardNum, BYTE byCondType, BYTE byCondSign, DWORD dwCondVal);

	//����Ĺ�ؿ���
	static VOID	 ReliveCard(VOID* pVoid, DWORD dwCardKey, BYTE byRelivePos, DWORD dwIsFirstDraw);

	//�������ٻ����
	static VOID	 SummonRetinueFromHand(VOID* pVoid, BYTE byNum, BYTE byCondType, BYTE byCondSign, DWORD dwCondVal, BYTE byOrien);

	//�ӿ����ٻ����
	static VOID	 SummonRetinueFromGroup(VOID* pVoid, BYTE byNum, BYTE byCondType, BYTE byCondSign, DWORD dwCondVal, BYTE byOrien);

	//�������
	static VOID	 ControlRetinue(VOID* pVoid, VOID* pTarget);

	//��ת���
	static VOID	 TurnRetinue(VOID* pVoid, DWORD dwIsTriggerScript);

	//�������
	static VOID	 CoverRetinue(VOID* pVoid, DWORD dwIsTriggerScript);

	//�˻����
	static VOID	 SendBackRetinue(VOID* pVoid, VOID* pTarget, BYTE bySendBackPos, DWORD dwIsFirstDraw);

	//�ͷŷ���
	static VOID	 CastSpell(VOID* pVoid, DWORD dwSpellID, DWORD dwTargetKey);

	//���ս������
	static DWORD GetEnv(VOID* pVoid);

	//����ս������
	static VOID	 SetEnv(VOID* pVoid, DWORD dwEnv);

	//���ս������
	static VOID	 ClearEnv(VOID* pVoid);

	static bool  IsAcceptMission(VOID* pVoid, DWORD dwMissionID );

	static VOID  TriggerMission(VOID* pVoid, DWORD dwMissionID );

};
