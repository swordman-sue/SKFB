<?php

/**
 * QuickSDK游戏同步加解密算法描述
 * @copyright quicksdk 2015
 * @author quicksdk 
 * @version quicksdk v 0.0.1 2014/9/2
 */

class quickAsy{

	/**
	 * 解密方法
	 * $strEncode 密文
	 * $keys 解密密钥 为游戏接入时分配的 callback_key
	 */
	public static function decode($strEncode, $keys) {
		if(empty($strEncode)){
			return $strEncode;
		}
		preg_match_all('(\d+)', $strEncode, $list);
		$list = $list[0];
		if (count($list) > 0) {
			$keys = self::getBytes($keys);
			for ($i = 0; $i < count($list); $i++) {
				$keyVar = $keys[$i % count($keys)];
				$data[$i] =  $list[$i] - (0xff & $keyVar);
			}
			return self::toStr($data);
		} else {
			return $strEncode;
		}
	}
	
	/**
	 * 计算游戏同步签名
	 */
	public static function getSign($params,$callbackkey){

		return md5($params['nt_data'].$params['sign'].$callbackkey);
	}
	
	/**
	 * MD5签名替换
	 */
	static private function replaceMD5($md5){
	
		strtolower($md5);
		$bytes = self::getBytes($md5);

		$len = count($bytes);
		
	 	if ($len >= 23){
			$change = $bytes[1];
           	$bytes[1] = $bytes[13];
            $bytes[13] = $change;

            $change2 = $bytes[5];
            $bytes[5] = $bytes[17];
            $bytes[17] = $change2;

            $change3 = $bytes[7];
            $bytes[7] = $bytes[23];
            $bytes[23] = $change3;
       }else{
			return $md5;
       }
       
       return self::toStr($bytes);
	}
	
	/**
	 * 转成字符数据
	 */
	private static function getBytes($string) {  
        $bytes = array();  
        for($i = 0; $i < strlen($string); $i++){  
             $bytes[] = ord($string[$i]);  
        }  
        return $bytes;  
    }  
    
    /**
     * 转化字符串
     */
    private static function toStr($bytes) {  
        $str = '';  
        foreach($bytes as $ch) {  
            $str .= chr($ch);  
        }  
   		return $str;  
    }
}

?>