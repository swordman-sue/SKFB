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
echo date("Y-m-d  G:---",$row["open_time"])
-->
</style>
<title>开启服务器</title>
</HEAD>
<body alink="#FF0000" link="#000099" vlink="#CC6600" topmargin="8" leftmargin="0" bgColor="#FFFFFF">
<?php	
	$server_id = $_GET['server_id'];
    if($server_id < 1)
    {		
		$cur_time = date("Y-m-d H:i");
		print("当前系统时间: $cur_time<BR>\n");	
?>
		<br><br><center><font color=green size=3><b>开启服务器</b></font></center>
		<br>
		<form action="<?php echo $PHP_SELF; ?>" method="get" name="frmAdduser">
		<table cellspacing=0 bordercolordark=#FFFFFF width="40%" bordercolorlight=#000000 border=1 align="center" cellpadding="2">
		  <tr bgcolor="#6b8ba8" style="color:FFFFFF">
			<td width="100%" align="center" valign="bottom" height="19" colspan="2">请仔细填写以下内容</td>
		  </tr>
		  <tr>
			<td width="30%" align="right" height="19">服务器ID</td>
			<td width="70%"><input type="text" name="server_id" size="10" maxlength="10"></td>
		  </tr> 
		  <tr>
			<td width="30%" align="right" height="19">开服时间</td>
			<td width="70%"><input type="datetime-local" name="open_time" size="10" maxlength="10"></td>
		  </tr> 		  
		  <tr>	
			<td width="100%" align="center" colspan="2"><input type="submit" value="开启"></td>
		  </tr>
		</table>
		
		<br><br><center><font color=green size=3><b>已经在运营的服务器列表</b></font></center>
		<table cellspacing=0 bordercolordark=#FFFFFF width="40%" bordercolorlight=#000000 border=1 align="center" cellpadding="2">		
		  <tr bgcolor="#6b8ba8" style="color:FFFFFF">
			<td width="2%" align="center" valign="bottom">服务器ID</td>
			<td width="2%" align="center" valign="bottom">开服时间</td>
		  </tr>	
		  <?php
			$select_Sql = "select * from server_list";
			$select_result=mysqli_query($conn, $select_Sql);
			while($row=mysqli_fetch_array($select_result))//通过循环读取数据内容
			{
		   ?>
			  <tr>  
				<td align="center"><?php echo $row["server_id"]?></td>
				<td align="center"><?php echo date("Y-m-d  H:i",$row["open_time"])?></td>
			  </tr>	
			<?php
			}				
			?>	
		</table>		
		</form>
<?php 
   }
   else
    {						
		$server_id = empty($_GET['server_id']) ? '0' : trim($_GET['server_id']);
			
		// 检测是否已经开启过了
		$select_Sql = "select * from server_list";
		$select_result=mysqli_query($conn, $select_Sql);
		while($row=mysqli_fetch_array($select_result))//通过循环读取数据内容
		{
			if($server_id == $row["server_id"])
			{
				//print("你傻了吧,此服务器已开过啦!!!!!!<BR>\n");
				echo "<br><br><br><center><b>你傻了吧,此服务器已开过啦!!!!!!</b><br><br><br><br><a href=\"$tool_url./open_server.php\">返回</a></center>";
				return;	
			}
		}
	
		// 插入新开的服务器
		$open_time =  date("U");
        $strSql = "insert into server_list(server_id,open_time) values('$server_id',$open_time)";
        $result = mysqli_query($conn,$strSql) or die(mysql_error());
		
		// 重新加载服务器列表
		$strSql = "insert into gm_event(cmd_id) values(4)";
        $result = mysqli_query($conn,$strSql) or die(mysql_error());
		
        //关闭对数据库的连接
        mysqli_close($conn);
        echo "<br><br><br><center><b>您已经成功开服[server_id:$server_id]</b><br><br><br><br><a href=\"$tool_url./open_server.php\">返回</a></center>";
    }  
	
	//echo "<br><center>GM命令说明: <BR>\n<br>";
	print("<br><center><B>开服操作说明:</B><BR><br>");
	print("1: 输入要开启的服务器ID........<BR>\n");
?>
</BODY>
</HTML>