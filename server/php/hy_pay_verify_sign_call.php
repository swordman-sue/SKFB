<?php

namespace Qcloud\Sms\Demo;

require_once "hy_pay_verify_sign.php";
use Qcloud\Sms\Pay;

try {
	
    $pay = new Pay();
	
	$array_data = $_POST;

    // 指定模板单发
    $result = $pay->pay_callback($array_data);
	
    //$rsp = json_decode($result);
    echo $result;
    //echo "<br>";
	
} catch (\Exception $e) {
    echo var_dump($e);
}