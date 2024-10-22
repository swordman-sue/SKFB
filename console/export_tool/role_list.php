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
<title>角色列表</title>
</HEAD>
<body alink="#FF0000" link="#000099" vlink="#CC6600" topmargin="8" leftmargin="0" bgColor="#FFFFFF">
<br><br><center><font color=green size=3><b>时空英雄玩家信息列表</b></font></center>
<br>
<table cellspacing=0 bordercolordark=#FFFFFF width="90%" bordercolorlight=#000000 border=1 align="center" cellpadding="2">
  <tr bgcolor="#6b8ba8" style="color:FFFFFF">
	<td width="1%" align="center" valign="bottom">角色ID</td>
	<td width="2%" align="center" valign="bottom">角色名称</td>	
	<td width="2%" align="center" valign="bottom">帐号</td>
    <td width="1%" align="center" valign="bottom">角色等级</td>
	<td width="1%" align="center" valign="bottom">钻石</td>
	<td width="2%" align="center" valign="bottom">金币</td>
	<td width="1%" align="center" valign="bottom">VIP等级</td>
	<td width="2%" align="center" valign="bottom">当前主线副本章节</td>
	<td width="2%" align="center" valign="bottom">当前主线副本关卡</td>
	<td width="2%" align="center" valign="bottom">在线时间(秒)</td>
	<td width="1%" align="center" valign="bottom">物品数量</td>
	<td width="1%" align="center" valign="bottom">装备数量</td>
	<td width="1%" align="center" valign="bottom">宝物数量</td>
	<td width="1%" align="center" valign="bottom">英雄数量</td>
	<td width="1%" align="center" valign="bottom">战力</td>
	<td width="2%" align="center" valign="bottom">角色创建时间</td>
	<td width="2%" align="center" valign="bottom">最后更新数据时间</td>
  </tr>
<?php    
	// 取挑转过来的参数
	$min_level = empty($_GET['min_level']) ? '0' : trim($_GET['min_level']);
	$max_level = empty($_GET['max_level']) ? '200' : trim($_GET['max_level']);
	$server_id = empty($_GET['server_id']) ? '0' : trim($_GET['server_id']);
	$role_id = empty($_GET['role_id']) ? '0' : trim($_GET['role_id']);
	$role_name = empty($_GET['role_name']) ? '' : trim($_GET['role_name']);
	$account = empty($_GET['account']) ? '' : trim($_GET['account']);
		
	// 根据角色ID查询
	if($role_id > 0)
	{
		$sql_condiction = "role_id = $role_id";
	}
	// 根据角色名称查询
	else if (strlen($role_name) > 0)
	{
		$sql_condiction = "role_name = '$role_name'";
	}
	// 根据帐号查询
	else if (strlen($account) > 0)
	{
		$sql_condiction = "account = '$account'";
	}	
	// 有指定服务器
	else if ($server_id > 0)
	{
		$sql_condiction = "level >= $min_level and level <= $max_level and server_id = $server_id";
	}
	else
	{
		$sql_condiction = "level >= $min_level and level <= $max_level";
	}
	
	print("<br><center><B>查询条件:$sql_condiction</B><BR><br>");
	
    $strSql= "select role_id,role_name,account,level,diamond,gold,vip_level,fb_chapter,fb_map,total_online_time,item_num,
	equipment_num,treasure_num,hero_num,fighting_power,create_time,last_update_time from role where ".$sql_condiction;
	
	$result=mysqli_query($conn,$strSql);
    while($row=mysqli_fetch_array($result)) //通过循环读取数据内容
    {
?>
	  <tr>  
		<td align="center"><?php echo $row["role_id"]?></td>
		<td align="center"><?php echo $row["role_name"]?></td>
		<td align="center"><?php echo $row["account"]?></td>
		<td align="center"><?php echo $row["level"]?></td>
		<td align="center"><?php echo $row["diamond"]?></td>
		<td align="center"><?php echo $row["gold"]?></td>
		<td align="center"><?php echo $row["vip_level"]?></td>
		<td align="center"><?php echo $row["fb_chapter"]?></td>
		<td align="center"><?php echo $row["fb_map"]?></td>
		<td align="center"><?php echo $row["total_online_time"]?></td>	
		<td align="center"><?php echo $row["item_num"]?></td>
		<td align="center"><?php echo $row["equipment_num"]?></td>
		<td align="center"><?php echo $row["treasure_num"]?></td>
		<td align="center"><?php echo $row["hero_num"]?></td>
		<td align="center"><?php echo $row["fighting_power"]?></td>
		<td align="center"><?php echo $row["create_time"]?></td>
		<td align="center"><?php echo $row["last_update_time"]?></td>
	  </tr>
<?php
    }
    //关闭对数据库的连接
    mysqli_close($conn);
?>
</table>
</BODY>
</HTML>