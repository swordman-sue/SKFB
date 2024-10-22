<?php

namespace vivo\sdk\check_user_info;

//require_once "SmsSender.php";

require '..\common\common.php';

use game_php_util\common\HttpsUtil;

try {	
	$authtoken = $_GET['authtoken'];

	$url = 'https://usrsys.vivo.com.cn/sdk/user/auth.do';
	// 按照协议组织 post 包体
	$data = new \stdClass();
	$data->authtoken = $authtoken;	
	$https = new HttpsUtil();
	$result = $https->sendCurlPost($url, $data);
    echo $result;
    echo "<br>";	

} catch (\Exception $e) {
    echo var_dump($e);
}