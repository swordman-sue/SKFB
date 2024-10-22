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
-->
</style>
<title>查询角色信息</title>
</HEAD>
<body alink="#FF0000" link="#000099" vlink="#CC6600" topmargin="8" leftmargin="0" bgColor="#FFFFFF">	
	<br><br><center><font color=green size=3><b>查询角色信息</b></font></center><br>
	<form action=$tool_url."/role_list.php" method="get" name="frmAdduser">
	<table cellspacing=0 bordercolordark=#FFFFFF width="60%" bordercolorlight=#000000 border=1 align="center" cellpadding="2">
	  <tr bgcolor="#6b8ba8" style="color:FFFFFF">
		<td width="100%" align="center" valign="bottom" height="19" colspan="2">请仔细填写以下内容</td>
	  </tr>
	  <tr>
		<td width="30%" align="right" height="19">角色ID(填时,忽略其他条件)：</td>
		<td width="70%"><input type="text" name="role_id" size="10" maxlength="10"></td>
	  </tr>  
	  <tr>
		<td width="30%" align="right" height="19">角色名称(填时,忽略其他条件)：</td>
		<td width="70%"><input type="text" name="role_name" size="10" maxlength="10"></td>
	  </tr>
  	  <tr>
		<td width="30%" align="right" height="19">帐号(填时,忽略其他条件)：</td>
		<td width="70%"><input type="text" name="account" size="10" maxlength="10"></td>
	  </tr> 
	  <tr>
		<td width="30%" align="right" height="19">最小等级(不填时,默认1级)：</td>
		<td width="70%"><input type="text" name="min_level" size="10" maxlength="10"></td>
	  </tr> 
	  <tr>
		<td width="30%" align="right" height="19">最大等级(不填时,默认200级)：</td>
		<td width="70%"><input type="text" name="max_level" size="10" maxlength="10"></td>
	  </tr>
	  <tr>
		<td width="30%" align="right" height="19">服务器ID级(不填时,默认所有服务器)：</td>
		<td width="70%"><input type="text" name="server_id" size="10" maxlength="10"></td>
	  </tr>    
	  <tr>	
		<td width="100%" align="center" colspan="2"><input type="submit" value="查询"></td>
	  </tr>
	</table>
	</form>	
<?php	
	print("<br><center><B>填写查询条件说明:</B><BR><br>");
	print("1: 如果输入<B><角色ID></B>或者<B><角色名称></B>则忽略其他条件...<BR>\n");	
	print("2: 如果所有的条件都木有填写时,则默认查询所有角色列表<BR>\n");	
?>
</BODY>
</HTML>