<?php
	require 'include/common.php';
	require 'console_functions.php';	
	//$v = empty($_GET['v']) ? '' : trim($_GET['v']);
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">

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
-->
</style>
<title>发放奖励</title>
</HEAD>
<body alink="#FF0000" link="#000099" vlink="#CC6600" topmargin="8" leftmargin="0" bgColor="#FFFFFF">
<?php
	$item_id = $_GET['item_id'];
	$item_num = $_GET['item_num'];
	$reward_id = $_GET['reward_id'];
    if($item_id < 1 || $item_num < 1 || $reward_id < 1)
    {
?>
		<br><br><center><font color=green size=3><b>发放奖励</b></font></center>
		<br>
		<form action="<?php echo $PHP_SELF; ?>" method="get" name="frmAdduser">
		<table cellspacing=0 bordercolordark=#FFFFFF width="60%" bordercolorlight=#000000 border=1 align="center" cellpadding="2">
		  <tr>
			<td width="30%" align="right" height="19">指定的玩家ID(0表示发放整个服务器玩家)：</td>
			<td width="70%"><input type="text" name="role_id" size="10" maxlength="10"></td>
		  </tr> 	
		  <tr>
			<td width="30%" align="right" height="19">指定的服务器(0表示所有的服务器)：</td>
			<td width="70%"><input type="text" name="server_id" size="10" maxlength="10"></td>
		  </tr> 		
		  <tr>
			<td width="30%" align="right" height="19">奖励ID(必填)：</td>
			<td width="70%"><input type="text" name="reward_id" size="10" maxlength="10"></td>
		  </tr> 
		  <tr>
			<td width="30%" align="right" height="19">奖励物品ID(必填)：</td>
			<td width="70%"><input type="text" name="item_id" size="10" maxlength="10"></td>
		  </tr> 
		  <tr>
			<td width="30%" align="right" height="19">奖励物品数量(必填)：</td>
			<td width="70%"><input type="text" name="item_num" size="10" maxlength="10"></td>
		  </tr> 		  
		  <tr>
			<td width="30%" align="right" height="19">奖励描述(暂时不起作用)：</td>
			<td width="70%"><input type="text" name="desc" size="100" maxlength="50"></td>
		  </tr>  
		  <tr>	
			<td width="100%" align="center" colspan="2"><input type="submit" value="发布"></td>
		  </tr>
		</table>
		</form>
<?php 
   }
   else
    {		
        //将用户填写信息添加到数据库
		$role_id = empty($_GET['role_id']) ? '0' : trim($_GET['role_id']);
		$server_id = empty($_GET['server_id']) ? '0' : trim($_GET['server_id']);
		$desc = empty($_GET['desc']) ? '0' : trim($_GET['desc']);
		$cmd_id = 8;
		if($role_id > 0)
		{
			$cmd_id = 7;
			$role_info = get_role_info($conn,$role_id);
			$server_id = $role_info[2];
		}
        $strSql = "insert into gm_event(cmd_id, param1, param2, param3, param4, param5) values('$cmd_id','$role_id','$server_id','$item_id','$item_num','$reward_id')";
		//$strSql = "insert into gm_event(cmd_id, param1) values('$cmd_id','$role_id')";
		printf($strSql);
        $result = mysqli_query($conn,$strSql) or die(mysql_error());
		//printf($strSql);
		//printf(mysql_error());
        //关闭对数据库的连接
        mysqli_close($conn);
        echo "<br><br><br><center>发放奖励成功！<br><br><br><br><a href=\"$tool_url./send_reward.php\">返回</a></center>";
    }  
?>
</BODY>
</HTML>