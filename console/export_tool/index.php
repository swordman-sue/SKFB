<?php
	require 'include/common.php';
	//$v = empty($_GET['v']) ? '' : trim($_GET['v']);
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link href="style/css/style.css" rel="stylesheet" type="text/css" />
<title>转换工具(skfb测试服)</title>
</head>
<body>
<div id="main">
	<div class="div">
		<form method="post" style="display:block">
			<font color="red" size='5'><strong>生成配置文件流程: 更新文件--->生成--->提交文件(skfb测试服)</strong></font>
			<br />
		</form>		
		<form method="post" action="svn_update_file.php" style="display:block">
			<font color="blue"><strong>更新文件</strong></font>
			<input type="submit" value="更新文件" id="svn_update_file"/>
			<br />
		</form>	
		<form method="post" action="svn_commit_file.php" style="display:block">
			<font color="blue"><strong>提交文件</strong></font>
			<input type="submit" value="提交文件" id="svn_commit_file"/>
			<br />
		</form>		
		<form method="post" action="main_auto.php" style="display:block">
			<font color="blue"><strong>自动生成</strong></font>
			<input type="submit" value="一键生成" id="autoall"/>
			<br />
		</form>
		<?php foreach($_cfg as $key => $val):?>
			<form method="post" action="main_default_new.php?ac=<?php echo $val;?>">
				<font color="red"><?php echo $val;?> 配置</font>
				<input type="submit" value="生成"/>
			</form>
		<?php endforeach;?>
	</div>
</div>
<!-- main end-->
</body>
</html>