var https = require('https');
var http = require('http');
var path = require('path');
var fs = require('fs');
var util = require('util');
var dateformat = require('./utils/dateformat');
var common_dao = require('./dao/common_dao');
var Md5 = require('./utils/Md5');
var md5=new Md5();
var common_logic = require('./common_logic');
var config = require('./utils/config');

var recharge_callback_url = config.recharge_callback_url;



// 智品充值回调
exports.recharge_callback_zhipin = function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    console.log('recharge_callback_zhipin be call!(%s)',cur_time);
			
	/*
	amount	int	以分为单位的 RMB 金额	200	整数, 合作方应当做好单位转换
	real_amount	int	实际支付金额，以分为单位的 RMB 金额	100	2017年5月8日上线, 对账时使用, amount 建议用于发送金币
	apporder	string	App传入的订单ID	00000	合作方应当做唯一区分
	sdkorder	string	Pay2平台中的订单ID	100	暂无
	sign	string	md5 加密密文	3b1eb7f7d1372ccf8544bc773a4c38bd	小写
	sign2	string	md5 加密密文	2610b0446555e93f907f133bdf532c84	小写
	success	string	1为成功, 非1为失败	1	
	ts	int	订单支付时间	1494209825	Unix 时间戳
	userdata	string	app 透传数据	test	
	test	int	1为测试数据, 0为上架通知	0	当使用网页支付时才会为1，用于区分支付来源, test 不参与sign验证于2017年4月25日添加
	*/
				
	var amount = req.query.amount;				// int	以分为单位的 RMB 金额	200	整数, 合作方应当做好单位转换
	var real_amount = req.query.real_amount;	// int	实际支付金额，以分为单位的 RMB 金额	100	2017年5月8日上线, 对账时使用, amount 建议用于发送金币
	var apporder = req.query.apporder;			// string	App传入的订单ID	00000	合作方应当做唯一区分
	var sdkorder = req.query.sdkorder;			// string	Pay2平台中的订单ID	100	暂无
	var sign = req.query.sign;					// string	md5 加密密文	3b1eb7f7d1372ccf8544bc773a4c38bd	小写
	var sign2 = req.query.sign2;				// string	md5 加密密文	2610b0446555e93f907f133bdf532c84	小写
	var success = req.query.success;			// string	1为成功, 非1为失败	1
	var ts = req.query.ts;						// int	订单支付时间	1494209825	Unix 时间戳
	var userdata = req.query.userdata;			// string	app 透传数据	tes
	var test = req.query.test;					// int	1为测试数据, 0为上架通知	0	当使用网页支付时才会为1，用于区分支付来源, test 不参与sign验证于2017年4月25日添加
	
	console.log('amount:%d', amount);
	console.log('real_amount:%d', real_amount);
	
	// 充值失败
	if(success != 1)
	{
		console.log('充值失败(cp_order_id:%s,sdkorder:%s)', apporder, sdkorder);
		return;
	}
		
	// 自定义参数带过来的数据
	// 1.充值Id 2.充值类型 3.cp充值ID, 5.IP地址 4.商品ID  6.cp自定义渠道类型
	var json_extend_data 	= JSON.parse(userdata);
	var recharge_id 		= json_extend_data.recharge_id;
	var recharge_type 		= json_extend_data.recharge_type;
	//var cp_order_id 		= json_extend_data.cp_order_id; // 由于sdk那边限制了extra字段的长度,所以，只能拿掉这个字段防止超过
	var ip 					= "127.0.0.1"; // json_extend_data.ip;	// 由于sdk那边限制了extra字段的长度,所以，只能拿掉这个字段防止超过
	var goods_id 			= json_extend_data.goods_id;
	var cp_channel_type		= 11401; //json_extend_data.cp_channel_type; // 由于sdk那边限制了extra字段的长度,所以，只能拿掉这个字段防止超过
	var app_role_id			= json_extend_data.role_id;	

	// sign 加密列子  apporder + sdkorder+ amount + success + ts + notify_secret;
	var notify_secret = '84be6c86995e88c852e70b41d6f287b5';
	var str_for_sign = util.format('%s%s%d%s%s%s',apporder, sdkorder, amount, success, ts, notify_secret);	

	// sign2 加密列子, 保留 sign 是为了兼容历史版本, 2017年5月8日后只需要验证 sign2 即可, 如果不在乎实际金额(仅短代时 real_amount 和 amount 会不一致)
	//var str_for_sign2 = apporder + sdkorder+ amount + success + ts + notify_secret + real_amount;
	var str_for_sign2 = util.format('%s%s%d%s%s%s%d',apporder, sdkorder, amount, success, ts, notify_secret, real_amount);	
	
	var md5_str = common_logic.my_md5(str_for_sign); // 不要转换为大写
    var md5_str2 = common_logic.my_md5(str_for_sign2);// 不要转换为大写
	
	// 检测签名串是否匹配
	if(md5_str2 != sign2)
	{
		console.log('本地与远端签名串不匹配(sdk_sign:%s,cp_sign:%s)', sign2, md5_str2);
		return;
	}
	
	var cp_order_id = apporder;		
	var sdk_order_id = sdkorder;
	var yd_channel_id = 1; //cp_channel_type;
	
	amount = amount * 0.01;
	
	var call_game_url = recharge_callback_url;
	//var call_game_url = "http://192.168.1.78:40036/recharge_callback_common"; // recharge_callback_url
	var redirect_url = call_game_url+'?sdk_order_id='+sdk_order_id+'&app_role_id='+app_role_id+
	'&price='+amount+'&goods_id='+goods_id+'&recharge_id='+recharge_id+'&recharge_type='+
	recharge_type+'&yd_channel_id='+yd_channel_id+'&ip='+ip+'&cp_order_id='+cp_order_id+'&cp_channel_type='+cp_channel_type;	
	common_logic.redirect_to_game_server(redirect_url, res, 'success');	
}



















