<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">


<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>客户端脚本压缩&版本生成(skfb测试服)!</title>
</head>
<body>
<?php	
	require 'config.php';

	set_time_limit(0);
	
	print("开始脚本压缩&版本生成(skfb测试服)....<BR>\n");

	exec(ROOT_PATH.'/export_tool/bat/client_res_update.bat', $output);	
	foreach ($output as $key=>$value)  
	{  
		echo $value.'<BR>';  
	} 	
		
	print("<BR>\n脚本压缩&版本生成结束(skfb测试服)!");	
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


