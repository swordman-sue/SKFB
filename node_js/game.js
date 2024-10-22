var https = require('https');
var http = require('http');
var path = require('path');
var fs = require('fs');
var util = require('util');
var dateformat = require('./utils/dateformat');
var common_dao = require('./dao/common_dao');
var common_logic = require('./common_logic');


// 苹果内购验票据
exports.apple_verify_receipt = function (req, res) {	
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    console.log('apple_verify_receipt be call!(%s)',cur_time);
	
	// 密钥检测
	
	var role_id = req.body.role_id;			// 玩家ID	
	
	
	//测试地址: https://sandbox.itunes.apple.com/verifyReceipt 
	//正式地址: https://buy.itunes.apple.com/verifyReceipt 
	
	var receipt_data = req.body.receipt_data;
	var http = require('http');
	var qs = require('querystring');
	var content = qs.stringify(receipt_data); 
	var options = {
	  host: 'sandbox.itunes.apple.com',
	  port: 80,
	  path: '/verifyReceipt',
	  method: 'POST',
	  headers:{
	  'Content-Type':'application/x-www-form-urlencoded',
	  'Content-Length':content.length
	  }
	};
	var client = res;
	var req = http.request(options, function(res) {
	  var result_data='';
	  res.on('data', function(chunk){
		 result_data += chunk;
	  });
	  res.on('end', function(){
			console.log("\n--->>\nresult:",result_data)
			
			// 先响应客户端
			client.send(result_data);
			
			// 解析Json数据
			var json_data = JSON.parse(result_data);
			if(json_data.status != 0)
			{
				console.log('苹果内购验票据失败!(%d)',json_data.status);
				return;
			}
			
			// 向服务器发货
			/*
			"receipt": {
				"original_purchase_date_pst": "2016-04-28 03:18:49 America/Los_Angeles",
				"purchase_date_ms": "1461838729285",
				"unique_identifier": "d4e721ec67ef2feca7fbdbd25a45cfb37e10ea7b",
				"original_transaction_id": "1000000208620470",
				"bvrs": "1.1",
				"transaction_id": "1000000208620470",
				"quantity": "1",
				"unique_vendor_identifier": "8E19EEC4-33D7-4536-B62E-112BAC68EECD",
				"item_id": "1108798151",
				"product_id": "1244",
				"purchase_date": "2016-04-28 10:18:49 Etc/GMT",
				"original_purchase_date": "2016-04-28 10:18:49 Etc/GMT",
				"purchase_date_pst": "2016-04-28 03:18:49 America/Los_Angeles",
				"bid": "com.doctorHys",
				"original_purchase_date_ms": "1461838729285"
			},*/ 		
	   });
	});
	req.write(content);
	req.end();	
}

// 取CP订单号
exports.get_cp_order_id = function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
	console.log('get_cp_order_id be call!(%s)',cur_time);	
	
	var app_role_id = req.body.app_role_id;		// string	游戏角色ID
	var price = req.body.price;					// float	价格（单位为：元）
	
	// todo::验签
	
	// 响应数据
	var resp_data = {
		error_code: 200,
		order_id: 0,
		error_msg: "success"
	};	
	var result_list = common_dao.get_order_series_id().then(function (result_list) {
        if (result_list.length > 0) {
			
			var order_id =  result_list[0].order_series_id + 1;
					
			common_dao.update_order_series_id(order_id);
			
			console.log('result_list.order_series_id:%d',order_id);	
			
			// 记录订单号信息
			var order_info = {
				record_time : cur_time,
				app_role_id : app_role_id,
				price : price
			}
			common_dao.add_order_info(order_id, order_info);
			
			resp_data.order_id = order_id;
			res.send(resp_data);
        }
		else{	
			resp_data.error_code = -1;
			resp_data.order_id = 0;
			resp_data.error_msg = 'system error';
			res.send(resp_data);
		}
    });	
}	

// 请求订单号
exports.cp_order_id = function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
	console.log('cp_order_id be call!(%s)',cur_time);	
	
	// 签名  规则：32位小写md5(app_id + yd_order_id + yd_user_id + app_role_id + server_id + coin + price 
	// + create_time+ goods_code + pay_channel +app_key)
	// 以上规则中的“+”号 不参与字符串拼接；
	// app_id与app_key是SDK对接时对接人员分配的云顶产品ID与密钥；
	var app_id = req.body.app_id;				// int	云顶分配的应用编号ID
	var yd_order_id = req.body.yd_order_id;		// string	云顶流水单号
	var yd_user_id = req.body.yd_user_id;		// int	云顶用户ID
	var app_role_id = req.body.app_role_id;		// string	游戏角色ID
	var server_id = req.body.server_id;			// string	区服ID
	var coin = req.body.coin;					// float	游戏代币数量
	var price = req.body.price;					// float	人民币数量 （单位为：元）
	var create_time = req.body.create_time;		// int	订单提交Unix时间戳（精确到秒，例：1448899200）
	var goods_code = req.body.goods_code;		// string	商品代码
	var extend_data = req.body.extend_data;		// string	透传参数，由厂商客户端传入，原样返回(recharge_type+order_id)	
	var pay_channel = req.body.pay_channel;		// int	1-AppStore 2-支付宝 3-支付宝快捷 4-支付宝网银 5-财付通 
												// 6-银行电汇 7-云钻 8-微信 ios正版固定传1
	var sign = req.body.sign;					// string	签名
	
	var app_key = "ljFh4tk94NDCzy9euv9dyQ0i9xv4lZq0";

	// 检测app_id是否合法
	if(app_id != 6381)
	{
		console.log('请求订单号本地与远端app_id不匹配(fore_app_id:%s,local_app_id:%s)', app_id, 6381);
		return;
	}
		
	var ready_sign_str = util.format('%d%s%d%s%s%d%d%d%s%d%s',app_id, yd_order_id, yd_user_id,
	app_role_id,server_id,coin,price,create_time,goods_code,pay_channel,app_key);
	
    var already_sign_str = md5.encrypt(ready_sign_str);
	
	// 检测签名串是否匹配
	if(already_sign_str != sign)
	{
		console.log('请求订单号本地与远端签名串不匹配(fore_sign:%s,local_sign:%s)', sign, already_sign_str);
		return;
	}	

			
	var result_list = common_dao.get_order_series_id().then(function (result_list) {
        if (result_list.length > 0) {
			
			var order_id =  result_list[0].order_series_id + 1;
					
			common_dao.update_order_series_id(order_id);
			
			console.log('result_list.order_series_id:%d',order_id);	
			
			// todo::记录云顶订单号与CP订单号映射关系
			
			
			var resp_data = {
				error_code: 200,
				order_id: order_id,
			};		
			res.send(resp_data);
        }
		else{
			var resp_data = {
				error_code: 0,
				order_id: 0,
				error_msg: "error"
			};		
			res.send(resp_data);
		}
    });
}

// 取在线玩家数量
function get_online_player_num(url, res){
	var client = res;
	http.get(url, function (res) {
        var list = [];
        var len = 0;
        res.on('data', function (data) {
            list.push(data);
            len += data.length;
        });
        res.on('end', function () {
            var result = Buffer.concat(list, len).toString("utf-8");
			client.send(result);	
        });
    }).on('error', function (e) {
		console.log('发送消息到游戏服务器出错:'+e);
    });		
}

// 请求在线玩家数量
exports.online_player_num = function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
	console.log('online_player_num be call!(%s)',cur_time);			
	var call_game_url = "http://127.0.0.1:40006/online_player_num";
	var redirect_url = call_game_url;
	get_online_player_num(redirect_url, res);
}


// 爱贝支付下单
exports.iapppay_order = function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
	console.log('iapppay_order be call!(%s)',cur_time);	
	
	console.log(req.body);
	
	// parseFloat
	var appid = req.body.appid;			// appid	应用编号	String(20)	必填	平台分配的应用编号
	var waresid = req.body.waresid;		// waresid	商品编号	Integer	必填	应用中的商品编号
	var waresname = req.body.goodsname;	// waresname	商品名称	String(32)	可选	商品名称，对于消费型_应用传入价格的计费方式有效，如果不传则展示后台设置的商品名称
	//var cporderid = req.body.cporderid;	// cporderid	商户订单号	String(64)	必填	商户生成的订单号，需要保证系统唯一
	var price = req.body.fee;	// parseFloat// price	支付金额	Float(6,2)	可选	支付金额，对于消费型_应用传入价格的计费方式有效，其它计费方式不需要传入本参数
	var currency = req.body.currency;		// currency	货币类型	String(32)	必填	货币类型以及单位：RMB – 人民币（单位：元）
	var appuserid = req.body.role_id;		// appuserid	用户在商户应用的唯一标识	String(32)	必填	用户在商户应用的唯一标识，建议为用户帐号。对于游戏，需要区分到不同区服，#号分隔；比如游戏帐号abc在01区，则传入“abc#01”
	var cpprivateinfo = req.body.extra_params;	// cpprivateinfo	商户私有信息	String(64)	可选	商户私有信息，支付完成后发送支付结果通知时会透传给商户
	var notifyurl = 'http://17m13880g2.iok.la:10197/recharge_callback/ios_ly/kuxuan'; 	// notifyurl	支付结果通知地址	String(128)	可选	商户服务端接收支付结果通知的地址
	
	// todo::验签
	
	var result_list1 = common_dao.get_order_series_id_with_update().then(function (result_list) {
        if (result_list.length > 0) {
			
			var order_id = result_list[0][0].order_series_id;
					
			//common_dao.update_order_series_id(order_id);
			
			//console.log('result_list.order_series_id:%d',order_id);	
			
			// 记录订单号信息
			var order_info = {
				record_time : cur_time,
				app_role_id : appuserid,
				price : price
			}
			//common_dao.add_order_info(order_id, order_info);
			
			var call_php_url = 'http://127.0.0.1/game_php_util/iapppay/trade.php';
			var param = util.format('appid=%s&waresid=%s&cporderid=%s&price=%s&currency=%s&appuserid=%s&cpprivateinfo=%s&notifyurl=%s&waresname=%s',
			appid, waresid,order_id,price,currency,appuserid,cpprivateinfo,notifyurl,waresname);
			var url = call_php_url+'?'+param;
			//console.log(url);
			common_logic.redirect_to_third_server(url, res);					
        }
		else{	
			res.send('0');
		}
    });	
}



// 请求服务器列表
exports.server_list = function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
	console.log('server_list be call!(%s)',cur_time);	
	
	common_dao.get_server_list().then(function (result) {
		var server_list = {};
		for(var i = 0; i < result.length; ++i){
			var server_data = {};
			server_data['server_id'] = result[i].server_id;
			server_data['name'] = result[i].name;
			server_data['open_time'] = result[i].open_time;
			server_list[i] = server_data;
		}
		res.send(server_list);
    });	
}

// 请求角色信息
exports.role_info = function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
	console.log('role_info be call!(%s)',cur_time);	
	
	console.log(req.query);	
	
	var account = req.query.account;
	var server_id = req.query.server_id;

	var resp_data = {};
	
	if(account == undefined){
		resp_data['code'] = 0;
		resp_data['desc'] = 'account undefined';
		res.send(resp_data);
		console.log('account == undefined');	
		return;
	}
	
	if(server_id == undefined){
		resp_data['code'] = 0;
		resp_data['desc'] = 'server_id undefined';
		res.send(resp_data);	
		console.log('server_id == undefined');	
		return;
	}	
	
	common_dao.get_role_info(account,server_id).then(function (result) {
        if (result.length > 0) {
			resp_data['code'] = 1;
			resp_data['desc'] = 'ok';				
			resp_data['role_id'] = result[0].role_id;
			resp_data['role_name'] = result[0].role_name;
        }else{
			resp_data['code'] = 2;
			resp_data['desc'] = 'not found';
		}
		res.send(resp_data);
    });	
}


// 客户端事件统计
exports.client_event_statistics = function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
	console.log('client_event_statistics be call!(%s)',cur_time);	
	
	console.log(req.body);	
	
	// 检测AppKey
	//if(req.body.app_key != 20160602){
	//	console.log('app_key error!(%d)',req.body.app_key);
	//	return;
	//}
	
	var channel_type 	= req.body.cp_channel_type;
	var data_id 		= req.body.data_id;
	var device_id 		= req.body.deviceid;
	var event_type 		= req.body.type;
	var is_new 			= req.body.is_new;
	var is_ios 			= req.body.is_ios;
	var ip 				= common_logic.get_client_ip_ex(req);
	
	var device_model 	= req.body.device_model;	// 机型
	var mem_size 		= req.body.mem_size;		// 可用内存
	var app_version 	= req.body.app_version;		// 版本号
	var os 				= req.body.os;				// (操作系统：win32、apple、android)
	
	var account			= req.body.account;	
	var server_id		= req.body.server_id;	
	var role_id			= req.body.role_id;		

	var cur_int_time = Date.now() / 1000;
	var last_update_time = cur_time; //  
	
	/*
		事件类型：
		游戏启动 1
		取版本信息 2
		取版本信息列表 3
		加载版本信息列表 4
		下载脚本压缩包 5
		解压脚本压缩包 6
		脚本逻辑启动 7
		加载第一批初始脚本 8
		开始资源解压和更新 9
		开始公共资源加载 10
		加载第二批初始脚本 11
		取服务器配置 12
		取服务器列表 13
		连接服务器 14
		打开登录界面 15
		打开SDK登录界面 16
		SDK登录成功 17
		进入游戏 18
	*/

	var table_name;
	
	// 翔通定制Ios 12001-12099
	if(channel_type >= 12001 && channel_type <= 12099){
		table_name = 'event_record_xtone_jt_ios';
	// 翔通定制android 11601-11699
	}else if(channel_type >= 11601 && channel_type <= 11699){
		table_name = 'event_record_xtone_jt_android';
	// unlock
	}else if(channel_type >= 20201 && channel_type <= 20299){
		table_name = 'event_record_unlock';
	// 乐嗨嗨BT	
	}else if(channel_type == 99991){
		table_name = 'event_record_lhh_bt';			
	}else{
		table_name = 'event_record';
		//console.log('暂未处理的类型!(channel_type:%d)',channel_type);
	}
	
	
	if(event_type == 1 || event_type == 5 || event_type == 7){		
		common_dao.add_client_event(table_name,data_id,device_id,channel_type,event_type,is_new,
		ip,cur_time,device_model,mem_size,app_version,os,cur_int_time);
	}else if(event_type == 18){
		common_dao.update_client_event_ex(table_name,data_id,device_id,event_type,account,server_id,role_id);
	}else{
		common_dao.update_client_event(table_name,data_id,device_id,event_type,cur_int_time,last_update_time);
	}	
	
	res.send("ok");
}


exports.event_statistics_error_log = function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
	console.log('event_statistics_error_log be call!(%s)',cur_time);	
	
	console.log(req.body);	
	
	/*
		所需参数：
		data_id
		device_id
		cp_channel_type
		type
		device_model(机型) 字符串(64)
		mem_size(可用内存) 数值 
		app_version(版本号) 字符串(64)
		os(操作系统：win32、apple、android) 字符串(16)
		error_msg(错误信息)1280
		
	*/
	
	var channel_type 	= req.body.cp_channel_type;
	var data_id 		= req.body.data_id;
	var device_id 		= req.body.deviceid;
	var event_type 		= req.body.type;
	var device_model 	= req.body.device_model;
	var mem_size 		= req.body.mem_size;
	var app_version 	= req.body.app_version;
	var os 				= req.body.os;
	var error_msg 		= req.body.error_msg;
	
	// (table_name,data_id,device_id,cp_channel_type,event_type,device_model,mem_size,app_version,os,error_msg,record_time)
	common_dao.add_event_statistics_error_log('error_log',data_id,device_id,channel_type,
	event_type,device_model,mem_size,app_version,os,error_msg,cur_time)
	
	res.send("ok");
}

// 活动公告 
exports.activity_notice = function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
	console.log('activity_notice be call!(%s)',cur_time);	
	
	console.log(req.body);	
	
	/*
		所需参数：
		data_id
		device_id
		cp_channel_type
		type
		device_model(机型) 字符串(64)
		mem_size(可用内存) 数值 
		app_version(版本号) 字符串(64)
		os(操作系统：win32、apple、android) 字符串(16)
		error_msg(错误信息)1280
		
	*/
	
	var channel_type 	= req.body.cp_channel_type;
	var data_id 		= req.body.data_id;
	var device_id 		= req.body.deviceid;
	var event_type 		= req.body.type;
	var device_model 	= req.body.device_model;
	var mem_size 		= req.body.mem_size;
	var app_version 	= req.body.app_version;
	var os 				= req.body.os;
	var error_msg 		= req.body.error_msg;
	
	// (table_name,data_id,device_id,cp_channel_type,event_type,device_model,mem_size,app_version,os,error_msg,record_time)
	common_dao.add_event_statistics_error_log('error_log',data_id,device_id,channel_type,
	event_type,device_model,mem_size,app_version,os,error_msg,cur_time)
	
	res.send("ok");
}










