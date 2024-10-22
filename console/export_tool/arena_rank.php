<?php
	require 'include/common.php';
	require 'console_functions.php';
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
<title>竞技场排行榜</title>
</HEAD>
<body alink="#FF0000" link="#000099" vlink="#CC6600" topmargin="8" leftmargin="0" bgColor="#FFFFFF">
<br><br><center><font color=green size=3><b>竞技场排行榜</b></font></center>
<br>
	<form action="<?php echo $PHP_SELF; ?>" method="get" name="frmAdduser">
	<table cellspacing=0 bordercolordark=#FFFFFF width="50%" bordercolorlight=#000000 border=1 align="center" cellpadding="2">
	  <tr bgcolor="#6b8ba8" style="color:FFFFFF">
		<td width="100%" align="center" valign="bottom" height="19" colspan="2">查询条件</td>
	  </tr>
	  <tr>
		<td width="30%" align="right" height="19">服务器ID(默认1服)：</td>
		<td width="70%"><input type="text" name="server_id" size="10" maxlength="10"></td>	
	  </tr> 	
	  <tr>
		<td width="30%" align="right" height="19">前N名(默认前100名)：</td>
		<td width="70%"><input type="text" name="rank_max" size="10" maxlength="10"></td>		
	  </tr> 		  
	  <tr>	
		<td width="100%" align="center" colspan="2" ><input type="submit" value="查询"></td>
	  </tr> 
	</table>
	</form>

<table cellspacing=0 bordercolordark=#FFFFFF width="50%" bordercolorlight=#000000 border=1 align="center" cellpadding="2">
  <tr bgcolor="#6b8ba8" style="color:FFFFFF">
	<td width="2%" align="center" valign="bottom">角色ID</td>
    <td width="2%" align="center" valign="bottom">角色等级</td>
	<td width="2%" align="center" valign="bottom">角色名称</td>
	<td width="2%" align="center" valign="bottom">当前排名</td>
	<td width="2%" align="center" valign="bottom">历史排名</td>
  </tr>
<?php    
	// 取挑转过来的参数
	$server_id = empty($_GET['server_id']) ? '1' : trim($_GET['server_id']);
	$rank_max = empty($_GET['rank_max']) ? '100' : trim($_GET['rank_max']);	
    $select_sql = 'select role_id,cur_rank,history_rank from arena_for_view where server_id = '.$server_id.' ORDER BY cur_rank limit 0,'.$rank_max;	
	$result=mysqli_query($conn,$select_sql);
    while($row=mysqli_fetch_array($result)) //通过循环读取数据内容
    {
		$role_id = $row["role_id"];
		$role_info = get_role_info($conn,$role_id);
		$role_level = $role_info[1] ? $role_info[1] : 1;
		$role_name = $role_info[0] ? $role_info[0] : "机器人";
?>
	  <tr> 	  
		<td align="center"><?php echo $row["role_id"]?></td>
		<td align="center"><?php echo $role_level?></td>
		<td align="center"><?php echo $role_name?></td>
		<td align="center"><?php echo $row["cur_rank"]?></td>
		<td align="center"><?php echo $row["history_rank"]?></td>
	  </tr>
<?php
    }
    //关闭对数据库的连接
    mysqli_close($conn);
?>
</table>
</BODY>
</HTML>