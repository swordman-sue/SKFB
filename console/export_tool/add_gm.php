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
<?php
    //本例是用PHP4实现向一个mysql数据表添加记录，
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
-->
</style>
<title>添加GM</title>
</HEAD>
<body alink="#FF0000" link="#000099" vlink="#CC6600" topmargin="8" leftmargin="0" bgColor="#FFFFFF">
<?php
	$cmd_id = $_GET['cmd_id'];
    if($cmd_id < 1)
    {
?>
		<br><br><center><font color=green size=3><b>添加GM命令</b></font></center>
		<br>
		<form action="<?php echo $PHP_SELF; ?>" method="get" name="frmAdduser">
		<table cellspacing=0 bordercolordark=#FFFFFF width="60%" bordercolorlight=#000000 border=1 align="center" cellpadding="2">
		  <tr bgcolor="#6b8ba8" style="color:FFFFFF">
			<td width="100%" align="center" valign="bottom" height="19" colspan="2">请仔细填写以下内容</td>
		  </tr>
		  <tr>
			<td width="30%" align="right" height="19">cmd_id：</td>
			<td width="70%"><input type="text" name="cmd_id" size="10" maxlength="10"></td>
		  </tr> 
		  <tr>
			<td width="30%" align="right" height="19">参数1：</td>
			<td width="70%"><input type="text" name="param1" size="10" maxlength="10"></td>
		  </tr>
		  <tr>
			<td width="30%" align="right" height="19">参数2：</td>
			<td width="70%"><input type="text" name="param2" size="10" maxlength="10"></td>
		  </tr> 
		  <tr>
			<td width="30%" align="right" height="19">参数3：</td>
			<td width="70%"><input type="text" name="param3" size="10" maxlength="10"></td>
		  </tr>
		  <tr>
			<td width="30%" align="right" height="19">参数4：</td>
			<td width="70%"><input type="text" name="param4" size="10" maxlength="10"></td>
		  </tr>
		  <tr>
			<td width="30%" align="right" height="19">参数5：</td>
			<td width="70%"><input type="text" name="param5" size="10" maxlength="10"></td>
		  </tr>	  
		  <tr>	
			<td width="100%" align="center" colspan="2"><input type="submit" value="添加"> <input type="reset" value="全部重写"></td>
		  </tr>
		</table>
		</form>
<?php 
   }
   else
    {				
        //将用户填写信息添加到数据库
		$cmd_id = empty($_GET['cmd_id']) ? '0' : trim($_GET['cmd_id']);
		$param1 = empty($_GET['param1']) ? '0' : trim($_GET['param1']);
		$param2 = empty($_GET['param2']) ? '0' : trim($_GET['param2']);
		$param3 = empty($_GET['param3']) ? '0' : trim($_GET['param3']);
		$param4 = empty($_GET['param5']) ? '0' : trim($_GET['param5']);
		$param5 = empty($_GET['cmd_id']) ? '0' : trim($_GET['cmd_id']);
        $strSql = "insert into gm_event(cmd_id,param1,param2,param3,param4,param5) values('$cmd_id','$param1','$param2','$param3','$param4','$param5')";
        $result = mysqli_query($conn,$strSql) or die(mysql_error());
        //关闭对数据库的连接
        mysqli_close($conn);
        echo "<br><br><br><center>您已经成功添加命令！<br><br><br><br><a href=\"$tool_url./add_gm.php\">返回</a></center>";
    }  
	
	//echo "<br><center>gm_id说明: <BR>\n<br>";
	print("<br><center><B>CMD_ID说明:</B><BR><br>");
	print("1: 系统邮件(邮件ID).....<BR>\n");
	print("2: 奖励中心发奖励(服务器ID,奖励类型)<BR>\n");
	print("3: 重新加载配置文件(不需要参数)<BR>\n");
	print("4: 重新加载服务器列表(不需要参数)<BR>\n");
	print("9: 踢人下线(目标的角色ID)<BR>\n");
	print("10: 禁言设置(1.目标的角色ID,2.禁言时间[分],3.是否禁言[1是0否])<BR>\n");
	print("11: 踢所有人下线(不需要参数)<BR>\n");
	print("12: 删除排行榜用户数据(1.排行榜类型,2.服务器ID,3.目标角色ID<BR>\n");
	print("13: 排行榜重新排行(1.排行榜类型,2.服务器ID,3.目标角色ID<BR>\n");
	
	print("1.战力 2.等级 3.爬塔 4.竞技场 5.叛军伤害排行榜 6.叛军功勋排行榜 7.世界BOSS荣誉排行榜 8.世界BOSS伤害排行榜<BR>\n");
	print("9.仙灵图鉴值排行 10.积分赛 11.主线副本星数排行榜 12.本服PVP排名 13.全服PVP排名 14.攻城排名 15.无尽之地杀敌排行<BR>\n");	
	
?>
</BODY>
</HTML>