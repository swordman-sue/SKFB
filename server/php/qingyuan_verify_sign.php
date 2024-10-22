<?php

	// 
	$public = "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDvpBDYeWK1VGZCwETz5MHSzmZag8zgDDH89JqgJ1EUDThBrHYlbksR3ACORfvDh6J+jSKWoXIQJcS12ElyqzwUaTpTthnUh0jU8mJgOzJuTwtk2xZKNEk71M098h4tmriJCPMWEHMn+0U/qCoHSdzbftNwXTpjRrf6L4PyFAeePQIDAQAB";
	
	// 取传过来的参数
	$data = $_POST;
	
	// 取出并解码 sign
	$sign = base64_decode($data['sign']);
	
	// 删除sign
	unset($data['sign']);
	
	// 排序
	ksort($data);
	
	// 拼接待签名字符串
	$str = ''; 
	foreach ($data as $k => $v) {
		$str .= '&'.$k.'='.$v; 
	}
	
	// 函数移除字符串左侧的空白字符或其他预定义字符
	$str = ltrim($str, '&'); 
	
	$pem = chunk_split($public, 64, "\n");
	$pem = "-----BEGIN PUBLIC KEY-----\n" . $pem . "-----END PUBLIC KEY-----\n";		
	
	// 调用验签方法
	$key = openssl_pkey_get_public($pem); 
	$result = (bool)openssl_verify($str, $sign, $key, OPENSSL_ALGO_SHA1); 
	
	openssl_free_key($key);
	
	// 验签成功
	if ($result){
		echo 1;	
	// 验签失败
	} else{	
		echo 0;
	}