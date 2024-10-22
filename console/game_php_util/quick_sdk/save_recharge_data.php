<?php

/**
 * QuickSDK 游戏同步加解密算法描述
 * @copyright quicksdk 2015
 * @author quicksdk 
 * @version quicksdk v 0.0.1 2014/9/2
 */

echo '<br>';

require('quicksdkAsy.php');
//require '../include/common.php';


$mysql_server_name='172.18.120.83:3306'; //改成自己的mysql数据库服务器
$mysql_username='root'; //改成自己的mysql数据库用户名
$mysql_password='ZLdebug@20160602'; //改成自己的mysql数据库密码
$mysql_database='skfb_hf'; //改成自己的mysql数据库名
$conn = mysqli_connect($mysql_server_name,$mysql_username,$mysql_password,$mysql_database) or die("error connecting") ; //连接数据库

//设置测试值
//$_POST['nt_data'] = '@108@119@174@165@158@82@167@152@171@166@161@162@159@115@90@106@94@103@83@85@154@166@156@161@155@160@166@155@118@90@139@141@118@101@110@90@82@165@165@148@167@151@153@159@160@164@157@118@82@165@160@87@116@118@117@165@162@176@165@163@168@166@169@152@157@157@169@171@147@153@150@113@117@160@157@166@164@151@159@158@110@115@166@158@153@118@109@110@102@172@161@152@119@116@162@168@151@161@164@151@160@147@158@152@119@164@169@164@162@167@169@117@95@163@160@156@158@166@152@160@152@164@157@114@117@167@171@173@143@167@168@156@151@164@144@161@168@113@116@98@160@171@172@152@159@169@149@154@167@151@167@161@117@115@167@166@157@157@168@152@158@167@116@104@99@98@97@101@105@100@108@100@98@102@109@106@101@106@100@101@102@105@106@100@111@103@107@103@117@103@165@171@148@157@168@151@160@161@111@111@169@148@177@146@165@159@165@158@110@105@97@102@105@101@106@99@100@103@109@84@106@109@112@108@99@114@103@108@110@97@161@148@178@146@172@156@158@155@118@117@145@164@160@170@163@172@119@98@101@103@105@112@104@153@163@168@165@166@170@118@110@165@165@148@173@168@171@113@97@114@103@172@164@152@165@170@168@118@117@151@175@171@170@149@172@151@166@154@162@153@163@171@112@170@169@171@117@98@157@171@165@168@153@172@143@167@146@167@150@165@172@112@115@102@165@153@172@171@151@160@149@118@114@103@165@157@170@160@168@162@166@166@144@163@157@172@163@152@152@154@115';
//$_POST['sign'] = '@104@155@109@158@105@104@104@106@111@103@112@103@148@152@104@158@149@111@101@151@102@111@156@149@155@112@113@105@154@110@108@106';
//$_POST['md5Sign'] = '07dae8d71374651c1f0ee4cc16a37f6e';
//$callback_key = '08682213938316890715589277849869';

$is_android = $_GET['is_android'];

// new code
//$callback_key = '90782163159998717041368980180119';
//$md5_key = 'bunr4tme1jc1zudk0iwllpgeqr6rbhpn';

// android
if ($is_android == 1 ){
	$callback_key = '90782163159998717041368980180119';
	$md5_key = 'bunr4tme1jc1zudk0iwllpgeqr6rbhpn';
}
// ios
else{
	$callback_key = '84076932623186346638334197291309';
	$md5_key = 'oaht53kkk7ucnidst1melppfyh9h1dfq';
}

$_POST['nt_data'] = $_GET['nt_data'];
$_POST['sign'] = $_GET['sign'];
$_POST['md5Sign'] = $_GET['md5Sign'];

//计算出的签名只
$thisSign = quickAsy::getSign($_POST,$md5_key);

//核对签名
if($thisSign !== $_POST['md5Sign']){
	exit('sign error');
}

//解密xml
$asyContent = quickAsy::decode($_POST['nt_data'],$callback_key);

//解析xml
$xmlObject = simplexml_load_string($asyContent);
$message = $xmlObject->message;

$uid = $message->uid;
//$loginName = $message->login_name;
//$status = $message->status;

$is_test = $message->is_test;
$channel = $message->channel;
$channel_uid = $message->channel_uid;
$game_order = $message->game_order;
$order_no = $message->order_no;
$pay_time = $message->pay_time;
$amount = $message->amount;
$status = $message->status;
$extras_params = $message->extras_params;

$extras_params_value = json_decode($extras_params);

//var_dump('role_id='.$extras_params_value->role_id);
//var_dump('goods_id='.$extras_params_value->goods_id);
//var_dump('recharge_type='.$extras_params_value->recharge_type);

$role_id = $extras_params_value->role_id;
$recharge_id = $extras_params_value->goods_id;
$recharge_type = $extras_params_value->recharge_type;

//$d=mktime(9, 12, 31, 6, 10, 2015);
date_default_timezone_set("PRC");
$record_time = date("Y-m-d G:i:s");

// 存储数据库
$strSql = "insert into quick_sdk_recharge_log(role_id,recharge_id,recharge_type,is_test,channel,channel_uid,game_order,order_no,pay_time,amount,status,extras_params,record_time) 
values('$role_id','$recharge_id','$recharge_type','$is_test','$channel','$channel_uid','$game_order','$order_no','$pay_time','$amount','$status','$extras_params','$record_time')";
$sql_result = mysqli_query($conn,$strSql) or die(mysql_error());	


// echo $uid.'-'.$loginName;
echo 'save success '.$record_time;

?>