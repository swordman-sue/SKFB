<?php
	require 'include/common.php';
	//$v = empty($_GET['v']) ? '' : trim($_GET['v']);
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link href="style/css/style.css" rel="stylesheet" type="text/css" />
<title>客户端资源更新</title>
</head>
<body>
<div id="main">
	<div class="div">
		<form method="post" style="display:block">
			<font color="red" size='6'><strong>资源更新流程</strong></font>
			<br />
		</form>		
		<form method="post" action="client_resource_udpate_file_1.php" style="display:block">
			<font color="blue"><strong>资源更新(内网):</strong></font>
			<input type="submit" value="资源更新" id="client_resource_udpate_file_1"/>
			<br />
		</form>	
		<form method="post" action="client_resource_udpate_file_2.php" style="display:block">
			<font color="blue"><strong>资源更新(外网):</strong></font>
			<input type="submit" value="资源更新" id="client_resource_udpate_file_2"/>
			<br />
		</form>	
	</div>
    <div class="my_btn" id="my_id">此处
      <form name="form1" method="post" action="">
        <label>my_btn
          <input type="submit" name="Submit" value="提交">
        </label>
      </form>
  显示  class "my_btn" id "my_id" 的内容</div>
</div>
<!-- main end-->
</body>
</html>