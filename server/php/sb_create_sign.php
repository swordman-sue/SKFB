<?php

namespace game_php_util;

//require_once "common.php";
require 'common\common.php';

use game_php_util\common\CommonUtil;

try {
	
    $CommonUtil = new CommonUtil();	
	$array_data = $_POST;
	$signKey = $array_data['signkey'];
	unset($array_data['signkey']); 
	print_r($array_data);
    // 创建签名
    $result = $CommonUtil->sb_create_sign($signKey, $array_data);
    echo $result;	
} catch (\Exception $e) {
    echo var_dump($e);
}