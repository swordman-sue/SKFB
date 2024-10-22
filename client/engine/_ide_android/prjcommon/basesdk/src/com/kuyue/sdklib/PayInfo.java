package com.kuyue.sdklib;

import org.json.JSONException;
import org.json.JSONObject;

import android.R.integer;
import android.util.Log;

/**
 * SDK支付信息
 */
public class PayInfo 
{
	private static final String TAG = "PayInfo";
	
	private String payType;
	private String orderID;
	private String goodsID;
	private String goodsName;
	private String goodsDesc;
	private Integer goodsNum;
	private Double 	money;
	private String currencyType;
	private String notifyUrl;
	private String extrasParams;
	private String sign;

	private String account;
	private String serverId;
	private String serverName;
	private String roleID;
	private String roleName;
	private String roleLevel;
	private String payTime;
	
	private String iapInfo;
	private int mycardSandboxMode;
	private String mycardNotifyPayRetUrl;

	public PayInfo() {}

	public void setGoodsID(String var) {
		goodsID = var;
	}		
	public String getGoodsID() {
		return goodsID;
	}

	public void setGoodsName(String var) {
		goodsName = var;
	}		
	public String getGoodsName() {
		return goodsName;
	}

	public void setGoodsNum(Integer var) {
		goodsNum = var;
	}
	public Integer getGoodsNum() {
		return goodsNum;
	}

	public String getGoodsDesc() {
		return goodsDesc;
	}

	public void setGoodsDesc(String goodsDesc) {
		this.goodsDesc = goodsDesc;
	}

	public void setMoney(Double var) {
		money = var;
	}
	public Double getMoney() {
		return money;
	}
	
	public void setOrderID(String var) {
		orderID = var;
	}
	public String getOrderID() {
		return orderID;
	}

	public void setNotifyUrl(String var) {
		notifyUrl = var;
	}
	public String getNotifyUrl() {
		return notifyUrl;
	}

	public String getAccount() {
		return account;
	}
	public void setAccount(String account) {
		this.account = account;
	}

	public void setRoleID(String var) {
		roleID = var;
	}	
	public String getRoleID() {
		return roleID;
	}

	public void setRoleName(String var) {
		roleName = var;
	}		
	public String getRoleName() {
		return roleName;
	}

	public void setRoleLevel(String var) {
		roleLevel = var;
	}		
	public String getRoleLevel() {
		return roleLevel;
	}

	public void setServerId(String var) {
		serverId = var;
	}		
	public String getServerId() {
		return serverId;
	}

	public String getServerName() {
		return serverName;
	}
	public void setServerName(String serverName) {
		this.serverName = serverName;
	}

	public void setExtrasParams(String var) {
		extrasParams = var;
	}		
	public String getExtrasParams() {
		return extrasParams;
	}

	public String getSign() {
		return sign;
	}
	public void setSign(String sign) {
		this.sign = sign;
	}

	public String getIapInfo() {
		return iapInfo;
	}

	public void setIapInfo(String iapInfo) {
		this.iapInfo = iapInfo;
	}

	public String getCurrencyType() {
		return currencyType;
	}

	public void setCurrencyType(String payType) {
		this.currencyType = payType;
	}

	public String getPayType() {
		return payType;
	}

	public void setPayType(String payType) {
		this.payType = payType;
	}

	public int getMycardSandboxMode() {
		return mycardSandboxMode;
	}

	public void setMycardSandboxMode(int sandboxMode) {
		this.mycardSandboxMode = sandboxMode;
	}

	public String getMycardNotifyPayRetUrl() {
		return mycardNotifyPayRetUrl;
	}
	
	public void setMycardNotifyPayRetUrl(String notifyPayRetUrl) {
		this.mycardNotifyPayRetUrl = notifyPayRetUrl;
	}

	public void setPayTime(String paytime) {
		this.payTime = paytime;
	}

	public String getPayTime() {
		return payTime;
	}
	
	public static PayInfo parsePayInfo(String PayInfoJson)
	{
		PayInfo payInfo = null;
		try 
		{
			JSONObject jsonObj = new JSONObject(PayInfoJson);

			payInfo = new PayInfo();

			payInfo.setPayType(jsonObj.optString("pay_type"));			
			payInfo.setOrderID(jsonObj.optString("order_id"));
			payInfo.setGoodsID(jsonObj.optString("recharge_id"));
			payInfo.setGoodsName(jsonObj.optString("goods_name"));
			payInfo.setGoodsDesc(jsonObj.optString("goods_desc"));
			payInfo.setGoodsNum(jsonObj.getInt("recharge_num"));
			payInfo.setMoney(jsonObj.getDouble("money"));
			payInfo.setCurrencyType(jsonObj.optString("currency_type"));			
			payInfo.setExtrasParams(jsonObj.optString("extra_params"));
			payInfo.setNotifyUrl(jsonObj.optString("notify_url"));
			payInfo.setSign(jsonObj.optString("sign"));

			payInfo.setAccount(jsonObj.optString("account"));
			payInfo.setServerId(jsonObj.optString("server_id"));
			payInfo.setServerName(jsonObj.optString("server_name"));// 服务器名称
			payInfo.setRoleID(jsonObj.optString("role_id"));
			payInfo.setRoleName(jsonObj.optString("role_name"));
			payInfo.setRoleLevel(jsonObj.optString("role_level"));
			payInfo.setPayTime(jsonObj.optString("td_time"));
			
			payInfo.setIapInfo(jsonObj.optString("iap_info"));
			
			payInfo.setMycardSandboxMode(jsonObj.optInt("mycard_sandbox_mode"));
			payInfo.setMycardNotifyPayRetUrl(jsonObj.optString("mycard_notify_pay_result_url"));
		} 
		catch (JSONException e) 
		{
			e.printStackTrace();
		}
		return payInfo;
	}
}
