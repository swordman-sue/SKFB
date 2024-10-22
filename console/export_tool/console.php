<?php
	require 'include/common.php';
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
<a href=\"http://182.254.186.181/skfb/console_develop/export_tool/role_list.php\">查询</a>
<td width="100%" align="center" colspan="2"><input type="submit" value="查询"></td>
<td width="100%" align="center" colspan="2"><?php echo "<a href=\"http://182.254.186.181/skfb/console_develop/export_tool/role_list.php\">查询</a>";?><input type="submit" value="查询"></td>
action="<?php echo $PHP_SELF; ?>" method="get" name="frmAdduser"
<?php echo "<a href=\"http://182.254.186.181/skfb/console_develop/export_tool/role_list.php\">查询</a>";?>
-->
</style>
<title>skfb测试服后台运营控制台</title>
</HEAD>
<body alink="#FF0000" link="#000099" vlink="#CC6600" topmargin="8" leftmargin="0" bgColor="#FFFFFF">	
	<br><br><center><font color=green size=3><b>skfb测试服后台运营控制台</b></font></center><br>
	<form action=$tool_url."/role_list.php" method="get" name="frmAdduser">
	<table cellspacing=0 bordercolordark=#FFFFFF width="40%" bordercolorlight=#000000 border=1 align="center" cellpadding="2">
	  <tr bgcolor="#6b8ba8" style="color:FFFFFF">
		<td width="100%" align="center" valign="bottom" height="19" colspan="2">功能入口</td>
	  </tr>
	  <tr>	
		<td width="30%" align="right" height="19">客户端资源更新</td>
		<td width="100%" align="center" colspan="2"><?php echo "<a href=\"client_res_update.php\">客户端资源更新</a>";?></td>
	  </tr> 
	  <tr>	
		<td width="30%" align="right" height="19">客户端打分支</td>
		<td width="100%" align="center" colspan="2"><?php echo "<a href=\"client_create_branch.php\">客户端打分支</a>";?></td>
	  </tr> 
	  <tr>	
		<td width="30%" align="right" height="19">服务器配置更新</td>
		<td width="100%" align="center" colspan="2"><?php echo "<a href=\"server_res_update.php\">服务器配置更新</a>";?></td>
	  </tr> 
	  <tr>	
		<td width="30%" align="right" height="19">GM命令操作</td>
		<td width="100%" align="center" colspan="2"><?php echo "<a href=\"add_gm.php\">GM命令操作</a>";?></td>
	  </tr> 
	  <tr>	
		<td width="30%" align="right" height="19">开服操作</td>
		<td width="100%" align="center" colspan="2"><?php echo "<a href=\"open_server.php\">开服操作</a>";?></td>
	  </tr> 
	  <tr>	
		<td width="30%" align="right" height="19">查询角色信息</td>
		<td width="100%" align="center" colspan="2"><?php echo "<a href=\"select_role.php\">查询角色信息</a>";?></td>
	  </tr>
	  <tr>	
		<td width="30%" align="right" height="19">发放奖励</td>
		<td width="100%" align="center" colspan="2"><?php echo "<a href=\"add_tmp_role_list.php\">发放奖励</a>";?></td>
	  </tr> 
	  <tr>	
		<td width="30%" align="right" height="19">发送邮件</td>
		<td width="100%" align="center" colspan="2"><?php echo "<a href=\"add_tmp_role_list.php\">发送邮件</a>";?></td>
	  </tr> 	
	  <tr>	
		<td width="30%" align="right" height="19">发布公告</td>
		<td width="100%" align="center" colspan="2"><?php echo "<a href=\"system_notice.php\">发布公告</a>";?></td>
	  </tr> 
	 <tr>	
		<td width="30%" align="right" height="19">角色货币详情日志</td>
		<td width="100%" align="center" colspan="2"><?php echo "<a href=\"role_currency_logs.php\">角色货币日志详情</a>";?></td>
	  </tr>  
	 <tr>	
		<td width="30%" align="right" height="19">竞技场排行榜</td>
		<td width="100%" align="center" colspan="2"><?php echo "<a href=\"arena_rank.php\">竞技场排行榜</a>";?></td>
	  </tr>  
	 <tr>	
		<td width="30%" align="right" height="19">查看玩家留言</td>
		<td width="100%" align="center" colspan="2"><?php echo "<a href=\"leave_message.php\">查看玩家留言</a>";?></td>
	  </tr> 
	 <tr>	
		<td width="30%" align="right" height="19">发放奖励(推荐)</td>
		<td width="100%" align="center" colspan="2"><?php echo "<a href=\"send_reward.php\">发放奖励</a>";?></td>
	  </tr>  
	 <tr>	
		<td width="30%" align="right" height="19">转表工具</td>
		<td width="100%" align="center" colspan="2"><?php echo "<a href=\"index.php\">转表工具</a>";?></td>
	  </tr>  	  
	</table>
	</form>	
</BODY>
</HTML>