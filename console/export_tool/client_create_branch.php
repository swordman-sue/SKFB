<?php
	require 'include/common.php';
	//$v = empty($_GET['v']) ? '' : trim($_GET['v']);
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link href="style/css/style.css" rel="stylesheet" type="text/css" />
<title>客户端打分支(skfb测试服)</title>
</head>
<body>
<div id="main">
	<div class="div">
		<form method="post" style="display:block">
			<br />
			<font color="red" size='6'><strong>客户端打分支</strong></font>
			<br />
			<br />
		</form>		
		<form method="post" action="client_create_branch_script.php" style="display:block">
			<font color="blue"><strong>打脚本分支</strong></font>
			<input type="submit" value="执行" id="client_create_branch_script"/>
			<br />
		</form>		
		<form method="post" action="client_create_branch_cdn.php" style="display:block">
			<font color="blue"><strong>打资源分支:</strong></font>
			<input type="submit" value="执行" id="client_create_branch_cdn"/>
			<br />
		</form>		
	</div>
</div>
<!-- main end-->
</body>
</html>