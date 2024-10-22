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
<title>查询角色信息 - 读取mysql的测试</title>
</HEAD>
<body alink="#FF0000" link="#000099" vlink="#CC6600" topmargin="8" leftmargin="0" bgColor="#FFFFFF">
<br><br><center><font color=green size=3><b>角色列表</b></font></center>
<br>
<table cellspacing=0 bordercolordark=#FFFFFF width="70%" bordercolorlight=#000000 border=1 align="center" cellpadding="2">
  <tr bgcolor="#6b8ba8" style="color:FFFFFF">
   <!-- <td width="5%" align="center" valign="bottom" height="19">ＩＤ</td>-->
	<td width="2%" align="center" valign="bottom">ID</td>
    <td width="2%" align="center" valign="bottom">姓名</td>
    <td width="2%" align="center" valign="bottom">年龄</td>
  </tr>
<?php
	$mysql_server_name='192.168.1.111:3307'; //改成自己的mysql数据库服务器
	$mysql_username='root'; //改成自己的mysql数据库用户名
	$mysql_password='debug'; //改成自己的mysql数据库密码
	$mysql_database='test'; //改成自己的mysql数据库名
	
	//$conn=mysql_connect($mysql_server_name,$mysql_username,$mysql_password) or die("error connecting") ; //连接数据库
	$conn=mysqli_connect($mysql_server_name,$mysql_username,$mysql_password,$mysql_database);
	//mysql_query("set names 'utf8'"); //数据库输出编码 应该与你的数据库编码保持一致
	//mysql_select_db($mysql_database); //打开数据库

    //连接到本地mysql数据库
    //$myconn=mysql_connect("192.168.1.111","root","");
    //选择test为操作库
   // mysql_select_db("test",$conn);
    $strSql="select * from student";
    //用mysql_query函数从user表里读取数据
	$result=mysqli_query($conn,$strSql);
    //$result=mysqli_query($strSql,$conn);
    while($row=mysqli_fetch_array($result))//通过循环读取数据内容
    {
?>
  <tr>  
    <td align="center"><?php echo $row["role_id"]?></td>
    <td align="center"><?php echo $row["name"]?></td>
    <td align="center"><?php echo $row["age"]?></td>
  </tr>
<?php
    }
    //关闭对数据库的连接
    mysqli_close($conn);
?>
</table>
</BODY>
</HTML>