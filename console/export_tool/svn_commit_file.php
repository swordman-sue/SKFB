<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">


<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>提交文件页面!</title>
</head>
<body>
<?php
	require 'config.php';

	print("开始添加文件....<BR>\n");

	exec(ROOT_PATH.'/export_tool/bat/add.bat');	
	
	print("添加文件结束!<BR>\n");

	print("开始提交文件....<BR>\n");

	exec(ROOT_PATH.'/export_tool/bat/commit.bat');	
	
	print("提交文件结束!");
?>
<div id="commit">
	<div class="div">
		<form method="post" action="index.php" style="display:block">
			<input type="submit" value="点击返回" id="main"/>
			<br />
		</form>	
	</div>
</div>

</body>
</html>


