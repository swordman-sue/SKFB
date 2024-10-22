/**\file
�̵������
\author Kevin
\date 2016/7/5
\note
*/

#pragma once
#include "DataDefine.h"
#include "DBCDefine.h"
class Player;

class CShopManager
{
public:
	CShopManager();
	~CShopManager();

	void Init(Player *pPlayer);

	void OnNewRole();

	void OnNewDay();

	void OnNewWeek();

	void OnNewMonth();

	// �Ƿ��к��
	bool IsHaveRedPoint(BYTE byShopType);

public:
	// ��Ʒ����
	void OnGoodsBuy(BYTE byType, WORD wGoodsId, WORD wBuyNum, WORD wContext);

	// ������Ʒ�����¼
	void OnGoodsBuyRecord(BYTE byType);

	// ��Ʒˢ��
	void OnGoodsRefresh(BYTE byShopType, BYTE byRefreshType);

	// ��Ʒ����
	void OnGoodsSell(BYTE byObjType, DWORD *pGoodsList, BYTE byGoodsNum);

	// ������ʱ��Ʒ�б�
	void OnNetLimitGoodsList(BYTE byType);

public:
	// Ӣ���̵���������
	void OnHeroShopReq();

	// Ӣ���̵�ˢ��
	void OnHeroSoulShopRefresh(BYTE byShopType, BYTE byRefreshType);

	// �����̵���������
	void OnNetAwakenShopReq();

	// �����̵�ˢ��
	void OnNetAwakenShopRefresh(BYTE byRefreshType);

	// �����̵���������
	void OnNetStarSoulShopReq();

	// �ǻ��̵�ˢ��
	void OnNetStarSoulShopRefresh(BYTE byRefreshType);

	// ˮ���̵���������
	void OnNetCrystalShopData();

	// ˮ���̵�ˢ��
	void OnNetCrystalShopRefresh();

	// ֱ���̵���������
	void OnNetDirectBuyShopReq();

	// ����ֱ���̵�����
	void UpdateDirectBuyShop(WORD wUpdateType);

	// ��ȡֱ����Ʒ
	void OnNetTakeDirectBuyGoods(WORD wGoodsId);

public:

	// ȡ��Ʒ�����¼����
	WORD GetGoodsBuyRecordNum(BYTE byType, WORD wGoodsId);

	// ȡ��Ʒ�����¼(����̵���Ҫ����Ľӿ�)
	const ShopGoodsBuyRecord* GetGoodsBuyRecordEx(BYTE byType, WORD wGoodsId);

	// ����ֱ���̵�����
	void SendDirectBuyShopData();

	// ֱ���̵�ͨ����ֵ����
	bool DirectBuyShopBuyByRecharge(WORD wGoodsId);
protected:

	// ����ͨ����Ʒ����
	bool DoCommonGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum, WORD wItemFromId, WORD wCurrencyFromId);

	// �̳ǵ��߹���
	bool MallItemBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// �̳��������
	bool MallGiftBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// ������Ʒ����
	bool GuildGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// Ӣ����Ʒ����
	bool HeroGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// �Ѿ���Ʒ����
	bool RebelGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// ������Ʒ����
	bool TowerGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// ��������Ʒ����
	bool ArenaGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// ������Ʒ����
	bool AwakenGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// �ǻ���Ʒ����
	bool StarSoulGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// ��������Ʒ����
	bool ScoreMatchGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// 7���̵깺��
	bool SevenDayShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// �ȼ�����̵깺��
	bool LevelGiftShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum, WORD wRoleLevel);

	// �޾�֮���̵�
	bool EndlessLandShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// ֱ���̵깺��
	bool DirectBuyShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// �һ��̵깺��
	bool ExchangeShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// �Ƽ��̵깺��
	bool RecommendShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// �����ͨ�̵깺��
	bool TeamDungeonNormalShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// ��ӽ����̵깺��
	bool TeamDungeonRewardShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// �����̵���Ʒ����
	bool DestinyShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// �����̵���Ʒ����
	bool HideShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// ʥ���ڻ��ֶһ���Ʒ����
	bool ChristmasScoreExchangeShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// ʥ������Ʒ�һ���Ʒ����
	bool ChristmasItemExchangeShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// ˮ���̵깺��
	bool CrystalShopBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// ������Ʒ����
	bool HotSaleGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// ��Դս��Ʒ����
	bool ResourceWarGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum);

	// GM��Ʒ����
	bool GMShopGoodsBuy(BYTE byShopType, WORD wGoodsId, WORD wBuyNum, WORD wGM);

protected:
	// ������Ʒ
	bool DoSellItem(const DWORD *pGoodsList, BYTE byGoodsNum, vector<MoneyData> &vCurrencyList);

	// ����Ӣ��
	bool DoSellHero(const DWORD *pGoodsList, BYTE byGoodsNum, vector<MoneyData> &vCurrencyList);

	// ����װ��
	bool DoSellEquipemnt(const DWORD *pGoodsList, BYTE byGoodsNum, vector<MoneyData> &vCurrencyList);

	// ���۱���
	bool DoSellTreasure(const DWORD *pGoodsList, BYTE byGoodsNum, vector<MoneyData> &vCurrencyList);

// �����̵����
protected:

	// ����ˢ���̵���Ʒ
	void CreateRefreshShopGoods(BYTE byShopType);

	// ����ˢ���̵�����
	void SendRefreshShopData(BYTE byShopType);

	// �����̵�ˢ����Ѵ���
	void UpdateShopRefreshFreeTimes(BYTE byShopType);

	// ȡˢ���̵�����
	RefreshShopData* GetRefreshShopData(BYTE byShopType);

	// ������Դս�̵�����
	void UpdateResourceWarShop(WORD wRefreshType);

protected:

	// ȡ��Ʒ�����¼(����̵���Ҫ����Ľӿ�)
	ShopGoodsBuyRecord* GetGoodsBuyRecord(BYTE byType, WORD wGoodsId);

	// �����Ʒ�����¼(����̵���Ҫ����Ľӿ�)
	bool AddGoodsBuyRecord(BYTE byType, WORD wGoodsId, WORD wGoodsNum);

	// ������Ʒ�����¼�б�
	void SendGoodsBuyRecordList(BYTE byType, BYTE byRecordNum, ShopGoodsBuyRecord *pRecordList);

	// �����̵�����
	void ResetShopData(BYTE byShopType);

	// ����Ƿ�����������
	bool CheckIsBuyNumLimit(BYTE byShopType, WORD wGoodsId, WORD wBuyNum, WORD wBuyNumMax);

private:
	Player *m_pPlayer;
	PlayerShopData *m_pShopData;
};