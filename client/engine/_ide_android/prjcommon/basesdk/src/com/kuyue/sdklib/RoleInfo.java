package com.kuyue.sdklib;

import org.json.JSONException;
import org.json.JSONObject;

/**
 * 角色信息
 */
public class RoleInfo {
	private String account;
	private String serverID;
	private String serverName;
	private String roleName;
	private String roleID;
	private String roleLevel;
	private String vipLevel;
	private String vipExp;
	//游戏货币(充值获得）
	private String gameMoney;
	//游戏货币II
	private String gameMoneyII;
	private String guildID;
	private String guildName;
	private String roleCreateTime;
	private String sex;
	private String power;
	private String guildRoleID;
	private String guildRoleName;
	private String profession;
	private String professionName;
	private String friendList;

	public String getAccount() {
		return account;
	}
	public void setAccount(String account) {
		this.account = account;
	}
	public String getServerID() {
		return serverID;
	}
	public void setServerID(String serverID) {
		this.serverID = serverID;
	}
	public String getServerName() {
		return serverName;
	}
	public void setServerName(String serverName) {
		this.serverName = serverName;
	}
	public String getRoleName() {
		return roleName;
	}
	public void setRoleName(String roleName) {
		this.roleName = roleName;
	}
	public String getRoleID() {
		return roleID;
	}
	public void setRoleID(String roleID) {
		this.roleID = roleID;
	}
	public String getRoleLevel() {
		return roleLevel;
	}
	public void setRoleLevel(String roleLevel) {
		this.roleLevel = roleLevel;
	}
	public String getVipLevel() {
		return vipLevel;
	}
	public void setVipLevel(String vipLevel) {
		this.vipLevel = vipLevel;
	}
	public String getVipExp() {
		return vipExp;
	}
	public void setVipExp(String vipExp) {
		this.vipExp = vipExp;
	}
	public String getGameMoney() {
		return gameMoney;
	}
	public void setGameMoney(String gameMoney) {
		this.gameMoney = gameMoney;
	}
	public String getGameMoneyII() {
		return gameMoneyII;
	}
	public void setGameMoneyII(String gameMoneyII) {
		this.gameMoneyII = gameMoneyII;
	}
	public String getGuildID() {
		return guildID;
	}
	public void setGuildID(String guildID) {
		this.guildID = guildID;
	}
	public String getGuildName() {
		return guildName;
	}
	public void setGuildName(String guildName) {
		this.guildName = guildName;
	}
	public String getRoleCreateTime() {
		return roleCreateTime;
	}
	public void setRoleCreateTime(String roleCreateTime) {
		this.roleCreateTime = roleCreateTime;
	}
	public String getSex() {
		return sex;
	}
	public void setSex(String sex) {
		this.sex = sex;
	}
	public String getPower() {
		return power;
	}
	public void setPower(String power) {
		this.power = power;
	}
	public String getGuildRoleID() {
		return guildRoleID;
	}
	public void setGuildRoleID(String guildRoleID) {
		this.guildRoleID = guildRoleID;
	}
	public String getGuildRoleName() {
		return guildRoleName;
	}
	public void setGuildRoleName(String guildRoleName) {
		this.guildRoleName = guildRoleName;
	}
	public String getProfession() {
		return profession;
	}
	public void setProfession(String profession) {
		this.profession = profession;
	}
	public String getProfessionName() {
		return professionName;
	}
	public void setProfessionName(String professionName) {
		this.professionName = professionName;
	}
	public String getFriendList() {
		return friendList;
	}
	public void setFriendList(String friendList) {
		this.friendList = friendList;
	}	

	public static RoleInfo parseRoleInfo(String roleInfoJson)
	{
		RoleInfo roleInfo = null;
		try 
		{
			JSONObject jsonObj = new JSONObject(roleInfoJson);

			roleInfo = new RoleInfo();
			roleInfo.setAccount(jsonObj.optString("account"));// 账号
			roleInfo.setServerID(jsonObj.optString("server_id"));// 服务器ID，其值必须为数字字符串
			roleInfo.setServerName(jsonObj.optString("server_name"));// 服务器名称
			roleInfo.setRoleName(jsonObj.optString("role_name"));// 角色名称
			roleInfo.setRoleID(jsonObj.optString("role_id"));// 角色ID
			roleInfo.setRoleLevel(jsonObj.optString("role_level"));// 等级
			roleInfo.setVipLevel(jsonObj.optString("vip_level"));// VIP等级
			roleInfo.setVipExp(jsonObj.optString("vip_exp"));// VIP经验
			roleInfo.setGameMoney(jsonObj.optString("game_money"));// 角色现有金额
			roleInfo.setGameMoneyII(jsonObj.optString("game_money_ii"));// 角色现有金额			
			roleInfo.setGuildName(jsonObj.optString("guild_name"));// 公会名
			roleInfo.setRoleCreateTime(jsonObj.optString("role_create_time")); // UC与1881渠道必传，值为10位数时间戳
			roleInfo.setGuildID(jsonObj.optString("guild_id")); // 360渠道参数，设置帮派id，必须为整型字符串
			roleInfo.setSex(jsonObj.optString("sex")); // 360渠道参数
			roleInfo.setPower(jsonObj.optString("battle_value")); // 360渠道参数，设置角色战力，必须为整型字符串
			roleInfo.setGuildRoleID(jsonObj.optString("guild_role_id")); // 360渠道参数，设置角色在帮派中的id
			roleInfo.setGuildRoleName(jsonObj.optString("guild_role_name")); // 360渠道参数，设置角色在帮派中的名称
			roleInfo.setProfession(jsonObj.optString("profession")); // 360渠道参数，设置角色职业id，必须为整型字符串
			roleInfo.setProfessionName(jsonObj.optString("profession_name")); // 360渠道参数，设置角色职业名称
			roleInfo.setFriendList(jsonObj.optString("friend_list")); // 360渠道参数，设置好友关系列表，格式请参考：http://open.quicksdk.net/help/detail/aid/190						
		} 
		catch (JSONException e) 
		{
			e.printStackTrace();
		}
		return roleInfo;
	}	
}
