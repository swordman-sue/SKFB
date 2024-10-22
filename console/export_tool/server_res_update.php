<?php
	require 'include/common.php';
	//$v = empty($_GET['v']) ? '' : trim($_GET['v']);
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link href="style/css/style.css" rel="stylesheet" type="text/css" />
<title>服务端配置更新(skfb测试服)</title>
</head>
<body>
<div id="main">
	<div class="div">
		<form method="post" style="display:block">
			<br />
			<font color="red" size='6'><strong>配置更新</strong></font>
			<br />
			<br />
		</form>		
		<form method="post" action="server_config_udpate_file.php" style="display:block">
			<font color="blue"><strong>服务器配置更新:</strong></font>
			<input type="submit" value="配置更新" id="server_config_udpate_file"/>
			<br />
		</form>		
	</div>
</div>
<!-- main end-->
</body>
</html>