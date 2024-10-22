<?php
	require 'include/common.php';
	//$v = empty($_GET['v']) ? '' : trim($_GET['v']);
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link href="style/css/style.css" rel="stylesheet" type="text/css" />
<title>客户端资源更新(skfb测试服)</title>
</head>
<body>
<div id="main">
	<div class="div">
		<form method="post" style="display:block">
			<br />
			<font color="red" size='6'><strong>资源更新</strong></font>
			<br />
			<br />
		</form>		
		<form method="post" action="client_resource_udpate.php" style="display:block">
			<font color="blue"><strong>客户端脚本压缩&版本生成:</strong></font>
			<input type="submit" value="执行" id="client_resource_udpate"/>
			<br />
		</form>		
		<form method="post" action="client_resource_sync.php" style="display:block">
			<font color="red"><strong>客户端资源同步:</strong></font>
			<input type="submit" value="执行" id="client_resource_sync"/>
			<br />
		</form>				

		<form method="post" style="display:block">
			<br />
			<br />
			<font color="red" size='6'><strong>资源打包</strong></font>
			<br />
			<br />
		</form>		
		<form method="post" action="client_resource_package.php" style="display:block">
			<font color="blue"><strong>客户端资源打包:</strong></font>
			<input type="submit" value="执行" id="client_resource_package"/>
			<br />
		</form>				
	</div>
</div>
<!-- main end-->
</body>
</html>