<?php
	require 'include/common.php';
	//$v = empty($_GET['v']) ? '' : trim($_GET['v']);
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<HTML>
<HEAD>
<META NAME="GENERATOR" Content="Microsoft Visual Studio 6.0">
<style type="text/css">
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
<br><br><center><font color=green size=3><b>待发奖励或者邮件用户列表</b></font></center>
<td width="100%" align="center" colspan="2"><input type="submit" value="发奖励"></td>
-->
</style>
<title>添加临时用户</title>
</HEAD>
<body alink="#FF0000" link="#000099" vlink="#CC6600" topmargin="8" leftmargin="0" bgColor="#FFFFFF">
<?php
	$url = $tool_url."/add_tmp_role_list.php";

	$referer_url = $_SERVER['HTTP_REFERER'];
	//print("pef:".$ref_page);	
	$pos = strpos($referer_url,"console.php"); 
	print("referer_url_pos:".$pos);	 
	
	$role_id = $_GET['role_id'];
    if($role_id < 1)
    {
?>
		<br><br><center><font color=green size=3><b>添加临时用户(发奖励或者邮件)</b></font></center><br>
		<form action="<?php echo $PHP_SELF; ?>" method="get" name="add_tmp_user">
			<table cellspacing=0 bordercolordark=#FFFFFF width="40%" bordercolorlight=#000000 border=1 align="center" cellpadding="2">
			  <tr bgcolor="#6b8ba8" style="color:FFFFFF">
				<td width="100%" align="center" valign="bottom" height="19" colspan="2">请仔细填写以下内容</td>
			  </tr>
			  <tr>	
				<td width="30%" align="right" height="19">奖励ID：</td>
				<td width="70%"><input type="text" name="reward_id" size="10" maxlength="10"><input type="submit" value="发奖励"></td>	
			  </tr>				  
			</table>
		</form>
		<form action="<?php echo $PHP_SELF; ?>" method="get" name="send_reward">	
			<table cellspacing=0 bordercolordark=#FFFFFF width="40%" bordercolorlight=#000000 border=1 align="center" cellpadding="2">	
			  <tr>
				<td width="30%" align="right" height="19">用户ID：</td>
				<td width="70%"><input type="text" name="role_id" size="10" maxlength="10"><input type="submit" name="add" value="添加"><input type="submit" name="clean" value="清空列表"></td>
			  </tr>			
			  <tr bgcolor="#6b8ba8" style="color:FFFFFF">
				<td width="2%" align="center" valign="bottom">角色ID</td>
				<td width="2%" align="center" valign="bottom">角色名称</td>
			  </tr>	
			  <?php
				$select_Sql = "select * from tmp_role_list";
				$select_result=mysqli_query($conn, $select_Sql);
				while($row=mysqli_fetch_array($select_result))//通过循环读取数据内容
				{
			   ?>
				  <tr>  
					<td align="center"><?php echo $row["role_id"]?></td>
					<td align="center"><?php echo $row["role_name"]?></td>
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
		// 检测是否重复添加
		$select_Sql = "select * from tmp_role_list";
		$select_result=mysqli_query($conn, $select_Sql);
		while($row=mysqli_fetch_array($select_result))//通过循环读取数据内容
		{
			if($role_id == $row["role_id"])
			{
				echo "<br><br><br><center><b>哥哥不要酱紫,已经添加过了!!!!!!</b><br><br><br><br><a href=$url>返回</a></center>";
				return;	
			}
		}
		
		$is_exist = false;
		$role_name = "unknow";
		// 检测是否存在此用户
		$select_Sql = "select role_name from role where role_id = $role_id";
		$select_result=mysqli_query($conn, $select_Sql);
		while($row=mysqli_fetch_array($select_result))//通过循环读取数据内容
		{
			$role_name = $row["role_name"];
			$is_exist = true;
		}
		
		if($is_exist == false)
		{
			echo "<br><br><br><center><b>我读书多,您可别骗我,此用户根据不存在!!!!!!</b><br><br><br><br><a href=$url>返回</a></center>";
			return;	
		}		
		
        $strSql = "insert into tmp_role_list(role_id, role_name) values('$role_id','$role_name')";
        $result = mysqli_query($conn,$strSql) or die(mysql_error());
        //关闭对数据库的连接
        mysqli_close($conn);
        echo "<br><br><br><center>添加用户成功！<br><br><br><br><a href=$url>返回</a></center>";
    }  
?>
</BODY>
</HTML>