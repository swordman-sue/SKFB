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
var URL = require('url');


/*
	这里的正常流程是按1,2,3,4,5步骤进行
*/

// set 3
//  http_get 请求
function http_get(url, extend_data, http){
	//var client = res;
	http.get(url, function (res) {
        var list = [];
        var len = 0;
        res.on('data', function (data) {
            list.push(data);
            len += data.length;
        });
        res.on('end', function () {
            var result_data = Buffer.concat(list, len).toString("utf-8");
			check_user_info_result(extend_data, result_data);
        });
    }).on('error', function (e) {
		console.log('重定向消息到第三方服务器出错:'+e);
    });		
}	

// http post 请求
function http_post(url, extend_data, http, post_data){
	
	var qs = require('querystring');	
	var content = qs.stringify(post_data);
	console.log(content); 
	var p = URL.parse(url); 
	var options = {
		host: p.hostname,
		port: p.port || 80,
		path: p.path,
		method: 'POST',
		headers:{
		'Content-Type':'application/x-www-form-urlencoded',
		'Content-Length':content.length
		}
	};
	var req = http.request(options, function(res) {
	  var result_data='';	  
	  res.on('data', function(chunk){
		 result_data += chunk;
	  });
	  res.on('end', function(){
		//console.log(result_data);  
		check_user_info_result(extend_data, result_data);
	   });
	});
	req.write(content);
	req.end();			
}

// http post 请求
function https_post(url, extend_data, https, post_data){
	
	var qs = require('querystring');	
	var content = qs.stringify(post_data);
	//console.log(content); 
	var p = URL.parse(url);	
	var options = {
		host: p.hostname,
		//port: p.port || 443,
		port: 443, // 因为是https, 所以端口为443
		path: p.path,
		method: 'POST',
		rejectUnauthorized: false,
		headers:{
		'Content-Type':'application/x-www-form-urlencoded',
		'Content-Length':content.length
		}
	};
	var req = https.request(options, function(res) {
	  var result_data='';
	  res.on('data', function(chunk){
		 result_data += chunk;
	  });
	  res.on('end', function(){
		console.log(result_data);  
		check_user_info_result(extend_data, result_data);
	   });
	});
	req.write(content);
	req.end();			
}

// http post 请求
function https_post_ex(url, extend_data, https, post_data){
	
	var qs = require('querystring');	
	var content = qs.stringify(post_data);
	//console.log(content); 
	var p = URL.parse(url);	
	var options = {
		host: p.hostname,
		port: p.port || 8443,
		//port: 443, // 因为是https, 所以端口为443
		path: p.path,
		method: 'POST',
		rejectUnauthorized: false,
		headers:{
		'Content-Type':'application/x-www-form-urlencoded',
		'Content-Length':content.length
		}
	};
	var req = https.request(options, function(res) {
	  var result_data='';
	  res.on('data', function(chunk){
		 result_data += chunk;
	  });
	  res.on('end', function(){
		console.log(result_data);  
		check_user_info_result(extend_data, result_data);
	   });
	});
	req.write(content);
	req.end();			
}

// http post 请求
function http_post_json(url, extend_data, http, post_data){
		
    var content = JSON.stringify(post_data);  
    console.log(content);  	
	 
	var p = URL.parse(url); 
	var options = {
		host: p.hostname,
		port: p.port || 80,
		path: p.path,
		method: 'POST',
		headers:{
		'Content-Type':'application/json',
		'Content-Length':content.length
		}
	};
	var req = http.request(options, function(res) {
	  var result_data='';
	  res.on('data', function(chunk){
		 result_data += chunk;
	  });
	  res.on('end', function(){
		 console.log(result_data);
		 check_user_info_result(extend_data, result_data);
	   });
	});
	req.write(content);
	req.end();			
}	

// 发送结果给游戏
function send_result_to_gameserver(game_server, account)
{
	// 构造响应数据
	var resp_data = {
		result : 1,
		desc :'success',
		account : account
	}
	
	// 响应
	game_server.send(resp_data);
}

// 发送结果给游戏
function send_errro_to_gameserver(game_server, account)
{
	// 构造响应数据
	var resp_data = {
		result : 0,
		desc :'fail',
		account : account
	}
	
	// 响应
	game_server.send(resp_data);
}

// set 1 检测用户信息(todo::暂时不使用了)
module.exports.check_user_info1 = function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
	console.log('check_user_info be call!(%s)',cur_time);	
	
	console.log(req.body);	
	
	var terminal_type = req.body.terminal_type;		// 终端类型(1=android 2=ios)
	var cp_channel_type = req.body.channel_type;	// CP渠道类型
	var account = req.body.account;					// 帐号
	var token = req.body.token;						// token
	var ip = req.body.ip;						// ip
		
	// todo::验签
	
	var extend_data = {
		terminal_type : terminal_type,
		cp_channel_type : cp_channel_type,
		account : account,
		token : token,
		ip : ip,
		res : res
	}
	
	// 拼接函数名称
	var func_name = util.format('do_check_user_info_%d', cp_channel_type * 0.01); 
	
	var myself = require('./check_user_info');

	// 调用函数
	myself[func_name](extend_data);
	
	/*
	// 云锐IOS联运
	if(cp_channel_type == 11001)
	{
		do_check_user_info_yunrui(extend_data);		
	}
	// 天娱IOS联运
	else if(cp_channel_type == 11101){
		do_check_user_info_tianyu(extend_data);
	}
	// 浩动IOS联运
	else if(cp_channel_type == 11201){
		
	}
	// todo::新加的需要在这里处理
	
	else{
		console.log('未处理的渠道类型!(%d)',cp_channel_type);
	}*/
} 
    

// 检测用户登录结果
function check_user_info_result(extend_data, result_data)
{
	var cp_channel_type = parseInt(extend_data.cp_channel_type * 0.01);
	
	// 拼接函数名称
	var func_name = util.format('check_user_info_result_%d', cp_channel_type); 
	
	console.log(func_name);
	
	var myself = require('./check_user_info');

	// 调用函数
	myself[func_name](extend_data, result_data);	
	
	/*
	// 云锐IOS联运
	if(cp_channel_type == 11001)
	{
		check_user_info_result_yunrui(extend_data, result_data);
	}
	// 天娱IOS联运
	else if(cp_channel_type == 11101)
	{
		check_user_info_result_tianyu(extend_data, result_data);
	}
	// 浩动IOS联运
	else if(cp_channel_type == 11201)
	{
		
	}	
	// todo::新加的需要在这里处理
	else{
		console.log('未处理的渠道类型!(%d)',cp_channel_type);
	}*/	
}











///////////////////////////// 开始位置
module.exports = {
	
	// set 1 检测用户信息
	check_user_info:function (req, res) 
	{
		var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
		console.log('check_user_info be call!(%s)',cur_time);	
		
		console.log(req.body);	
		
		var terminal_type = req.body.terminal_type;		// 终端类型(1=android 2=ios)
		var cp_channel_type = req.body.channel_type;	// CP渠道类型
		var account = req.body.account;					// 帐号
		var token = req.body.token;						// token
		var ip = req.body.ip;							// ip
			
		// todo::验签
		
		var extend_data = {
			terminal_type : terminal_type,
			cp_channel_type : cp_channel_type,
			account : account,
			token : token,
			ip : ip,
			res : res
		}
		
		// 转整形
		var cp_channel_type = parseInt(cp_channel_type * 0.01);
		
		// 拼接函数名称
		var func_name = util.format('do_check_user_info_%d', cp_channel_type); 
		
		console.log(func_name);	
		
		var myself = require('./check_user_info');

		// 调用函数
		myself[func_name](extend_data);
	}, 	
	

	// set 2
	// 处理检测用户信息(云锐) 11001
	do_check_user_info_110:function(extend_data)
	{
		// http://i.muomou.com/index.php/user/checkUser
		// 参数：
		// uid: ⽤户⾝份统⼀标识识别码
		// vkey: sid的值
		// appid: cp应⽤ID
		// sign: 数据加密验证字符串 sign构成：md5(uid + vkey + appid + appsecret) (字符串连接顺序不能变！)
		// http://i.muomou.com/index.php/user/checkUser/uid/123456/vkey/d1d5451216/appid/a22155/sign/adfe6652d2fereew343	
		// 返回：校验状态码json序列化字符串：{"status":"0","msg":"xxx"}
		// 操作失败：{“status”:"-1","msg":"xxx"} -1 app不不存在-2 未登录 -10000 ⾮非法请求
		
		var uid = extend_data.account;
		var vkey = extend_data.token;
		var cp_channel_type = extend_data.cp_channel_type;
		
		// AppId列表
		var app_id_list = [];  
		app_id_list[11001] = 'G100524';		
		app_id_list[11002] = 'G100550';	

		var app_secret_list = [];  
		app_secret_list[11001] = '8d3a6665413f4d68ba6438d0d69756be';		
		app_secret_list[11002] = 'b43c0229d3ae5fae38d9532f1e39c02c';		
		
		var appid = app_id_list[cp_channel_type]; 			//'G100524';
		var appsecret = app_secret_list[cp_channel_type];	//'8d3a6665413f4d68ba6438d0d69756be';
		var ready_to_sign_str = util.format('%s%s%s%s', uid, vkey, appid, appsecret);
		var already_sign_str = md5.encrypt(ready_to_sign_str);
		
		var params = util.format('/uid/%s/vkey/%s/appid/%s/sign/%s', uid, vkey, appid, already_sign_str); 
		var url = 'http://i.muomou.com/index.php/user/checkUser' + params;
		
		// 请求
		http_get(url, extend_data, http);	
	},

	// set 5
	// 用户信息校验结果(云锐) 11001
	check_user_info_result_110:function(extend_data, result_data)
	{
		var result_data_json = JSON.parse(result_data);
		
		// 检测是否失败
		if(result_data_json.status != 0)
		{
			console.log('用户信息校验失败(云锐)[%s]',result_data);
			return;
		}
		
		send_result_to_gameserver(extend_data.res, extend_data.account);
	},


	// set 2
	// 处理检测用户信息(天娱) 11101
	do_check_user_info_111:function(extend_data)
	{
		// http:// 47.104.167.135:8080/PlatformLogin/user/checkLogin
		// 请求为post 参数类型为json
		// userId	sdk登录成功返回的用户ID
		// accessToken	sdk登录成功返回的Token

		var url = 'http://47.104.167.135:8080/PlatformLogin/user/checkLogin';
			
		var post_data = {
			userId : extend_data.account,
			accessToken : extend_data.token
		};
		
		http_post_json(url, extend_data, http, post_data);
	},

	// 用户信息校验结果(天娱) 11101
	check_user_info_result_111:function(extend_data, result_data)
	{
		// code	int	成功：200 ,其他失败
		// message	String	对 code 的说明
		// 示例：{"code":200,"message":"exists"}
		var result_data_json = JSON.parse(result_data);
		
		// 检测是否失败
		if(result_data_json.code != 200)
		{
			console.log('用户信息校验失败(天娱)[%s]',result_data);
			return;
		}
		
		send_result_to_gameserver(extend_data.res, extend_data.account);	
	},
	
	
	// set 2
	// 处理检测用户信息(浩动) 11201
	do_check_user_info_112:function(extend_data)
	{				
		var userID = extend_data.account;
		var token = extend_data.token;
		var cp_channel_type = extend_data.cp_channel_type;
		
		//console.log(extend_data);	
		
		var app_secret_list = [];  
		app_secret_list[11201] = '55b69ee0053843c59543eefe8c5102f5';
		app_secret_list[11202] = 'eb5b794fa3a645dbb998e6fb945707e4';
		app_secret_list[11203] = 'c17ed433b7354e58b64ebd534663b2c0';	
		app_secret_list[11204] = 'dcee74597dca42a99874a7020495d8f1';	
		app_secret_list[11205] = 'e9f942c1002ae7846650efbf9fd0203b';	
		app_secret_list[11206] = '5fcb8379191e4e6fb458ec4362d1a073'; 
		app_secret_list[11207] = '88aca41460ce4d16b1f191197937201e';
		app_secret_list[11208] = '73b2c231fc794c3a94ca5e6e3370cbdb';
		app_secret_list[11209] = '4c21c23b62cc481d869721350f3f5583';
		app_secret_list[11210] = 'a9c265c5b2eb463eb30a89a80574bbbc';	
		app_secret_list[11211] = '5ca2f5985c7d45399f0d90694fc1d7bf';
		app_secret_list[11212] = '5d9529aeecef429a8b10d587e29b8881';
		app_secret_list[11213] = '0185e8fb78bc4c55a37597c99758adb4';
		app_secret_list[11214] = 'cad3316cc34c493fb4491ef9793b31cc';
		app_secret_list[11215] = 'ea7b41a265fd47c4bb04899b432aa5db';
		app_secret_list[11216] = '3b16ff7f0cb040b2bdae5b9d505afccc';
		app_secret_list[11217] = '46bfe8b209a048e09a6cfc425e1629ec';
		app_secret_list[11218] = 'fcc8d9cafe134dc3b7eade55eb59ab35';		
		
		
		
		
		var appsecret = app_secret_list[cp_channel_type];
		
		// 使用新的sdk
		if(cp_channel_type == 11205){
			/*
			http://uc.hi7.cn/api/v1/user/login/validate/access_token?token=ACCESST
			OKEN&uuid=USERID&appid=GZAPPID				
			返回json格式字符串 如:{"datas":{"status":1},"code":0}
			其中 status 为 1则表示有效帐号, 0则无效帐号
			*/
			
			var app_id_list = [];  
			app_id_list[11205] = 'gzc2cb557eb0f9946f';
			var appid = app_id_list[cp_channel_type];
			
			var params = util.format('token=%s&uuid=%s&appid=%s', token, userID, appid); 		
					
			var url = 'http://uc.hi7.cn/api/v1/user/login/validate/access_token?'+params;
			
			console.log('[%s]',url);	
				
			http_get(url, extend_data, http);		
		}
		else if(cp_channel_type >= 11219){
			/*
			请求 URL：http://api.mgwyx.com/index.php?r=auth/CheckToken
			请求方式：GET	
			game_id 是	string	游戏ID
			game_user_id  是	string	游戏用户ID
			access_token 是	string	用户登录access_token
			sign	是	string	签名，md5(access_token+game_id+game_user_id+app_key)
			*/
			
			var game_id_list = [];
			game_id_list[11219] = 2;
			game_id_list[11220] = 2;
			game_id_list[11221] = 2;
			var game_id = game_id_list[cp_channel_type];
			
			var app_key_list = [];
			app_key_list[11219] = '8beb24cc8ba60e68d194c3d8e6c307b9';
			app_key_list[11220] = '8beb24cc8ba60e68d194c3d8e6c307b9';
			app_key_list[11221] = '8beb24cc8ba60e68d194c3d8e6c307b9';
			var app_key = app_key_list[cp_channel_type];	
			
			var ready_to_sign_str = util.format('%s%s%s%s', token, game_id, userID, app_key);
			var already_sign_str = md5.encrypt(ready_to_sign_str);
			
			var params = util.format('game_id=%s&game_user_id=%s&access_token=%s&sign=%s', game_id, userID, token,already_sign_str); 		
					
			var url = 'http://api.mgwyx.com/index.php?r=auth/CheckToken&'+params;
			
			console.log('[%s]',url);	
				
			http_get(url, extend_data, http);				
			
		}
		// 使用旧的sdk
		else{			
			// http://eagle.hoho666.com/user/verifyAccount
			// 请求为 GET
			// userID 是 string 用户 id
			// token 是 string 访问 token
			// sign 是 string 签名，md5("userID="+userID+"token="+token+appSecret)	userID=[userID]token=[token][appSecret] 			
			
			var ready_to_sign_str = util.format('userID=%stoken=%s%s', userID, token, appsecret);
			var already_sign_str = md5.encrypt(ready_to_sign_str);
			
			var params = util.format('userID=%s&token=%s&sign=%s', userID, token, already_sign_str); 		
					
			var url = 'http://eagle.hoho666.com/user/verifyAccount?'+params;
			
			console.log('[%s]',url);	
				
			http_get(url, extend_data, http);			
		}
	},
	
	// 用户信息校验结果(浩动) 11201
	check_user_info_result_112:function(extend_data, result_data)
	{		
		if(result_data == undefined){
			console.log('用户信息校验失败(浩动)[%s]',result_data);
			return;
		}	
		
		var result_data_json = JSON.parse(result_data);
	
		var cp_channel_type = extend_data.cp_channel_type;
	
		// 使用新的sdk
		if(cp_channel_type == 11205)
		{
			/*
				返回json格式字符串 如:{"datas":{"status":1},"code":0}
				其中 status 为 1则表示有效帐号, 0则无效帐号
			*/
			
			//console.log('[result_data_json:%s]',result_data_json);	
			
			if(result_data_json.datas.status != 1){
				console.log('用户信息校验失败(浩动)[%s]',result_data);
				return;
			}	
			
			send_result_to_gameserver(extend_data.res, extend_data.account);
		}
		else if(cp_channel_type >= 11219){
			/*
				{
					"state":{
					"code":1,
					"msg":"会话验证成功"
					},
					"data":{
					"uid":"82246429",
					"nickname":"a1727920"
					}
				} 
			*/
			
			if(result_data_json.state.code != 1){
				console.log('用户信息校验失败(浩动)[%s]',result_data);
				return;
			}			
			
			send_result_to_gameserver(extend_data.res, result_data_json.data.uid);
		}
		// 使用旧的sdk
		else{			
			/*
			{
				"state" : "1",
				"data" : {
				"userID" : "50571",
				"username" : "1504514399598.zy",
				"channelID" : "10"
				}
			}*/
								
			// 检测是否失败
			if(result_data_json.state != 1){
				console.log('用户信息校验失败(浩动)[%s]',result_data);
				return;
			}
					
			// 转换为字符类型
			var account = util.format('%s', result_data_json.data.userID); 
			
			send_result_to_gameserver(extend_data.res, account);		
		}	
	},
	
		// set 2
	// 处理检测用户信息(酷炫) 11301
	do_check_user_info_113:function(extend_data)
	{
		// https://cp.iapppay.com/openid/openidcheck
		// 请求为 POST
		// 请求格式示例
		// transdata=urlencode(xxx)&sign=urlencode(yyy)&signtype=RSA
		// urlencode前内容：
		// transdata={"appid":"123","logintoken":"3213213"}&sign=xxxxxx&signtype=RSA
		// appid	应用编号	String(20)	必填	平台分配的应用编号
		// logintoken	登录令牌	String(32)	必填	商户调用计费平台SDK登录接口得到的登录令牌
		// sign	对TRANSDATA的签名数据		必填	对TRANSDATA的签名数据
		// signtype	签名算法类型		必填	签名算法类型，目前仅支持RSA			
		
		// app 检测方式
		//var appid = '3018063546';	
		//var logintoken = extend_data.token;
		//var call_php_url = 'http://127.0.0.1/game_php_util/iapppay/CheckLogin.php';
		//var url = call_php_url+'?appid='+appid+'&logintoken='+logintoken;
		//http_get(url, extend_data, http);		
		
		// H5 不需要校验
		send_result_to_gameserver(extend_data.res, extend_data.account);		
	},
	
	// 用户信息校验结果(酷炫) 11301 todo::因为不需要校验所有此方法不会调用
	check_user_info_result_113:function(extend_data, result_data)
	{
		/*
			成功应答数据样例（urlencode前内容）：
			transdata={"loginname":"testuser","userid":"1234567890"}&sign=xxxxxx&signtype=RSA
		*/
		
		console.log('用户信息校验结果(酷炫)[%s]',result_data);
		
		var result_data_json = JSON.parse(result_data);
		
		// 检测是否失败
		if(result_data_json.code == undefined)
		{
			console.log('用户信息校验失败(酷炫)[%s]',result_data);
			return;
		}
				
		// 转换为字符类型
		var account = result_data_json.transdata.userid; // util.format('%s', result_data_json.data.userID); 
		
		send_result_to_gameserver(extend_data.res, account);	
	},
	
		// set 2
	// 处理检测用户信息(智品) 11401
	do_check_user_info_114:function(extend_data)
	{
		/*
		https://open2.badambiz.com/api/user/verify_token/	
		请求参数包括appid和token
		appid：游戏在BadamSDK后台申请的游戏唯一标识游戏ID
		token：游戏客户端回传给游戏服务器的用户登录token
		*/
		
		var url = 'https://open2.badambiz.com/api/user/verify_token/';
		var appid = '00ab21dc40b382db0603411c7d50a899';
			
		var post_data = {
			appid:appid,
			token:extend_data.token
		};

		https_post(url, extend_data, https, post_data);
	},	
	
	// 用户信息校验结果(智品) 11401
	check_user_info_result_114:function(extend_data, result_data)
	{
		/*
			result：表示验证请求是否成功，0-表示验证成功，非0 表示验证失败
			message：错误信息
			data：用户信息，包括openid，account，nickname，avatar等		
		
			{
				"data": {
					"openid": "1000038",
					"account": "beihaison20",
					"nickname": "",
					"avatar": "http://oppzxlnxx.bkt.clouddn.com/pay2_headimg.png"
				},
				"result": 0
			}
		*/
		
		console.log('用户信息校验结果(智品)[%s]',result_data);		
		var result_data_json = JSON.parse(result_data);
			
		// 检测是否失败
		if(result_data_json.result != 0)
		{
			console.log('用户信息校验失败(智品)[result:%s,message:%s]', result_data_json.result, result_data_json.message);
			return;
		}
				
		// 转换为字符类型
		var account = result_data_json.data.account;
		
		send_result_to_gameserver(extend_data.res, account);	
		
	},	
	
	// set 2
	// 处理检测用户信息(糖谷) 11501
	do_check_user_info_115:function(extend_data)
	{
		/*
			协议：HTTP 请求方式： POST 编码：UTF-8
			请求地址：https://sdk.3344sy.com/loginVerify
			
			uid String 用户账号(必须) 1
			tokenId String 34sy 平台SDK 登录成功后返回(必须) 2
			sign String 签名(必须) 3 不参与签名	
			
			签名字符串为： appkey + uid + tokenId, 签名算法为32位小写MD5
		*/
		
		var url = 'https://sdk.3344sy.com/loginVerify'; 
		
		var appkey = 'a97ccfcffcfb6653edd8349a7e7197b9';
		var uid = extend_data.account;
		var tokenId = extend_data.token;
		
		var str_for_sign = util.format('%s%s%s',appkey, uid, tokenId);	
		var md5_str = md5.encrypt(str_for_sign);
						
		var post_data = {
			uid : uid,
			tokenId : tokenId,
			sign : md5_str
		};
		
		https_post(url, extend_data, https, post_data);
	},	
	
	// 用户信息校验结果(糖谷) 11501
	check_user_info_result_115:function(extend_data, result_data)
	{
		/*
			success 接收成功
			errorSign 签名错误
			error 未知错误
		*/
						
		// 检测是否失败
		if(result_data != 'success')
		{
			console.log('用户信息校验失败(糖谷)[result:%s]', result_data);
			return;
		}
		
		send_result_to_gameserver(extend_data.res, extend_data.account);	
	},	

	// 处理检测用户信息(翔通定制Android) 11601
	do_check_user_info_116:function(extend_data)
	{
		/*
			// 验证地址：http://oss.vanggame.com:8080/user/verifyAccount
			string strAppKey = "39a723159ac8597bfc766c0f5aefd25f";
			char szSign[256] = { 0 }; // Md5签名值（32位小写）md5("userID=" + userID + "token=" + token + appKey)，
			sprintf_s(szSign, sizeof(szSign), "userID=%stoken=%s%s", pRecv->m_szAccount, pRecv->m_szToken, strAppKey.c_str());

			MD5::Digest md5_sign = MD5::Encode(szSign);
			string strAlreadySignStr = md5_sign.HexString();
		*/
		
		var url = 'http://oss.vanggame.com:8080/user/verifyAccount?'; 
		
		var appkey = '578ecd1545aaa270578a81d92f43873e';  
		var userID = extend_data.account;
		var token = extend_data.token;
		
		var str_for_sign = util.format('userID=%stoken=%s%s',userID, token, appkey);	
		var md5_str = md5.encrypt(str_for_sign);
								
		var params = util.format('userID=%s&token=%s&sign=%s',userID, token, md5_str);	
		
		var call_url = url + params;
		
		console.log('call_url:%s', call_url);
		
		http_get(call_url, extend_data, http);
	},
	
	// 用户信息校验结果(翔通定制Android) 11601
	check_user_info_result_116:function(extend_data, result_data)
	{
		/*
			返回(JSON格式)：
			{
				state: 1(登录认证成功)；其他失败，查看失败状态码说明
				data: 认证成功才有数据，否则为空
				{
				userID:V8Server 生成的唯一用户ID
				username:V8Server 生成的统一格式的用户名
				}
			}
		*/
			
		console.log('result_data:%s', result_data);
		
		result_data = JSON.parse(result_data);
		
		// 检测是否失败
		if(result_data.state != 1)
		{
			console.log('用户信息校验失败(翔通定制)[state:%d]', result_data.state);
			return;
		}
		
		var userinfo = result_data.data;
		
		var userID = userinfo.userID.toString();
		
		console.log('userID:%s', userID);
		
		send_result_to_gameserver(extend_data.res, userID);	
	},	
	
	
	// 处理检测用户信息(翔通定制Ios) 12001
	do_check_user_info_120:function(extend_data)
	{
		/*
			// 验证地址：http://vserver.cyjj8.com:8080/user/verifyAccount
			string strAppKey = "39a723159ac8597bfc766c0f5aefd25f";
			char szSign[256] = { 0 }; // Md5签名值（32位小写）md5("userID=" + userID + "token=" + token + appKey)，
			sprintf_s(szSign, sizeof(szSign), "userID=%stoken=%s%s", pRecv->m_szAccount, pRecv->m_szToken, strAppKey.c_str());

			MD5::Digest md5_sign = MD5::Encode(szSign);
			string strAlreadySignStr = md5_sign.HexString();
		*/
		
		var cp_channel_type = extend_data.cp_channel_type;
		var url_list = [];  
		url_list[12001] = 'http://vserver.cyjj8.com:8080/user/verifyAccount?';	// ios_1	
		url_list[12002] = 'http://oss.vanggame.com:8080/user/verifyAccount?';	// ios_2
		var url = url_list[cp_channel_type];
		
		var app_key_list = []; 
		app_key_list[12001] = 'f9782ab22f6111c9bcd74db11e0c8a17';	// ios_1	
		app_key_list[12002] = '9b5d374ce279f22ed4db2d4174e6751c';	// ios_2	
		var appkey = app_key_list[cp_channel_type];
		var userID = extend_data.account;
		var token = extend_data.token;
		
		var str_for_sign = util.format('userID=%stoken=%s%s',userID, token, appkey);	
		var md5_str = md5.encrypt(str_for_sign);
								
		var params = util.format('userID=%s&token=%s&sign=%s',userID, token, md5_str);	
		
		var call_url = url + params;
		
		console.log('call_url:%s', call_url);
		
		http_get(call_url, extend_data, http);
	},
	
	// 用户信息校验结果(翔通定制Ios) 12001
	check_user_info_result_120:function(extend_data, result_data)
	{
		/*
			返回(JSON格式)：
			{
				state: 1(登录认证成功)；其他失败，查看失败状态码说明
				data: 认证成功才有数据，否则为空
				{
				userID:V8Server 生成的唯一用户ID
				username:V8Server 生成的统一格式的用户名
				}
			}
		*/
			
		console.log('result_data:%s', result_data);
		
		result_data = JSON.parse(result_data);
		
		// 检测是否失败
		if(result_data.state != 1)
		{
			console.log('用户信息校验失败(翔通定制)[state:%d]', result_data.state);
			return;
		}
		
		var userinfo = result_data.data;
		
		var userID = userinfo.userID.toString();
		
		console.log('userID:%s', userID);
		
		send_result_to_gameserver(extend_data.res, userID);	
	},	

	// 处理检测用户信息(翔通专服anroid and ios) 12101
	do_check_user_info_121:function(extend_data)
	{
		/*
			// 验证地址：http://vserver.cyjj8.com:8080/user/verifyAccount
			string strAppKey = "39a723159ac8597bfc766c0f5aefd25f";
			char szSign[256] = { 0 }; // Md5签名值（32位小写）md5("userID=" + userID + "token=" + token + appKey)，
			sprintf_s(szSign, sizeof(szSign), "userID=%stoken=%s%s", pRecv->m_szAccount, pRecv->m_szToken, strAppKey.c_str());

			MD5::Digest md5_sign = MD5::Encode(szSign);
			string strAlreadySignStr = md5_sign.HexString();
		*/
		
		var url = 'http://vserver.cyjj8.com:8080/user/verifyAccount?'; 
		
		
		var app_key_list = [];  
		app_key_list[12101] = '7548b762ee882c912d37aea37c17c290';	// android
		app_key_list[12110] = '69388abbfb69df9bfd55a641203a4d02';	// ios
		
		var appkey = app_key_list[extend_data.cp_channel_type]; 
		var userID = extend_data.account;
		var token = extend_data.token;
		
		var str_for_sign = util.format('userID=%stoken=%s%s',userID, token, appkey);	
		var md5_str = md5.encrypt(str_for_sign);
								
		var params = util.format('userID=%s&token=%s&sign=%s',userID, token, md5_str);	
		
		var call_url = url + params;
		
		console.log('call_url:%s', call_url);
		
		http_get(call_url, extend_data, http);
	},
	
	// 用户信息校验结果(翔通专服anroid and ios) 12101
	check_user_info_result_121:function(extend_data, result_data)
	{
		/*
			返回(JSON格式)：
			{
				state: 1(登录认证成功)；其他失败，查看失败状态码说明
				data: 认证成功才有数据，否则为空
				{
				userID:V8Server 生成的唯一用户ID
				username:V8Server 生成的统一格式的用户名
				}
			}
		*/
			
		console.log('result_data:%s', result_data);
		
		result_data = JSON.parse(result_data);
		
		// 检测是否失败
		if(result_data.state != 1)
		{
			console.log('用户信息校验失败(翔通专服anroid)[state:%d]', result_data.state);
			return;
		}
		
		var userinfo = result_data.data;
		
		var userID = userinfo.userID.toString();
		
		console.log('userID:%s', userID);
		
		send_result_to_gameserver(extend_data.res, userID);	
	},		
		
	
	// 处理检测用户信息(unlock) 20201
	do_check_user_info_202:function(extend_data)
	{
		/*
			接口地址 : http://api.unlock.game/index.php?ct=index&ac=tokenVerity
			传参方式：POST
			
			token	Y	CP作为二次校验用的token
		*/
		
		var url = 'http://api.unlock.game/index.php?ct=index&ac=tokenVerity';
						
		var post_data = {
			token : extend_data.token
		};
		
		var startTime = new Date().getTime();
		extend_data['start_time'] = startTime;		
		
		https_post(url, extend_data, https, post_data);
	},	
	
	// 用户信息校验结果(unlock) 20201
	check_user_info_result_202:function(extend_data, result_data)
	{
		/*
			返回值：JSON
			成功：{"state":1,"data":{"user_id":'xxxx'}}
			失败：{"state":0,"message":"token\u8fc7\u671f"}
		*/
		
		var endTime = new Date().getTime();
		var costTime = endTime - extend_data.start_time;
		console.log('costTime=%s,token:%s',costTime,extend_data.token);		
		
		console.log('result_data:%s', result_data);
		
		result_data = JSON.parse(result_data);
						
		// 检测是否失败
		if(result_data.state != 1)
		{
			console.log('用户信息校验失败(unlock)[message:%d]', result_data.message);
			send_errro_to_gameserver(extend_data.res, extend_data.account);
			return;
		}
		
		var userinfo = result_data.data;
		
		var userID = userinfo.user_id.toString();
		
		//console.log('userinfo.user_id:%s', userID);
		
		send_result_to_gameserver(extend_data.res, userID);	
	},	
	
	
	// 处理检测用户信息(辉耀安卓) 11801
	do_check_user_info_118:function(extend_data)
	{
		/*
			接口地址 : http://u9.huiyaohuyu.cn/api/validateGameLogin
			传参方式： GET	
			UserId string Y U9 用户 id
			Token string Y 登录 toekn
		*/
		
		var url = 'http://u9.huiyaohuyu.cn/api/validateGameLogin?';
												
		var userID = extend_data.account;
		var token = extend_data.token;												
																							
		var params = util.format('UserId=%s&Token=%s',userID, token);	
		
		var call_url = url + params;
		
		console.log('call_url:%s', call_url);
		
		http_get(call_url, extend_data, http);
	},	
	
	// 用户信息校验结果(辉耀安卓) 11801
	check_user_info_result_118:function(extend_data, result_data)
	{
		/*
			Code : int 
			Message : string
			返回结果： 返回结果：
			{
				"Code": 0,
				"Message": "success"
			}
		*/
		
		console.log('result_data:%s', result_data);
		
		result_data = JSON.parse(result_data);
						
		// 检测是否失败
		if(result_data.Code != 0)
		{
			console.log('用户信息校验失败(辉耀广分安卓)[Code:%d]', result_data.Code);
			return;
		}
				
		send_result_to_gameserver(extend_data.res, extend_data.account);	
	},	

	// 处理检测用户信息(辉耀ios) 11901
	do_check_user_info_119:function(extend_data)
	{
		/*
			方法：post   [http://api.huiyaohuyu.cn/user/checkToken](http://api.huiyaohuyu.cn/) 
			输入
			| 参数    | 描述        |
			| ----- | --------- |
			| guid  | hy登录态标识   |
			| token | Hy登录token |
			|       |           |
		*/
		
		var url = 'http://api.huiyaohuyu.cn/user/checkToken';
												
		var userID = extend_data.account;
		var token = extend_data.token;												
																							
		var post_data = {
			guid:extend_data.account,
			token:extend_data.token
		};

		http_post(url, extend_data, http, post_data);
	},	

	// 用户信息校验结果(辉耀ios) 11901
	check_user_info_result_119:function(extend_data, result_data)
	{
		/*
			| 参数        | 描述                |
			| --------- | ----------------- |
			| status    | 0 登录态有效，其他  登录态失效 |
			| message   |                   |
			| count     |                   |
			| data      |                   |
			| data_list |                   |
		*/
		
		console.log('result_data:%s', result_data);
		
		result_data = JSON.parse(result_data);
						
		// 检测是否失败
		if(result_data.status != 0)
		{
			console.log('用户信息校验失败(辉耀ios)[status:%d]', result_data.status);
			return;
		}
				
		send_result_to_gameserver(extend_data.res, extend_data.account);
	},	
	
	
	// 处理检测用户信息(遨游) 12201
	do_check_user_info_122:function(extend_data)
	{
		/*
		接口地址：https://api.weilongwl.com:8443/web_api/v2/open/validSign
		请求方式：HTTPS POST
		请求参数：项	描述	备注
		uid	用户id，由客户端接口在登录成功回调获取	long型
		sign	登录签名，由客户端接口在登录成功回调获取	字符串型

		*/
		
		var url = 'https://api.weilongwl.com:8443/web_api/v2/open/validSign?';
												
		var userID = extend_data.account;
		var token = extend_data.token;		
		
		var params = util.format('uid=%s&sign=%s',userID, token);			
		var call_url = url + params;	
																							
		var post_data = {
			//uid:extend_data.account,
			//sign:extend_data.token
		};
		
		console.log('call_url:%s', call_url);

		https_post_ex(call_url, extend_data, https, post_data);
	},		
	
	// 用户信息校验结果(遨游) 12201
	check_user_info_result_122:function(extend_data, result_data)
	{
		/*
			返回结果：验证通过返回1，验证失败返回0
		*/
		
		console.log('result_data:%d', result_data);
		
		//result_data = JSON.parse(result_data);
						
		// 检测是否失败
		if(result_data != 1)
		{
			console.log('用户信息校验失败(遨游)[%d]', result_data);
			return;
		}
				
		send_result_to_gameserver(extend_data.res, extend_data.account);
	},		
	
		// 处理检测用户信息(乐次元android) 12301
	do_check_user_info_123:function(extend_data)
	{
		/*
			http://www.clgame.com.cn:8089/user/verifyAccount  
			请求方式：HTTP POST
			userID	(整型)上面登录认证成功之后，SDKServer返回的userID（并不是SdkUserID）
			token	登录认证成功之后，SDKServer返回的token
			sign	md5("userID="+userID+"token="+token+appKey);
		*/
		
		var url = 'http://www.clgame.com.cn:8089/user/verifyAccount';
												
		var userID = extend_data.account;
		var token = extend_data.token;
		var appKey = "71e1380afdc27842ae5365ad42fc0d82"; // old_key: 4d28e1b0312b059a4362e346e7e39e67 
		
		var str_for_sign = util.format('userID=%stoken=%s%s',userID,token,appKey);
		console.log('str_for_sign:%s', str_for_sign);		
		var md5_str = md5.encrypt(str_for_sign);
																							
		var post_data = {
			userID:userID,
			token:token,
			sign:md5_str
		};
		
		console.log('call_url:%s', url);

		http_post(url, extend_data, http, post_data);
	},	
	
	// 用户信息校验结果(乐次元android) 12301
	check_user_info_result_123:function(extend_data, result_data)
	{
		/*
			state	1(登录认证成功)；其他失败
			data	认证成功才有数据，否则为空格式如下(JSON):{ userID:SDKServer生的唯一用户IDusername:SDKServer生成的统一格式的用户名}
			
			{
				"state": 1,
				"data": {
					" userID ": 10001,
					" username ": "10001.war3”
				}
			}
		*/
		
		
		result_data = JSON.parse(result_data);
		
						
		// 检测是否失败
		if(result_data.state != 1)
		{
			console.log('用户信息校验失败(乐次元android)[state:%d]', result_data.state);
			return;
		}
		
		var userID = result_data.data.userID.toString();
		
		console.log('用户登录成功(乐次元android)[userID:%s]', userID);
				
		send_result_to_gameserver(extend_data.res, userID);
	},		
	
	
	
	// 处理检测用户信息(翔通bt1) 20401
	do_check_user_info_204:function(extend_data)
	{
		/*
			// 验证地址：'http://oss.vanggame.com:8080/user/verifyAccount?';
			string strAppKey = "39a723159ac8597bfc766c0f5aefd25f";
			char szSign[256] = { 0 }; // Md5签名值（32位小写）md5("userID=" + userID + "token=" + token + appKey)，
			sprintf_s(szSign, sizeof(szSign), "userID=%stoken=%s%s", pRecv->m_szAccount, pRecv->m_szToken, strAppKey.c_str());

			MD5::Digest md5_sign = MD5::Encode(szSign);
			string strAlreadySignStr = md5_sign.HexString();
		*/
		
		var url = 'http://oss.vanggame.com:8080/user/verifyAccount?';	
		
		// appkey:9f0e5c67bb9f76d6970a9dceb4dc8bb7
		// AppSecret: 039e9668b3d27a69b5413944a4f62f3d
		
		// appId: 61
		// appName: 超时空联盟bt（IOS）
		//appKey: 3d7c4b48d3a674a64f49ad1ea1c356ea
		//appSecret: 2a2f66ec481754e01a64be69c04c6124		
		var app_key_list = [];  
		app_key_list[20401] = '9f0e5c67bb9f76d6970a9dceb4dc8bb7';	// android
		app_key_list[20402] = '3d7c4b48d3a674a64f49ad1ea1c356ea';	// ios
		
		var appkey = app_key_list[extend_data.cp_channel_type]; 
		var userID = extend_data.account;
		var token = extend_data.token;
		
		var str_for_sign = util.format('userID=%stoken=%s%s',userID, token, appkey);	
		var md5_str = md5.encrypt(str_for_sign);
								
		var params = util.format('userID=%s&token=%s&sign=%s',userID, token, md5_str);	
		
		var call_url = url + params;
		
		console.log('call_url:%s', call_url);
		
		http_get(call_url, extend_data, http);
	},
	
	// 用户信息校验结果(翔通bt1) 20401
	check_user_info_result_204:function(extend_data, result_data)
	{
		/*
			返回(JSON格式)：
			{
				state: 1(登录认证成功)；其他失败，查看失败状态码说明
				data: 认证成功才有数据，否则为空
				{
				userID:V8Server 生成的唯一用户ID
				username:V8Server 生成的统一格式的用户名
				}
			}
		*/
			
		console.log('result_data:%s', result_data);
		
		result_data = JSON.parse(result_data);
		
		// 检测是否失败
		if(result_data.state != 1)
		{
			console.log('用户信息校验失败(翔通bt1)[state:%d]', result_data.state);
			return;
		}
		
		var userinfo = result_data.data;
		
		var userID = userinfo.userID.toString();
		
		console.log('userID:%s', userID);
		
		send_result_to_gameserver(extend_data.res, userID);	
	},		
	
	// 处理检测用户信息(翔通bt2) 20501
	do_check_user_info_205:function(extend_data)
	{
		/*
			// 验证地址：'http://oss.vanggame.com:8080/user/verifyAccount?';
			string strAppKey = "39a723159ac8597bfc766c0f5aefd25f";
			char szSign[256] = { 0 }; // Md5签名值（32位小写）md5("userID=" + userID + "token=" + token + appKey)，
			sprintf_s(szSign, sizeof(szSign), "userID=%stoken=%s%s", pRecv->m_szAccount, pRecv->m_szToken, strAppKey.c_str());

			MD5::Digest md5_sign = MD5::Encode(szSign);
			string strAlreadySignStr = md5_sign.HexString();
		*/
		
		var url = 'http://oss.vanggame.com:8080/user/verifyAccount?';	
		
		// appkey:cd867d5129ec189431f216edf4a4ded4
		// AppSecret: 2b498a51a613897f7bf7a02d916f8bc1	
		
		// appId: 63
		// appName: 超时空联盟超V(IOS)
		// appKey: c10395be9e76d7ff5390e9b529c174ad
		// appSecret: 28e9bfbc4195704403bd2dd02bcc8e79
		var app_key_list = [];  
		app_key_list[20501] = 'cd867d5129ec189431f216edf4a4ded4';	// android
		app_key_list[20502] = 'c10395be9e76d7ff5390e9b529c174ad';	// ios
		
		var appkey = app_key_list[extend_data.cp_channel_type]; 
		var userID = extend_data.account;
		var token = extend_data.token;
		
		var str_for_sign = util.format('userID=%stoken=%s%s',userID, token, appkey);	
		var md5_str = md5.encrypt(str_for_sign);
								
		var params = util.format('userID=%s&token=%s&sign=%s',userID, token, md5_str);	
		
		var call_url = url + params;
		
		console.log('call_url:%s', call_url);
		
		http_get(call_url, extend_data, http);
	},
	
	// 用户信息校验结果(翔通bt2) 20501
	check_user_info_result_205:function(extend_data, result_data)
	{
		/*
			返回(JSON格式)：
			{
				state: 1(登录认证成功)；其他失败，查看失败状态码说明
				data: 认证成功才有数据，否则为空
				{
				userID:V8Server 生成的唯一用户ID
				username:V8Server 生成的统一格式的用户名
				}
			}
		*/
			
		console.log('result_data:%s', result_data);
		
		result_data = JSON.parse(result_data);
		
		// 检测是否失败
		if(result_data.state != 1)
		{
			console.log('用户信息校验失败(翔通bt2)[state:%d]', result_data.state);
			return;
		}
		
		var userinfo = result_data.data;
		
		var userID = userinfo.userID.toString();
		
		console.log('userID:%s', userID);
		
		send_result_to_gameserver(extend_data.res, userID);	
	},		

	// 处理检测用户信息(翔通bt3) 20601
	do_check_user_info_206:function(extend_data)
	{
		/*
			// 验证地址：'http://oss.vanggame.com:8080/user/verifyAccount?';
			string strAppKey = "39a723159ac8597bfc766c0f5aefd25f";
			char szSign[256] = { 0 }; // Md5签名值（32位小写）md5("userID=" + userID + "token=" + token + appKey)，
			sprintf_s(szSign, sizeof(szSign), "userID=%stoken=%s%s", pRecv->m_szAccount, pRecv->m_szToken, strAppKey.c_str());

			MD5::Digest md5_sign = MD5::Encode(szSign);
			string strAlreadySignStr = md5_sign.HexString();
		*/
		
		var url = 'http://oss.vanggame.com:8080/user/verifyAccount?';	
		
		// appkey:4bebd9aa2e23e4c52f711ac44c5d33dd
		// AppSecret: 61224a2c7456e9c3ee5ab59e817f1991
		
		// appId: 62
		// appName: 超时空联盟满V(IOS)
		// appKey: ece90c7b20c5efd10360254ba923a82e
		// appSecret: d5b529cf08afec955d7e40eb49696ccb
		var app_key_list = [];  
		app_key_list[20601] = '4bebd9aa2e23e4c52f711ac44c5d33dd';	// android
		app_key_list[20602] = 'ece90c7b20c5efd10360254ba923a82e';	// ios
		
		var appkey = app_key_list[extend_data.cp_channel_type]; 
		var userID = extend_data.account;
		var token = extend_data.token;
		
		var str_for_sign = util.format('userID=%stoken=%s%s',userID, token, appkey);	
		var md5_str = md5.encrypt(str_for_sign);
								
		var params = util.format('userID=%s&token=%s&sign=%s',userID, token, md5_str);	
		
		var call_url = url + params;
		
		console.log('call_url:%s', call_url);
		
		http_get(call_url, extend_data, http);
	},
	
	// 用户信息校验结果(翔通bt3) 20601
	check_user_info_result_206:function(extend_data, result_data)
	{
		/*
			返回(JSON格式)：
			{
				state: 1(登录认证成功)；其他失败，查看失败状态码说明
				data: 认证成功才有数据，否则为空
				{
				userID:V8Server 生成的唯一用户ID
				username:V8Server 生成的统一格式的用户名
				}
			}
		*/
			
		console.log('result_data:%s', result_data);
		
		result_data = JSON.parse(result_data);
		
		// 检测是否失败
		if(result_data.state != 1)
		{
			console.log('用户信息校验失败(翔通bt3)[state:%d]', result_data.state);
			return;
		}
		
		var userinfo = result_data.data;
		
		var userID = userinfo.userID.toString();
		
		console.log('userID:%s', userID);
		
		send_result_to_gameserver(extend_data.res, userID);	
	},	

	// 处理检测用户信息(云顶ios) 10401
	do_check_user_info_104:function(extend_data)
	{
		/*
			http://ins.app-fame.com/act.aspx?act=118&userid=用户号&usertoken=用户登录令牌&appid=云顶分配的appid
			请求方式: GET
			参数名	必选	类型及范围	说明
			act	是	int	接口代码，固定118
			appid	是	int	云顶分配的应用编号
			userid	是	int	客户端传给你服务端的用户编号
			usertoken	是	string	客户端传给你服务端的用户登录令牌
		*/
		
		var url = 'http://ins.app-fame.com/act.aspx?';
		var act = 118;	
		
		var app_id_list = [];  
		app_id_list[10401] = 6381;	// 魔兽战役OL
		app_id_list[10402] = 6582;	// 超时空部落
		
		var appid = app_id_list[extend_data.cp_channel_type]; 
		var userid = extend_data.account;
		var usertoken = extend_data.token;
		
		var params = util.format('act=%d&userid=%s&usertoken=%s&appid=%s', act, userid, usertoken, appid);			
		var call_url = url + params;
		
		console.log('call_url:%s', call_url);
		
		http_get(call_url, extend_data, http);
	},	
	
	
	// 用户信息校验结果(云顶ios) 10401
	check_user_info_result_104:function(extend_data, result_data)
	{
		/*
			result	int	1 操作成功0 操作失败 
			desc	string	操作结果描述
		*/
			
		console.log('result_data:%s', result_data);
		
		result_data = JSON.parse(result_data);
		
		// 检测是否失败
		if(result_data.result != 1)
		{
			console.log('用户信息校验失败(云顶ios)[result:%d,desc:%s]', result_data.result, result_data.desc);
			return;
		}
		
		var userid = extend_data.account;
				
		send_result_to_gameserver(extend_data.res, userid);	
	},

	// 处理检测用户信息(翔通专服2 anroid and ios) 12401
	do_check_user_info_124:function(extend_data)
	{
		/*
			// 验证地址：http://vserver.cyjj8.com:8080/user/verifyAccount
			string strAppKey = "39a723159ac8597bfc766c0f5aefd25f";
			char szSign[256] = { 0 }; // Md5签名值（32位小写）md5("userID=" + userID + "token=" + token + appKey)，
			sprintf_s(szSign, sizeof(szSign), "userID=%stoken=%s%s", pRecv->m_szAccount, pRecv->m_szToken, strAppKey.c_str());

			MD5::Digest md5_sign = MD5::Encode(szSign);
			string strAlreadySignStr = md5_sign.HexString();
			
			操戈群雄iOS
			AppID：60
			AppKey：	b9e130dd00daba10ebb514d5b9cbf8f6
			AppSecret：	f2e8eb96a3f3751862f5ca0ec1bbe736


			操戈群雄 安卓
			AppID：61
			AppKey：	8e7747b5ca053110326d3980b93fac15
			AppSecret：	2781636b0fbc64d50674c0f29b50788d		
		*/
		
		var url = 'http://vserver.cyjj8.com:8080/user/verifyAccount?'; 
		
		
		var app_key_list = [];  
		app_key_list[12401] = '8e7747b5ca053110326d3980b93fac15';	// android  
		app_key_list[12410] = 'b9e130dd00daba10ebb514d5b9cbf8f6';	// ios
		
		var appkey = app_key_list[extend_data.cp_channel_type]; 
		var userID = extend_data.account;
		var token = extend_data.token;
		
		var str_for_sign = util.format('userID=%stoken=%s%s',userID, token, appkey);			
		var md5_str = md5.encrypt(str_for_sign);
								
		var params = util.format('userID=%s&token=%s&sign=%s',userID, token, md5_str);	
		
		var call_url = url + params;
		
		console.log('call_url:%s', call_url);
		
		http_get(call_url, extend_data, http);
	},
	
	// 用户信息校验结果(翔通专服2 anroid and ios) 12401
	check_user_info_result_124:function(extend_data, result_data)
	{
		/*
			返回(JSON格式)：
			{
				state: 1(登录认证成功)；其他失败，查看失败状态码说明
				data: 认证成功才有数据，否则为空
				{
				userID:V8Server 生成的唯一用户ID
				username:V8Server 生成的统一格式的用户名
				}
			}
		*/
			
		console.log('result_data:%s', result_data);
		
		result_data = JSON.parse(result_data);
		
		// 检测是否失败
		if(result_data.state != 1)
		{
			console.log('用户信息校验失败(翔通专服anroid)[state:%d]', result_data.state);
			return;
		}
		
		var userinfo = result_data.data;
		
		var userID = userinfo.userID.toString();
		
		console.log('userID:%s', userID);
		
		send_result_to_gameserver(extend_data.res, userID);	
	},			
	
	// 处理检测用户信息(不凡ios联运) 12501
	do_check_user_info_125:function(extend_data)
	{
		/*
			验证地址：https://ios-sdk.binglue.com/h5s/uid
			传输类型	GET

			参数名		类型	必填	说明
			AppId		Int		Y		APPID
			Token		String	Y		用户Token
			Ip			String	Y		客户端IP
			Timestamp	Int		Y		时间戳
			Sign		String	Y		数据签名	

			MD5（AppId + Token + Ip + Timestamp + APPKEY）
		*/
		
		var url = 'https://ios-sdk.binglue.com/h5s/uid?'; 
		
		var app_id_list = [];  
		app_id_list[12501] = 245;	
		app_id_list[12502] = 532;	
		
		var app_key_list = [];  
		app_key_list[12501] = '59fc7b55b78a82a4fc343da0055d04f7';
		app_key_list[12502] = 'b80aa3a0eb85c8bdb60900699cb92cbc';
		
		var AppId = app_id_list[extend_data.cp_channel_type]; 
		var AppKey = app_key_list[extend_data.cp_channel_type]; 
		
		var Token = extend_data.token;
		var Ip = extend_data.ip;
		var Timestamp =  parseInt(Date.now() / 1000);
		
		var str_for_sign = util.format('%d%s%s%d%s',AppId, Token, Ip, Timestamp, AppKey);	
		var sign = md5.encrypt(str_for_sign);
								
		var params = util.format('AppId=%d&Token=%s&Ip=%s&Timestamp=%d&Sign=%s',AppId, Token, Ip, Timestamp, sign);	
		
		var call_url = url + params;
		
		console.log('call_url:%s', call_url);
		
		http_get(call_url, extend_data, https);
	},
	
	// 用户信息校验结果(不凡ios联运) 12501
	check_user_info_result_125:function(extend_data, result_data)
	{
		/*
			参数名		类型	说明
			ResultCode	Int		结果编号 1为成功 其他为失败
			ResultMsg	String	错误信息
			Uid			Int		用户ID
		*/
			
		console.log('result_data:%s', result_data);
		
		result_data = JSON.parse(result_data);
		
		// 检测是否失败
		if(result_data.ResultCode != 1)
		{
			console.log('用户信息校验失败(不凡ios联运)[Code:%d,msg:%s]', result_data.ResultCode, result_data.ResultMsg);
			return;
		}
		
		var Uid = result_data.Uid.toString();
		
		send_result_to_gameserver(extend_data.res, Uid);	
	},	

	// 处理检测用户信息(ygame) 12601
	do_check_user_info_126:function(extend_data)
	{
		/*
			Method: HTTP POST 
			URL: https://api.ygame.vn/v1/user/user_info
				
			| Name        | Description           | Note           |
			| ------------- |:-------------:|:-------------:|
			| accessToken      | Get from YKit system ||
			| appId      | The id of your application is issued by YGame      ||
			| sign | Md5 (accessToken + appId + appKey) |appKey: the secret key of your application is issued by YGame|
			
			android:
			app_id:     300c49a75df7578b1579644873d687d7
			app_secret: 2c8b753932b4ce7fe69453d1c4e16b94

			ios:
			app_id:     62da21fa67f53be07c105c676cfa097d
			app_secret: 359a4cf001d014d0973638526bc89ab0			
		*/
		
		var url = 'https://api.ygame.vn/v1/user/user_info?';  // fot live
		//var url = 'https://dev.ygame.vn/v1/user/user_info?'; 	// for test
		
		var app_id_list = [];  
		app_id_list[12601] = '300c49a75df7578b1579644873d687d7';	// android
		app_id_list[12610] = '62da21fa67f53be07c105c676cfa097d';	// ios
		
		var app_key_list = [];  
		app_key_list[12601] = '2c8b753932b4ce7fe69453d1c4e16b94';	// android
		app_key_list[12610] = '359a4cf001d014d0973638526bc89ab0';	// ios
		
		var AppId = app_id_list[extend_data.cp_channel_type]; 
		var AppKey = app_key_list[extend_data.cp_channel_type]; 
		
		var Token = extend_data.token;
		
		// Md5 (accessToken + appId + appKey)
		var str_for_sign = util.format('%s%s%s',Token, AppId, AppKey);	
		var sign = md5.encrypt(str_for_sign);
								
		var params = util.format('accessToken=%s&appId=%s&sign=%s',Token, AppId, sign);	
		
		var call_url = url + params;
		
		console.log('call_url:%s', call_url);
		
		http_get(call_url, extend_data, https);
	},

	// 用户信息校验结果(YGame) 12601
	check_user_info_result_126:function(extend_data, result_data)
	{
		/*
			A successful response will contain your user information in JSON format:
			json
			{
				"status":1,
				"msg":"",
				"data":{
					"userId":123
					"username":"mario",
					"dateCreate":"2017-09-20 14:59:28",
					"dateUpdate":"2017-09-21 14:59:28",
					"accessToken":"a0a6e4e4caebb1d799b7b49ed45ff71c",
					"accessExpires":1474444768875,	
				}
			}
		*/
			
		console.log('result_data:%s', result_data);
		
		result_data = JSON.parse(result_data);
		
		// 检测是否失败
		if(result_data.status != 1)
		{
			console.log('Check User Info Fail(YGame)[Code:%d,msg:%s]', result_data.status, result_data.msg);
			return;
		}
		
		var userId = result_data.data.userId.toString();
		
		send_result_to_gameserver(extend_data.res, userId);	
	},	


	// 处理检测用户信息(未来互动) 12701
	do_check_user_info_127:function(extend_data)
	{
		/*
			Method: HTTP POST
			URL: https://api.91fun.com/api/cp/user/check
				
			app_id	是	STRING	游戏接入时分配的应用app_id
			mem_id	是	STRING	登陆时提供给CP的用户ID
			user_token	是	STRING	登陆获取的user_token
			sign	是	STRING	参数校验
			sign 的签名规则：md5(app_id=...&mem_id=...&user_token=...&app_key=...)
		*/
		
		var url = 'https://api.91fun.com/api/cp/user/check'; 
		
		var app_id_list = [];  
		app_id_list[12701] = 6069;
		app_id_list[12702] = 6073;
		
		var app_key_list = [];  
		app_key_list[12701] = '78e5f996fa5fbcf4761ebacb244301e9';
		app_key_list[12702] = 'a679fd85e389d758692358a1deb94350';		
		
		var app_id = app_id_list[extend_data.cp_channel_type]; 
		var app_key = app_key_list[extend_data.cp_channel_type]; 
		
		var user_token = extend_data.token;
		var mem_id = extend_data.account;
		
		// sign=md5(app_id=1&mem_id=23&user_token=rkmi2huqu9dv6750g5os11ilv2&app_key=de933fdbede098c62cb309443c3cf251)
		var str_for_sign = util.format('app_id=%s&mem_id=%s&user_token=%s&app_key=%s',app_id, mem_id, user_token, app_key);	
		var sign = md5.encrypt(str_for_sign);
										
		var call_url = url;
		
		console.log('call_url:%s', call_url);
		
		//http_get(call_url, extend_data, https);
		
		var post_data = {
			app_id:app_id,
			mem_id:mem_id,
			user_token:user_token,
			sign:sign
		};
		https_post(url, extend_data, https, post_data);		
	},	
		
		
	// 用户信息校验结果(未来互动) 12701
	check_user_info_result_127:function(extend_data, result_data)
	{
		/*
			{
			  "status": "0",
			  "msg": "请求参数为空 app_id"
			}
			status	STRING	返回状态码
			msg	STRING	返回信息					
		*/
			
		console.log('result_data:%s', result_data);
		
		result_data = JSON.parse(result_data);
		
		// 检测是否失败
		if(result_data.status != 0)
		{
			console.log('用户信息校验失败(未来互动)[status:%d,msg:%s]', result_data.status, result_data.msg);
			return;
		}
		
		var account = extend_data.account;
		
		send_result_to_gameserver(extend_data.res, account);	
	},			
	
	
	// 处理检测用户信息(funpax) 30001-30099
	do_check_user_info_300:function(extend_data)
	{
		/*
			Method:  post/get
			URL: http://47.75.231.164/webapi/checkUserInfo
				
			参数	必要性	说明
			uid	必传	从客户端登录回调中获取的uid
			token	必传	从客户端登录回调中获取的token，请注意token的长度不要被截断

		*/
		
		var url = 'http://47.75.231.164/webapi/checkUserInfo?'; 
		
		var params = util.format('uid=%s&token=%s',extend_data.account, extend_data.token);	
										
		var call_url = url + params;
		
		//console.log('call_url:%s', call_url);
		
		http_get(call_url, extend_data, http);
	},	
	
	// 用户信息校验结果(funpax) 30001-30099
	check_user_info_result_300:function(extend_data, result_data)
	{
		/*
			参数	类型	必要性	说明
			status	bool	必有	接口验证状态，若通过验证为true，否则为false
			message	string	必有	Status为false时，message有值，为错误提示语
		
			接口会返回JSON格式字符串，如：
			例1 {"status":false,"message":"tokenUidError"}
			例2 {"status":true,"message":""}		
		*/
			
		//console.log('result_data:%s', result_data);
		
		result_data = JSON.parse(result_data);
		
		// 检测是否失败
		if(result_data.status != true)
		{
			console.log('用户信息校验失败(funpax)[status:%s,message:%s]', result_data.status, result_data.message);
			return;
		}
		
		var account = extend_data.account;
		
		send_result_to_gameserver(extend_data.res, account);	
	},	

	// 处理检测用户信息(乐次元ios) 12801-12899
	do_check_user_info_128:function(extend_data)
	{
		/*
			Method:  HTTP POST
			URL: http://sdkapi.clgame.com.cn:2018/rest/sdk/verifyToken
			token	是	STRING	游戏接入时分配返回的唯一标识token值
		*/
		
		var url = 'http://sdkapi.clgame.com.cn:2018/rest/sdk/verifyToken'; 
		
		var post_data = {
			token:extend_data.token
		};
		http_post(url, extend_data, http, post_data);		
	},	

	// 用户信息校验结果(乐次元ios) 12801-12899
	check_user_info_result_128:function(extend_data, result_data)
	{
		/*
			状态码	类型	说明
			result	STRING	返回状态码
			msg	STRING	返回信息
			
			{
			  "result": "0",
			  "msg": "token不合法"
			}
		*/
			
		console.log('result_data:%s', result_data);
		
		result_data = JSON.parse(result_data);
		
		// 检测是否失败
		if(result_data.result != 1)
		{
			console.log('用户信息校验失败(乐次元ios)[result:%s,msg:%s]', result_data.result, result_data.msg);
			return;
		}
		
		var account = extend_data.account;
		
		send_result_to_gameserver(extend_data.res, account);	
	},	
	
	// 处理检测用户信息(乐次元_350_android) 12901-12999
	do_check_user_info_129:function(extend_data)
	{
		/*
			Method:  POST或GET
			URL: http://sdkapi.350.com/user/verifyAccount/

			参数	描述
			userID	登录认证成功之后，返回的userID
			token	SDK登录认证成功之后，返回到游戏前端的token
			sign	md5("userID="+userID+"token="+token+appKey);这里游戏服务器按照格式，生成一个md5串，appKey是分配给游戏的AppKey
			appid	游戏的game_id
			
			
		*/
		
		var url = 'http://sdkapi.350.com/user/verifyAccount/'; 
		
		var userID = extend_data.account;
		var token = extend_data.token;
		var appid = 400;
		var appKey = 'yWpx3hWQHFhSnTCj#400#6KuRKuaAjLJ5sYRy';
		
		var str_for_sign = util.format('userID=%stoken=%s%s',userID, token, appKey);	
		var sign = md5.encrypt(str_for_sign);	
		
		var post_data = {
			userID:userID,
			token:token,
			sign:sign,
			appid:appid
		};
		http_post(url, extend_data, http, post_data);		
	},

	
	// 用户信息校验结果(乐次元_350_android) 12901-12999
	check_user_info_result_129:function(extend_data, result_data)
	{
		/*
			{
			  state: 1(登录认证成功)；其他失败
			  data: 认证成功才有数据，否则为空
				  {
					  userID:唯一用户ID
					  username:用户名
				  }
			 }
		*/
			
		console.log('result_data:%s', result_data);
		
		result_data = JSON.parse(result_data);
		
		// 检测是否失败
		if(result_data.state != 1)
		{
			console.log('用户信息校验失败(乐次元_150_android)[state:%s]', result_data.state);
			return;
		}
		
		var account = result_data.data.userID;
		
		send_result_to_gameserver(extend_data.res, account);	
	},		
	
	// todo::新添加的接口往下走....
	
	


///////////////////////////// 结束位置
}






