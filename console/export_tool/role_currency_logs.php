<?php
	require 'include/common.php';
	//$v = empty($_GET['v']) ? '' : trim($_GET['v']);
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<?
    /*本例是用PHP4连接一个mysql数据库操作的演示，
    实现连接打开一个库，并读取数据的基本功能。
    */
?>
<HTML>
<HEAD>
<META NAME="GENERATOR" Content="Microsoft Visual Studio 6.0">
<style type="text/css">
<!--
input { font-size:9pt;}
A:link {text-decoration: underline; font-size:9pt;color:000059}
A:visited {text-decoration: underline; font-size:9pt;color:000059}
A:active {text-decoration: none; font-size:9pt}
A:hover {text-decoration:underline;color:red}
body,table {font-size: 9pt}
tr,td{font-size:9pt}
<td align="center" height="19"><?echo $row["id"]?></td>
-->
</style>
<title>角色货币日志详情</title>
</HEAD>
<body alink="#FF0000" link="#000099" vlink="#CC6600" topmargin="8" leftmargin="0" bgColor="#FFFFFF">
<br><br><center><font color=green size=3><b>角色货币日志详情</b></font></center>
<br>
	<form action="<?php echo $PHP_SELF; ?>" method="get" name="frmAdduser">
	<table cellspacing=0 bordercolordark=#FFFFFF width="50%" bordercolorlight=#000000 border=1 align="center" cellpadding="2">
	  <tr bgcolor="#6b8ba8" style="color:FFFFFF">
		<td width="100%" align="center" valign="bottom" height="19" colspan="2">查询条件</td>
	  </tr>
	  <tr>
		<td width="30%" align="right" height="19">角色ID：</td>
		<td width="70%"><input type="text" name="role_id" size="10" maxlength="10"></td>
	  </tr> 
	  <tr>
		<td width="30%" align="right" height="19">货币类型(5=钻石,3=金币)：</td>
		<td width="70%"><input type="text" name="currency_type" size="10" maxlength="10"></td>
	  </tr> 	  
	  <tr>	
		<td width="100%" align="center" colspan="2"><input type="submit" value="查询"></td>
	  </tr> 
	</table>
	</form>

<table cellspacing=0 bordercolordark=#FFFFFF width="50%" bordercolorlight=#000000 border=1 align="center" cellpadding="2">
  <tr bgcolor="#6b8ba8" style="color:FFFFFF">
	<td width="2%" align="center" valign="bottom">角色ID</td>
    <td width="2%" align="center" valign="bottom">角色等级</td>
	<td width="2%" align="center" valign="bottom">货币类型</td>
	<td width="2%" align="center" valign="bottom">货币值</td>
	<td width="2%" align="center" valign="bottom">当前货币值</td>
	<td width="2%" align="center" valign="bottom">来源描述</td>
	<td width="2%" align="center" valign="bottom">操作时间</td>
  </tr>
<?php    
	// 货币配置
	$currency_name_list = array();
	$currency_name_list[3] =  "金币";
	$currency_name_list[5] =  "钻石";
	$currency_name_list[6] =  "雄魂";
	$currency_name_list[12] =  "爬塔声望";
	$currency_name_list[13] =  "竞技场声望";
	$currency_name_list[14] =  "叛军值(战功)";
	$currency_name_list[17] =  "神魂";
	$currency_name_list[22] =  "荣誉";
	$currency_name_list[24] =  "仙魂";
	$currency_name_list[25] =  "积分赛荣誉";
	
	// 来源描述
	$currency_from_des = array(); 
	$currency_from_des[0] = "未知";
	$currency_from_des[1] = "基金";
	$currency_from_des[2] = "竞技场挑战";
	$currency_from_des[3] = "爬塔挑战购买次数";
	$currency_from_des[4] = "爬塔重置副本";
	$currency_from_des[5] = "爬塔购买星数商品";
	$currency_from_des[6] = "创建工会";
	$currency_from_des[7] = "英雄高级召唤";
	$currency_from_des[8] = "领地升级";
	$currency_from_des[9] = "主线副本重置";
	$currency_from_des[10] = "精英副本重置";
	$currency_from_des[11] = "领地暴动镇压";
	$currency_from_des[12] = "争霸赛挑战次数购买";
	$currency_from_des[13] = "英雄重生";
	$currency_from_des[14] = "装备重生";
	$currency_from_des[15] = "宝物重生";
	$currency_from_des[16] = "英雄分解";	
	$currency_from_des[17] = "装备分解";
	$currency_from_des[18] = "积分赛刷新目标";
	$currency_from_des[19] = "积分赛挑战次数购买";
	$currency_from_des[20] = "商城道具购买";
	$currency_from_des[21] = "宝物熔炼";
	$currency_from_des[22] = "宝物强化";
	$currency_from_des[23] = "宝物精炼";
	$currency_from_des[24] = "世界BOSS挑战次数购买";
	$currency_from_des[25] = "摇钱树奖励";
	$currency_from_des[26] = "铜雀台奖励";
	$currency_from_des[27] = "夺宝挑战";
	$currency_from_des[28] = "装备强化";
	$currency_from_des[29] = "仙灵突破";	
	$currency_from_des[30] = "英雄副本挑战";
	$currency_from_des[31] = "日常副本挑战";
	$currency_from_des[32] = "爬塔副本挑战";
	$currency_from_des[33] = "英雄升级";
	$currency_from_des[34] = "英雄突破";
	$currency_from_des[35] = "英雄培养";
	$currency_from_des[36] = "英雄觉醒";
	$currency_from_des[37] = "物品分解";
	$currency_from_des[38] = "使用物品";
	$currency_from_des[39] = "新建角色";
	$currency_from_des[40] = "叛军奖励";	
	$currency_from_des[41] = "领取中心";
	$currency_from_des[42] = "觉醒商店刷新";
	$currency_from_des[43] = "英雄商店刷新";
	$currency_from_des[44] = "主线副本挑战";
	$currency_from_des[45] = "精英副本挑战";
	
	// 取挑转过来的参数
	$role_id = empty($_GET['role_id']) ? '0' : trim($_GET['role_id']);
	$currency_type = empty($_GET['currency_type']) ? '0' : trim($_GET['currency_type']);
	
	// 根据角色ID查询
	if($role_id > 0 and $currency_type > 0)
	{
		$sql_condiction = " where role_id = $role_id and type= $currency_type";
	}
	else if($role_id > 0)
	{
		$sql_condiction = " where role_id = $role_id";
	}	
	else if($currency_type > 0)
	{
		$sql_condiction = " where type = $currency_type limit 0, 500";
	}
	else
	{
		$sql_condiction = " limit 0, 500";
	}
		
    $strSql= "select role_id,role_level,type,value,cur_value,from_id,time from currency_logs".$sql_condiction;
	
	$result=mysqli_query($conn,$strSql);
    while($row=mysqli_fetch_array($result)) //通过循环读取数据内容
    {
?>
	  <tr>  
		<td align="center"><?php echo $row["role_id"]?></td>
		<td align="center"><?php echo $row["role_level"]?></td>
		<td align="center"><?php echo $currency_name_list[$row["type"]]?></td>
		<td align="center"><?php echo $row["value"]?></td>
		<td align="center"><?php echo $row["cur_value"]?></td>
		<td align="center"><?php echo $currency_from_des[$row["from_id"]]?></td>
		<td align="center"><?php echo $row["time"]?></td>
	  </tr>
<?php
    }
    //关闭对数据库的连接
    mysqli_close($conn);
?>
</table>
</BODY>
</HTML>