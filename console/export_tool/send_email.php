<?php
	require 'include/common.php';
	//$v = empty($_GET['v']) ? '' : trim($_GET['v']);
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<meta charset="utf-8">
<title>菜鸟教程(runoob.com)</title>
</head>
<body>

<?php

//$to = "chenyangpin@126.com";         // 邮件接收者
//$subject = "参数邮件";                // 邮件标题
//$message = "Hello! 这是邮件的内容。";  // 邮件正文
//$from = "chenyangpin@126.com";   // 邮件发送者
//$headers = "From:" . $from;         // 头部信息设置
//mail($to, $subject, $message, $headers);
//echo "邮件已发送";

$to = "chenyangpin@126.com";//收件人 
$subject = "Test";//邮件主题 
$message = "This is a test mail!";//邮件正文 
ini_set('SMTP','smtp.126.com');//发件SMTP服务器 
ini_set('smtp_port',25);//发件SMTP服务器端口 
ini_set('sendmail_from',"chenyangpin@126.com");//发件人邮箱 
mail($to,$subject,$message);
echo "邮件已发送";

//if (isset($_REQUEST['email'])) { // 如果接收到邮箱参数则发送邮件
	// 发送邮件
	//$email = 'chenyangpin@126'; //$_REQUEST['email'] ;
	//$subject = 'test'; //$_REQUEST['subject'] ;
	//$message = 'Hello'; // $_REQUEST['message'] ;
	//mail("someone@example.com", $subject, $message, "From:" . $email);
	//echo "邮件发送成功";
//} else { // 如果没有邮箱参数则显示表单
//	echo "<form method='post' action='mailform.php'>
//	Email: <input name='email' type='text'><br>
//	Subject: <input name='subject' type='text'><br>
//	Message:<br>
//	<textarea name='message' rows='15' cols='40'>
//	</textarea><br>
//	<input type='submit'>
//	</form>";
//}
?>
</body>
</html>