var https = require('https');
var http = require('http');
var path = require('path');
var fs = require('fs');
var util = require('util');
var dateformat = require('./utils/dateformat');
var common_dao = require('./dao/common_dao');
var common_logic = require('./common_logic');
var Md5 = require('./utils/Md5');
var md5=new Md5();
var config = require('./utils/config');

// var recharge_callback_url = config.recharge_callback_url;

var recharge_callback_android_url 	=  "http://172.18.120.87:40006/recharge_callback_quicksdk_android";
var recharge_callback_ios_url 		=  "http://172.18.120.87:40006/recharge_callback_quicksdk_ios";

// 乐游长尾充值回调 
exports.recharge_callback_cw_leyou = function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    console.log('recharge_callback_cw_leyou be call!(%s)',cur_time);

	var call_game_url = recharge_callback_android_url;
	var redirect_url = call_game_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign;	
	common_logic.redirect_to_game_server(redirect_url, res, 'SUCCESS');	

	// 存储充值发货通知数据
	//var call_php_url = 'http://192.168.1.111/game_php_util/quick_sdk/save_recharge_data.php';
	//var redirect_php_url = call_php_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign+'&is_android='+1;
	//save_recharge_log(redirect_php_url);	
}


// 汇娱长尾充值回调(ios)
exports.recharge_callback_cw_huiyu_ios = function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    console.log('recharge_callback_cw_huiyu_ios be call!(%s)',cur_time);

	
	var call_game_url = recharge_callback_ios_url;
	var redirect_url = call_game_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign;	
	common_logic.redirect_to_game_server(redirect_url, res, 'SUCCESS');	
	
	// 存储充值发货通知数据
	//var call_php_url = 'http://192.168.1.111/game_php_util/quick_sdk/save_recharge_data.php';
	//var redirect_php_url = call_php_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign+'&is_android='+1;
	//save_recharge_log(redirect_php_url);	
}


// 汇娱长尾充值回调(android)
exports.recharge_callback_cw_huiyu_android = function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    console.log('recharge_callback_cw_huiyu_android be call!(%s)',cur_time);

	var call_game_url = recharge_callback_android_url;
	var redirect_url = call_game_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign;	
	common_logic.redirect_to_game_server(redirect_url, res, 'SUCCESS');	
	
	// 存储充值发货通知数据
	//var call_php_url = 'http://192.168.1.111/game_php_util/quick_sdk/save_recharge_data.php';
	//var redirect_php_url = call_php_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign+'&is_android='+1;
	//save_recharge_log(redirect_php_url);	
}






