<?php

namespace Qcloud\Sms\Demo;

require_once "SmsSender.php";


use Qcloud\Sms\SmsSingleSender;
use Qcloud\Sms\SmsMultiSender;
use Qcloud\Sms\SmsSenderUtil;

try {
    //$singleSender = new SmsSingleSender($appid, $appkey);
    // 指定模板单发
    // 假设模板内容为：测试短信，{1}，{2}，{3}，上学。
    //$params = array($random, "10");
    //$result = $singleSender->sendWithParam("86", $phoneNumber, $templId, $params, "", "", $ext);
    //$rsp = json_decode($result);
    //echo 'Hello,Kevin';
    //echo "<br>";
	
	$authtoken = $_GET['authtoken'];

	$wholeUrl = 'https://usrsys.vivo.com.cn/sdk/user/auth.do';
	// 按照协议组织 post 包体
	$data = new \stdClass();
	$data->authtoken = $authtoken;	
	$my_sender = new SmsSenderUtil();
	$result = $my_sender->sendCurlPost($wholeUrl, $data);
    echo $result;
    echo "<br>";	

} catch (\Exception $e) {
    echo var_dump($e);
}