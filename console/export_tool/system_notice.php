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
<title>发布公告</title>
</HEAD>
<body alink="#FF0000" link="#000099" vlink="#CC6600" topmargin="8" leftmargin="0" bgColor="#FFFFFF">
<?php
	$notice_id = $_GET['notice_id'];
	$notice_text = $_GET['notice_text'];
    if($notice_id < 1 && !strlen($notice_text))
    {
?>
		<br><br><center><font color=green size=3><b>发布公告</b></font></center>
		<br>
		<form action="<?php echo $PHP_SELF; ?>" method="get" name="frmAdduser">
		<table cellspacing=0 bordercolordark=#FFFFFF width="60%" bordercolorlight=#000000 border=1 align="center" cellpadding="2">
		  <tr>
			<td width="30%" align="right" height="19">指定的服务器(0表示所有的服务器)：</td>
			<td width="70%"><input type="text" name="server_id" size="10" maxlength="10"></td>
		  </tr> 		
		  <tr>
			<td width="30%" align="right" height="19">公告ID(ID优先)：</td>
			<td width="70%"><input type="text" name="notice_id" size="10" maxlength="10"></td>
		  </tr> 
		  <tr>
			<td width="30%" align="right" height="19">公告内容：</td>
			<td width="70%"><input type="text" name="notice_text" size="100" maxlength="50"></td>
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
		$cmd_id = 5;
		$server_id = empty($_GET['server_id']) ? '0' : trim($_GET['server_id']);
		$notice_id = empty($_GET['notice_id']) ? '0' : trim($_GET['notice_id']);
		$notice_text = empty($_GET['notice_text']) ? '0' : trim($_GET['notice_text']);
        $strSql = "insert into gm_event(cmd_id, param1, param2, text) values('$cmd_id','$server_id','$notice_id','$notice_text')";
        $result = mysqli_query($conn,$strSql) or die(mysql_error());
        //关闭对数据库的连接 $tool_url
        mysqli_close($conn);
		echo "<br><br><br><center>发布公告成功！<br><br><br><br><a href=\"$tool_url./system_notice.php\">返回</a></center>";
    }  
?>
</BODY>
</HTML>