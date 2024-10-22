<?php

namespace Qcloud\Sms\Demo;

//require '../include/common.php';

require_once "SmsSender.php";
use Qcloud\Sms\SmsSingleSender;
use Qcloud\Sms\SmsMultiSender;

try {
    // 请根据实际 appid 和 appkey 进行开发，以下只作为演示 sdk 使用
    //$appid = 1400028903;
    //$appkey = "ca6a1c9327c53603a50fc056c15c4c18";
    //$phoneNumber1 = "13632772857";
    //$phoneNumber2 = "18664662008";    
    //$phoneNumber3 = "13434337746";
    //$templId = 16247;
	
	 $appid = $_GET['appid'];
     $appkey = $_GET['appkey'];
	 $templId = $_GET['templId'];
	 $random = $_GET['random'];
	 $phoneNumber = $_GET['phoneNumber'];
	 $ext = $_GET['ext']; 

    $singleSender = new SmsSingleSender($appid, $appkey);

     //$strSql = "insert into tmp_role_list(role_id,role_name) values('$templId','templId')";
     //$sql_result = mysqli_query($conn,$strSql) or die(mysql_error());	
	
	
    // 普通单发
  //  $result = $singleSender->send(0, "86", $phoneNumber2, "测试短信，普通单发，深圳，小明，上学。", "", "");
 //   $rsp = json_decode($result);
 //   echo $result;
//    echo "<br>";

    // 指定模板单发
    // 假设模板内容为：测试短信，{1}，{2}，{3}，上学。
    $params = array($random, "10");
    $result = $singleSender->sendWithParam("86", $phoneNumber, $templId, $params, "", "", $ext);
    $rsp = json_decode($result);
    echo $result;
    echo "<br>";

   // $multiSender = new SmsMultiSender($appid, $appkey);

    // 普通群发
   // $phoneNumbers = array($phoneNumber1, $phoneNumber2, $phoneNumber3);
   // $result = $multiSender->send(0, "86", $phoneNumbers, "测试短信，普通群发，深圳，小明，上学。", "", "");
  // $rsp = json_decode($result);
  //  echo $result;
  //  echo "<br>";

    // 指定模板群发，模板参数沿用上文的模板 id 和 $params
  //  $params = array("指定模板群发", "深圳", "小明");
 //   $result = $multiSender->sendWithParam("86", $phoneNumbers, $templId, $params, "", "", "");
 //   $rsp = json_decode($result);
 //   echo $result;
 //   echo "<br>";
} catch (\Exception $e) {
    echo var_dump($e);
}