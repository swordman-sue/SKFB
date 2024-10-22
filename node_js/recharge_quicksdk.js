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


// 国内版本
exports.recharge_callback_I = function (req, res) {
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

// 国外版本
exports.recharge_callback_II = function (req, res, game_svr_ip, game_svr_port,callbackKey) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    console.log('recharge_callback_II be call!(%s)',cur_time);
	
	 console.log(req.body);
			
	/*
		1.	接口目的
		玩家在完成充值后,SDK会按照此接口协议向游戏提供的发货地址POST数据（简称为发送通知），游戏在收到SDK POST的数据后应验证相关信息并向玩家发送道具。

		v	游戏应按照第三章中的算法验证签名数据，确定请求是否合法。
		v	游戏发货的金额应当以通知中的金额为准发放道具。
		v	游戏应判断是否重复发放道具，若已发放则不能再次发放。
		v	处理成功后返回SUCCESS这7个字母SDK将不再通知，否则SDK将在一段内持续通知
		
		{ 
		  uid: '69',
		  username: 'xs47380392',
		  cpOrderNo: '1543923469',
		  orderNo: '0020181204193748413759',
		  payTime: '2018-12-04 19:38:16',
		  payAmount: '0.99',
		  payType: '8',
		  payStatus: '0',
		  payCurrency: 'USD',
		  usdAmount: '0.99',
		  extrasParams: 'eyJnb29kc19pZCI6MCwiY3BfY2hhbm5lbF90eXBlIjozMDAxMCwiY3Bfb3JkZXJfaWQiOiIxNTQzOTIzNDY5IiwicmVjaGFyZ2VfdHlwZSI6MiwiaXNfaW5fZXhhbWluZSI6MCwiaXAiOiIxMTMuNjYuMjUxLjIyMCIsInJvbGVfaWQiOjIyNSwicmVjaGFyZ2VfaWQiOjIwMDF9',
		  actRate: '1',
		  sign: '905342fb02e7e1c095694530e299d6b5' 
		}			

	*/
	
													// 参数			必要性	说明
	var uid 			= req.body.uid;				// uid			必传	购买道具的用户uid
	var username 		= req.body.username;		// username		必有	购买道具的用户username
	var cpOrderNo 		= req.body.cpOrderNo;		// cpOrderNo	可为空	游戏下单时传递的游戏订单号，原样返回
	var orderNo 		= req.body.orderNo;			// orderNo		必有	SDK唯一订单号
	var payTime 		= req.body.payTime;			// payTime		必有	用户支付时间，如2017-02-06 14:22:32
	var payAmount 		= req.body.payAmount;		// payAmount	必有	用户支付金额
	var payCurrency 	= req.body.payCurrency;		// payCurrency	必有	用户支持的币种，如RMB，USD等
	var usdAmount 		= req.body.usdAmount;		// usdAmount	必有	用户支付的游戏道具以美元计价的金额
	var extrasParams 	= req.body.extrasParams;	// extrasParams	可为空	游戏下单时传递的扩展参数，将原样返回。
	var sdk_sign 		= req.body.sign;			// sign	必有	签名值，游戏应根据签名约定，本地计算后与此值进行比对
	var payType 		= req.body.payType;	
	var payStatus 		= req.body.payStatus;	
	var actRate 		= req.body.actRate;	
	
	// 验签名规则
	/*
	1. 接收方收到消息后将收到的键值对POST参数，移除sign后，按首字母升序排序。
	2. 将所有键值对按 key1=val1&key2=val2&key3=val3& 格式进行拼接。
	3. 在第3步拼接的字符后拼接SDK约定的callbackKey则此时格式为：key1=val1&key2=val2&key3=val3&callbackKey
	4. 将第4步处理的字符串进行md5编码，得到32位md5值。
	5. 将第5步获取的md5值与第1步中发起方传递的sign值进行对比，一致则通过。
	*/

	
	// 参数列表
	var param_list = [];
	param_list[0] = {key:'uid',			value:uid};
	param_list[1] = {key:'username',	value:username};
	param_list[2] = {key:'cpOrderNo',	value:cpOrderNo};
	param_list[3] = {key:'orderNo',		value:orderNo};
	param_list[4] = {key:'payTime',		value:payTime};
	param_list[5] = {key:'payAmount',	value:payAmount};
	param_list[6] = {key:'payCurrency',	value:payCurrency};
	param_list[7] = {key:'usdAmount',	value:usdAmount};
	param_list[8] = {key:'extrasParams',value:extrasParams};
	param_list[9] = {key:'payType',		value:payType};
	param_list[10] = {key:'payStatus',	value:payStatus};
	param_list[11] = {key:'actRate',	value:actRate};
	

	// 按首字母升序排序
	//param_list.sort(common_logic.key_sort('key', false));
	
	
	//var str_for_sign = '';
	//for(var i = 0; i < param_list.length; ++i){
	//	str_for_sign += util.format('%s=%s&',param_list[i].key, param_list[i].value);
	//}	
	//str_for_sign += callbackKey;
	
	// cpOrderNo=1543908441&extrasParams={\"goods_id\":0,\"cp_channel_type\":30010,\"cp_order_id\":\"1543908441\",\"recharge_type\":2,\"is_in_examine\":0,\"ip\":\"113.66.251.220\",\"role_id\":225,\"recharge_id\":2003}
	// &orderNo=0020181204152720977927&payAmount=14.99&payCurrency=USD&payTime=2018-12-04 15:27:33&uid=69&usdAmount=14.99&username=xs47380392&21299129384669034103404539411823
	var str_for_sign = util.format('actRate=%s&cpOrderNo=%s&extrasParams=%s&orderNo=%s&payAmount=%s&payCurrency=%s&payStatus=%s&payTime=%s&payType=%s&uid=%s&usdAmount=%s&username=%s&%s',
	actRate, cpOrderNo, extrasParams, orderNo, payAmount, payCurrency, payStatus, payTime, payType, uid, usdAmount, username, callbackKey);
	
	console.log(str_for_sign);
	
	//console.log(extrasParams);
		
	var cp_sign = common_logic.my_md5(str_for_sign); // 不要转换为大写
	
	// 检测签名串是否匹配
	if(cp_sign != sdk_sign)
	{
		console.log('签名不匹配(sdk_sign:%s,cp_sign:%s)', sdk_sign, cp_sign);
		res.send('FAILED');
		return;
	}
	
	var extrasParams = new Buffer(extrasParams, 'base64').toString();
	
	// 自定义参数带过来的数据
	// 1.充值Id 2.充值类型 3.cp充值ID, 5.IP地址 4.商品ID  6.cp自定义渠道类型
	var extrasParams 		= JSON.parse(extrasParams);
	var recharge_id 		= extrasParams.recharge_id;
	var recharge_type 		= extrasParams.recharge_type;
	var cp_order_id 		= extrasParams.cp_order_id; 
	var ip 					= extrasParams.ip;	
	var goods_id 			= extrasParams.goods_id;
	var cp_channel_type		= extrasParams.cp_channel_type; 
	var app_role_id			= extrasParams.role_id;		
	
	var sdk_order_id = orderNo;
	var yd_channel_id = 1; 
	
	payAmount *= 100;
	
	var url = util.format('http://%s:%d/recharge_callback_common',game_svr_ip, game_svr_port);
	var url = url + '?sdk_order_id='+sdk_order_id+'&app_role_id='+app_role_id+'&price='+payAmount+'&goods_id='+goods_id+'&recharge_id='+recharge_id+'&recharge_type='+
	recharge_type+'&yd_channel_id='+yd_channel_id+'&ip='+ip+'&cp_order_id='+cp_order_id+'&cp_channel_type='+cp_channel_type+'&currency_type=1';	
	console.log(url);
	common_logic.redirect_to_game_server(url, res, 'SUCCESS');	
}



















