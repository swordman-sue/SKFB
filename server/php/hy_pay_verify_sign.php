<?php

namespace Qcloud\Sms;

final class Pay {
	private static $APP_ID = 13101;
	private static $APP_KEY = "b58b8c406cbec209b9283d25fb924fa1";
	private static $PAY_KEY = "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCv8BIomX6jNeEXZFotHUVvSjvQSkliujuo86adSsPXvBt1G1sRc/8XiP7Gm2qmE0jPwk4mhUKjVcdS3XakebXq27c3GSr1l73vEH6H/dLVq8NC9GnHJCqfnXctsi0fSFqUdsZfc0LYvu3tpWox0Jb2hZ5P4l5a1IA69xCdfFqR4wIDAQAB";
	
	public function verify_sign($input, $APP_KEY, $PAY_KEY) {
		
		//print_r($input);
		
		$sign = $input['sign'];
		unset($input['sign']);
		ksort($input);
		$str = implode($input) . $APP_KEY;
		
		//echo "str===".$str;
		
		$pem = chunk_split($PAY_KEY, 64, "\n");
		$pem = "-----BEGIN PUBLIC KEY-----\n" . $pem . "-----END PUBLIC KEY-----\n";
		$key_id = openssl_pkey_get_public($pem);
		
		//echo "key_id===".$key_id;
		
		return openssl_verify($str, base64_decode($sign), $key_id);
	}
	public function pay_callback($array_data) {
		//$data = $_POST;
		//echo "array_data===".$array_data;
		$data = $array_data;
		
		if ($this->verify_sign($data, static::$APP_KEY, static::$PAY_KEY)) {
			//exit(json_encode(array('ret' => 0, 'msg' => "success")));
			echo 1;
		}
		else{
			echo 0;
		}
	}
}

