package com.szba.skfb.quickpp;

import java.util.UUID;

import org.json.JSONException;
import org.json.JSONObject;

import com.kuyue.sdklib.PayInfo;
import com.kuyue.sdklib.RoleInfo;
import com.quicksdk.entity.GameRoleInfo;
import com.quicksdk.entity.OrderInfo;

// QuickSDK���õ�����Ϸ��Ϣ
public class QKGameInfo 
{	
	private static final String TAG = "QKGameInfo";
		
	// ������ɫ��Ϣ������֧����
	public static GameRoleInfo parseBaseRoleInfo(String info)
	{	
		RoleInfo roleInfo = RoleInfo.parseRoleInfo(info);
		
		GameRoleInfo qkRoleinfo = new GameRoleInfo();	
		qkRoleinfo.setServerID(roleInfo.getServerID());// ������ID����ֵ����Ϊ�����ַ���
		qkRoleinfo.setServerName(roleInfo.getServerName());// ����������
		qkRoleinfo.setGameRoleName(roleInfo.getRoleName());// ��ɫ����
		qkRoleinfo.setGameRoleID(roleInfo.getRoleID());// ��ɫID
		qkRoleinfo.setGameUserLevel(roleInfo.getRoleLevel());// �ȼ�
		qkRoleinfo.setVipLevel(roleInfo.getVipLevel());// VIP�ȼ�
		qkRoleinfo.setGameBalance(roleInfo.getGameMoney());// ��ɫ���н��
		qkRoleinfo.setPartyName(roleInfo.getGuildName());// ������		

		qkRoleinfo.setRoleCreateTime(roleInfo.getRoleCreateTime()); // UC��1881�����ش���ֵΪ10λ��ʱ���
		qkRoleinfo.setPartyId(roleInfo.getGuildID()); // 360�������������ð���id������Ϊ�����ַ���
		qkRoleinfo.setGameRoleGender(roleInfo.getSex()); // 360��������
		qkRoleinfo.setGameRolePower(roleInfo.getPower()); // 360�������������ý�ɫս��������Ϊ�����ַ���
		qkRoleinfo.setPartyRoleId(roleInfo.getGuildRoleID()); // 360�������������ý�ɫ�ڰ����е�id
		qkRoleinfo.setPartyRoleName(roleInfo.getGuildRoleName()); // 360�������������ý�ɫ�ڰ����е�����
		qkRoleinfo.setProfessionId(roleInfo.getProfession()); // 360�������������ý�ɫְҵid������Ϊ�����ַ���
		qkRoleinfo.setProfession(roleInfo.getProfessionName()); // 360�������������ý�ɫְҵ����
		qkRoleinfo.setFriendlist(roleInfo.getFriendList()); // 360�������������ú��ѹ�ϵ�б���ʽ��ο���http://open.quicksdk.net/help/detail/aid/190						

		return qkRoleinfo;
	};

	// ������ɫ��Ϣ�������ϴ���ɫ��Ϣ��
	public static GameRoleInfo parseDetailRoleInfo(String info)
	{
		return parseBaseRoleInfo(info);
	}		
	
	// ����������Ϣ������֧����
	public static OrderInfo parseOrderInfo(String info)
	{
		PayInfo payInfo = PayInfo.parsePayInfo(info);

		OrderInfo orderInfo = new OrderInfo();
		orderInfo.setCpOrderID(payInfo.getOrderID());// ��Ϸ������
		orderInfo.setGoodsID(payInfo.getGoodsID()); // ��ƷID������ʶ����Ĳ�Ʒ
		orderInfo.setGoodsName(payInfo.getGoodsName());// ��Ʒ����
		orderInfo.setCount(payInfo.getGoodsNum());// �����������繺��"10Ԫ��"��10
		orderInfo.setAmount(payInfo.getMoney()); // �ܽ���λΪԪ��
		orderInfo.setExtrasParams(payInfo.getExtrasParams()); // ͸������					
		return orderInfo;
	}
}
