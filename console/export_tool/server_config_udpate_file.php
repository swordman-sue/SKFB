<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<?php
	require 'include/common.php';
	//$v = empty($_GET['v']) ? '' : trim($_GET['v']);
?>

<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>服务器配置更新!</title>
</head>
<body>
<?php
	require 'config.php';

	print("开始更新配置....<BR>\n");

	exec(ROOT_PATH.'/export_tool/bat/server_config_update.bat');	
	
	// 插入
	$sql = "insert into gm_event (cmd_id) values ('3')";
	mysqli_query($conn, $sql);
	mysqli_close($conn); //关闭MySQL连接
	print("更新资源配置完成!");
?>
<div id="commit">
	<div class="div">
		<form method="post" action="client_res_update.php" style="display:block">
			<input type="submit" value="点击返回" id="main"/>
			<br />
		</form>	
	</div>
</div>

</body>
</html>


