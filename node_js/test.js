
var https = require('https');
var zlib = require('zlib');
var http = require('http');

var express = require('express');
var app = express();
var path = require('path');
var fs = require('fs');
var util = require('util');
//var bodyParser = require('body-parser');
var dateformat = require('./utils/dateformat');
//var log4j = require('./utils/log4j');
var common_logic = require('./common_logic');
var common_dao = require('./dao/common_dao');
var Md5 = require('./utils/Md5');
var md5=new Md5();

var config = require('./utils/config');


app.use('/skyx', express.static(__dirname + '/public/skyx'));
//app.use(bodyParser.urlencoded({ extended: true }));


var server = app.listen(3333, function () {
  var host = server.address().address;
  var port = server.address().port;

  console.log('Example app listening at http://%s:%s', host, port);
   console.log('__dirname:%s', __dirname);
  // console.log('cur_time:%s', getDate); 
});


app.get('/redirect_test', function (req, response) {
	console.log('redirect_test be call!(time %s)',Date.now());
	
	var options = {
		host: 'www.baidu.com', // 这里是代理服务器       
		port: 8080, // 这里是代理服务器端口 
		path: '',       
		method: 'get',
		headers: {
		  // 如果代理服务器需要认证
		  'Proxy-Authentication': 'Base ' + new Buffer('user:password').toString('base64') // 替换为代理服务器用户名和密码
		}       
	};

	var req = http.request(options, function(req, res) {
		res.pipe(response); // 这个pipe很喜欢
		console.log(req.url);
	}).end();	
	
});

/*
var server = http.createServer(function (req, res) {
  //这里有个错误，params 是 undefined
  var ok = req.params.ok;
  res.writeHead(200, {'Content-Type':'text/plain'});
  res.end('Hello World');
});
server.listen(8080, '127.0.0.1');
console.log('Server running at http://127.0.0.1:8080/');
*/

function test_call_back_function(req, res){
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    console.log('test_call_back_function be call!(%s)',cur_time);			
	//console.log('收到请求111111:%s',Date.now());
	res.send('大家好，这里是蟑螂科技首页!! I want to fuck you!');
}

// 测试接口 
app.get('/test_ex', test_call_back_function);


//app.post(path, callback [, callback ...])

//app.set('title', 'My Site');
//app.get('title');

var user_dao = require('./dao/userdao');

//var MixCrypto = require('../utils/MixCrypto');
//var mixCrypto = new MixCrypto({});

app.get('/test', function (req, res) {
  console.log('test function is be call!(time %s)',Date.now());
	var result = user_dao.test_get_user_data(1234).then(function (result) {
        if (result.length > 0) {
            console.log(result[0].role_id);
			//var name=mixCrypto.encrypt(result[0].name);
			console.log(result[0].name);
			console.log(result[0].age);
			res.send(result[0].name);
        } 
    });
});

app.get('/get_test', function (req, res) {
  console.log('post function is be call!(time %s)',Date.now());
  //res.send('this is get test!!!');
  
  	var server_config = JSON.parse(fs.readFileSync('./public/skyx/develop/server_config_pin.json','utf-8'));
		
	var szParam = {
	code:200,
	str:"操作成功",
	msg:server_config
	};
	
	res.send(szParam);
});


function https_post_test()
{
	var random = 7878;
	//var phoneNumber = 13632772857;
	//var ext = req.query.ext;
	//var templId = 16247;
	var appid = 1400028903;
	//var appkey = 'ca6a1c9327c53603a50fc056c15c4c18';
	//var redirect_url = url+'?random='+random+'&phoneNumber='+phoneNumber+'&ext='+ext+'&templId='+templId+'&appid='+appid+'&appkey='+appkey;	
	
	var post_data='sdkappid='+appid+'&random='+random;//请求数据
	var reqdata = JSON.stringify(post_data);
	var options = {
		hostname: '119.254.237.50',
		port: '443',
		path: '/sdk/user/auth.do',
		method: 'POST',
		rejectUnauthorized: false,
		requestCert: true,
		auth: 'admin:123456************',
		headers: {
			'username': 'admin',
			'password': '123456************',
			'Cookie': 'locale=zh_CN',
			'X-BuildTime': '2015-01-01 20:04:11',
			'Autologin': '4',
			'Accept-Encoding': 'gzip, deflate',
			'X-Timeout': '3600000',
			'Content-Type': 'Application/json',
			"Content-Length":reqdata.length
		}
	};
	var req = https.request(options, function (res) {
	});
	req.write(reqdata);
	req.on('response', function (response) {
		switch (response.headers['content-encoding']) {
			case 'gzip':
				var body = '';
				var gunzip = zlib.createGunzip();
				response.pipe(gunzip);
				gunzip.on('data', function (data) {
					body += data;
				});
				gunzip.on('end', function () {
					var returndatatojson= JSON.parse(body);
					req.end();
				});
				gunzip.on('error', function (e) {
					console.log('error' + e.toString());
					req.end();
				});
				break;
			case 'deflate':
				var output = fs.createWriteStream("d:temp.txt");
				response.pipe(zlib.createInflate()).pipe(output);
				req.end();
				break;
			default:
				var body = "";  
				response.on('data', function (data) { body += data; })  
                        .on('end', function () { console.log('body='+body); });  				
				console.log('body='+body);	 // for test res.send(200, body);
				req.end();
				break;
		}
	});
	req.on('error', function (e) {
		console.log('error');	
		console.log(new Error('problem with request: ' + e.message));
		req.end();
		//setTimeout(cb, 10);
	});

	console.log('https_post_test');	
}


// 发送消息到游戏服务器
function SendMsgToGameServer(url, res){
	//console.log("Send2GS:" + url);
	var client = res;
	http.get(url, function (res) {
        var list = [];
        var len = 0;
        res.on('data', function (data) {
            list.push(data);
            len += data.length;
        });
        res.on('end', function () {
            var _data = Buffer.concat(list, len).toString("utf-8");
			client.send(_data);
			console.log("resp_context:" + _data);
			//SendMsgToUser(client,_data,nMsgId);
        });
    }).on('error', function (e) {
		console.log('发送消息到游戏服务器出错:'+e);
        //defered.reject(e);
    });		
}

function call_php_func(url)
{
	http.get(url, function (res) {
        var list = [];
        var len = 0;
        res.on('data', function (data) {
            list.push(data);
            len += data.length;
        });
        res.on('end', function () {
            var _data = Buffer.concat(list, len).toString("utf-8");
			console.log("call_php_resp:" + _data);
			//client.send(_data);
			return _data;
        });
    }).on('error', function (e) {
		//console.log('发送消息到游戏服务器出错:'+e);
		var error_desc = "发送消息到游戏服务器出错"+e;
		return error_desc;
    });	
}

var crypto = require('crypto');

var bodyParser = require("body-parser");  
app.use(bodyParser.urlencoded({ extended: false })); 

// 测试使用
app.post('/recharge_callback_quicksdk', function (req, res) {
    console.log('post recharge_callback!(time %s)',Date().toLocaleString());	
 
	//var call_game_url = "http://192.168.1.222:40006/recharge_callback_quicksdk";
	//var call_game_url = "http://192.168.1.222:40006/recharge_callback_quicksdk";
	var call_game_url = "http://192.168.1.88:40006/recharge_callback_quicksdk";
	var redirect_url = call_game_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign;	
	SendMsgToGameServer(redirect_url, res);
	
		// 存储充值发货通知数据
	var call_php_url = 'http://192.168.1.111/game_php_util/quick_sdk/save_recharge_data.php';
	var redirect_php_url = call_php_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign;
	//console.log("redirect_php_url:" + redirect_php_url);	
	call_php_func(redirect_php_url);
	
	//console.log("recharge_callback_quicksdk is over");	
	//res.send("SUCCESS");	
});

// 测试使用
app.get('/web_server_test', function (req, res) {
    console.log('post recharge_callback!(time %s)',Date().toLocaleString());
	
	var call_game_url = "http://192.168.1.222:40006/web_server_test";
	var redirect_url = call_game_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign;	
	SendMsgToGameServer(call_game_url, res);	
	
	//res.send("SUCCESS");	
});

// 测试使用
app.get('/save_rechage_data', function (req, res) {
    console.log('save_rechage_data be call !(time %s)',Date().toLocaleString());
	
	var call_game_url = 'http://192.168.1.111/game_php_util/quick_sdk/save_recharge_data.php';
	//var redirect_url = call_game_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign;	
	SendMsgToGameServer(call_game_url, res);	
	
	//res.send("SUCCESS");	
});

function http_post_request_json(req, res, json_data) {  
  
   // var data = {  
    //    address: 'test@test.com',  
   //     subject: "test"  
   // };  
    console.log('json_data='.json_data);  
  
    json_data = JSON.stringify(json_data);  
    console.log(json_data);  
    var opt = {  
        method: "POST",  
        host: "192.168.1.222",  
        port: 40006,  
        path: "/recharge_callback_quicksdk",  
        headers: {  
            "Content-Type": 'application/json',  
            "Content-Length": json_data.length  
        }  
    };  
  
    var req = http.request(opt, function (serverFeedback) {  
        if (serverFeedback.statusCode == 200) {  
            var body = "";  
            serverFeedback.on('data', function (data) { body += data; })  
                          .on('end', function () { console.log('body='.body);  res.send(200, body); });  
        }  
        else {  
            res.send(500, "error");  
        }  
    });  
    req.write(json_data + "\n");  
    req.end();  
}  




// 发送消息到游戏服务器
function test_call_return(){
	//console.log("Send2GS:" + url);
	//var client = res;
	var url = "http://192.168.1.222:2222/msg";
	//var url = "http://192.168.1.111:16888/test";
	//var url = "http://checkuser.sdk.quicksdk.net/v2/checkUserInfo";
	http.get(url, function (res) {
        var list = [];
        var len = 0;
        res.on('data', function (data) {
            list.push(data);
            len += data.length;
			console.log("data:" + data);
        });
        res.on('end', function () {
            var _data = Buffer.concat(list, len).toString("utf-8");
			//client.send(_data);
			console.log("_data:" + _data);
			//SendMsgToUser(client,_data,nMsgId);
        });
    }).on('error', function (e) {
		console.log('发送消息到游戏服务器出错:'+e);
        //defered.reject(e);
    });		
}

//myDate.getYear(); //获取当前年份(2位)
//myDate.getFullYear(); //获取完整的年份(4位,1970-????)
//myDate.getMonth(); //获取当前月份(0-11,0代表1月)         // 所以获取当前月份是myDate.getMonth()+1; 
//myDate.getDate(); //获取当前日(1-31)
//myDate.getDay(); //获取当前星期X(0-6,0代表星期天)
//myDate.getTime(); //获取当前时间(从1970.1.1开始的毫秒数)
//myDate.getHours(); //获取当前小时数(0-23)
//myDate.getMinutes(); //获取当前分钟数(0-59)
//myDate.getSeconds(); //获取当前秒数(0-59)
//myDate.getMilliseconds(); //获取当前毫秒数(0-999)
//myDate.toLocaleDateString(); //获取当前日期

// 获取客户端IP
function get_client_ip_ex(req) {
	
	//return req.headers['x-forwarded-for'] ||
	//req.connection.remoteAddress ||
	//req.socket.remoteAddress ||
	//req.connection.socket.remoteAddress;	
	var src_ip = req.headers['x-forwarded-for'] ||
        req.connection.remoteAddress ||
        req.socket.remoteAddress ||
        req.connection.socket.remoteAddress;
		
	var ip = "127.0.0.1";
	var i = 0;
	var arr = src_ip.split("::ffff:").map(function (val) {
		i++;
		// 取第二个值
		if(i == 2){
			ip = val;
		}
	});	
	return ip;
};

// 获取客户端IP
function get_client_ip(req) {		
	var src_ip = req.headers['x-forwarded-for'] ||
        req.connection.remoteAddress ||
        req.socket.remoteAddress ||
        req.connection.socket.remoteAddress;
		
	var ip = "127.0.0.1";
	var i = 0;
	var arr = src_ip.split("::ffff:").map(function (val) {
		i++;
		// 取第二个值
		if(i == 2){
			ip = val;
		}
	});	
	return ip;		
};

// 测试使用
app.get('/my_test', function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    console.log('welcome be call![time:%s, ip:%s]',cur_time, get_client_ip(req));
	res.send("Hello,Welcome to ZhangLang!");	
});



//console.log('find_result:%s',find_result);

function my_find_str(src, sub, len)
{
	var i = 0;
	var find_result = "fuck";
	var arr = src.split("orderID=").map(function (val) {
		i++;	
		// 取第二个值
		if(i == 2){
			find_result = val;
		}
	});	
	return find_result;
}




app.get('/get_version_url', function (req, res) {
    console.log('get get_version_url!(time %s)',Date().toLocaleString());
	
	var channel_type = req.query.channel_type;
	var sub_channel_type = req.query.sub_channel_type;
	var app_version = req.query.app_version;
	
	var channel_type_ios_ly_xt = 10701; // IOS联运(翔通)
	
	// 翔通子类型列表 index = 翔通子类型
	var xt_sub_type_list = [];
	xt_sub_type_list[0] = { IsInExamine:1, IsSkipUpdate:1};	// 是否在审核中，是否跳过热更
	xt_sub_type_list[1] = { IsInExamine:1, IsSkipUpdate:1};
	xt_sub_type_list[2] = { IsInExamine:1, IsSkipUpdate:1};
	
	
	// 非翔通联运 index = CP渠道类型
	var examine_toggle_list = [];
	examine_toggle_list[10004] = { Version: "1.0.0", IsSkipUpdate:1}; //"1.0.0"; // 审核版本号， 是否跳过热更
	examine_toggle_list[10005] = { Version: "1.0.0", IsSkipUpdate:1};
	examine_toggle_list[10006] = { Version: "1.0.0", IsSkipUpdate:1};
	examine_toggle_list[10007] = { Version: "1.0.0", IsSkipUpdate:1};
	examine_toggle_list[10008] = { Version: "1.0.0", IsSkipUpdate:1};
	examine_toggle_list[10009] = { Version: "1.0.0", IsSkipUpdate:1};
	examine_toggle_list[10101] = { Version: "1.0.0", IsSkipUpdate:1};
	examine_toggle_list[10201] = { Version: "1.0.0", IsSkipUpdate:1};
	examine_toggle_list[10202] = { Version: "1.0.0", IsSkipUpdate:1};
	examine_toggle_list[10301] = { Version: "1.0.0", IsSkipUpdate:1};
	examine_toggle_list[10302] = { Version: "1.0.0", IsSkipUpdate:1};
	examine_toggle_list[10401] = { Version: "1.0.0", IsSkipUpdate:1};
	examine_toggle_list[10501] = { Version: "1.0.0", IsSkipUpdate:1};
	examine_toggle_list[10601] = { Version: "1.0.0", IsSkipUpdate:1};	
		
	var isInExamine = false;
	var isSkipUpate = 0;
	
	// 如果是翔通
	if(channel_type == channel_type_ios_ly_xt)
	{
		var subData = xt_sub_type_list[sub_channel_type];
		isSkipUpate = subData.IsSkipUpdate;
		isInExamine = subData.IsInExamine;	
	}
	// 其他渠道
	else
	{
		var examineData = examine_toggle_list[i];
		isSkipUpate = examineData.IsSkipUpdate;
		
		// 大于等于
		if(app_version >= examineData.Version)
		{
			isInExamine = true;
		}			
	}
	
	var version_url = "";
	
	if(isInExamine)
		version_url = "is in examine";
	else
		version_url = "is not in examine";

	var resp_data = {
		code: 200,
		url : version_url
	}
	res.send(resp_data);	
});

 
 //var encode_name = common_logic.url_encode("KevinChen");
 //console.log('encode_name:%s', encode_name);
 
var resp_data = {
	error_code: 0,
	order_id: 0,
	error_msg: "error"
};		

resp_data.error_code = 78;
resp_data.fuck_you = 'man';

//var json_resp_data 	= JSON.parse(resp_data);

var http = require('http');
var qs = require('querystring');
var content = qs.stringify(resp_data);

//console.log('content:%s', content);
 
 
//log4j.dateFilelogger.error("eeeeeeer" + 111);

 // 钻
var readyToSignStr = 'cpid=31&cporder=1516415394&currency=RMB&fee=168.00&gameid=22&goodsname=1680&model=1&timestamp=1516415393&token=5a61a8fe3a2e05a62a6340bc807357b1d387d7bde1a59b96c468a83b1a_1517019316_1&315a54236f0ce93315a54236f0cedb';
//var encode_url = common_logic.url_encode_ex(readyToSignStr);
var alreadySignStr = common_logic.my_md5(readyToSignStr);
//var alreadySignStr = md5.encrypt(readyToSignStr);

//console.log('alreadySignStr:%s', alreadySignStr)


//※交易成功時間(UTC+8)
//yyyy-mm-ddThr:mi:se(24 小時制)
// UTC() 根据世界时返回 1997 年 1 月 1 日 到指定日期的毫秒数。
var cur_time = dateformat(new Date(),"yyyy-MM-ddThh:mm:ss");

//console.log(new Date());





//call_func_name.f

//call_func_name.function('fuckyou,man', my_printf);

//this.my_test_func_11('fuckyou,man', my_printf);

//var ddd = 11;
//('my_test_func_'+ddd)('fuckyou,man', my_printf);

//[call_func_name]:('fuckyou,man', my_printf);

//var otherfun = 'test'

//[my_test_func_11]('fuckyou,man', my_printf);

//call_func_name('fuckyou,man', my_printf);

//this.['my_test_func_11']('fuckyou,man', my_printf);


var fuck_dongdong ={
	jb : '好小',
	name : 'dongdong',
}

//console.log(fuck_dongdong.jb);
//console.log(fuck_dongdong.name);

//this.my_test_func_11(fuck_dongdong, my_printf);



//console.log(parseInt(22.5));

//common_dao.update_and_get_order_series_id();


common_dao.get_order_series_id_with_update().then(function (result_list) {
	//console.log('result_list.length:%d',result_list.length);
	console.log('result_list:',result_list[0][0]);
	var order_series_id = result_list[0][0].order_series_id;
	console.log('order_series_id',order_series_id);
	if (result_list.length > 0) {
		//var record_data_json = JSON.parse(result_list[1]);	
		//console.log('result_list',record_data_json);
		//console.log('result_list',result_list['RowDataPacket']);
		//for(var i = 0; i < result_list.length; ++i)
		//{
		//	console.log('result_list',result_list[i]);
		//}
	}else{
		console.log('数据为空');
	}
});

var cd_order_id = 'MMS1801240000137716';

/*
common_dao.get_mycard_recharge_record_by_tradeno(cd_order_id).then(function (result_list) {
	//console.log('result_list.length:%d',result_list.length);
	console.log('result_list',result_list);
	if (result_list.length > 0) {
		console.log('result_list.record_time',result_list[0].record_time);
		//console.log('result_list',result_list['RowDataPacket']);
		//for(var i = 0; i < result_list.length; ++i)
		//{
		//	console.log('result_list',result_list[i]);
		//}
	}else{
		console.log('数据为空');
	}
});*/



//setTimeout(function(){  
//	console.log('hello');
//},10000); 

// 发送消息到游戏服务器
function SendMsgToGameServer_EX(url, res){
	//console.log("Send2GS:" + url);
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
			//client.send(_data);
			//console.log("resp_context:" + _data);
			
			if(result == 0){
				console.log("失败:" + result);
			}
			else{
				console.log("成功:" + result);
			}
				
			var json_extend_data = JSON.parse(result);
			
			console.log("成功:" + json_extend_data.a);
			
			//SendMsgToUser(client,_data,nMsgId);
        });
    }).on('error', function (e) {
		console.log('发送消息到游戏服务器出错:'+e);
        //defered.reject(e);
    });		
}


var passwd = common_logic.my_md5('huiyaozf@20180612');
console.log("passwd:%s",passwd);





/*
var call_php_url = 'http://192.168.1.111/game_php_util/iapppay/CheckLogin.php';
var url = call_php_url+'?appid='+'1111'+'&logintoken='+'sdfafa';
console.log(url);
SendMsgToGameServer_EX(url, null);	
*/

var call_php_url = 'http://192.168.1.111/game_php_util/iapppay/trade.php';
var url = call_php_url+'?appid='+'1111'+'&logintoken='+'sdfafa';
//console.log(call_php_url);
//SendMsgToGameServer_EX(call_php_url, null);		
	
	//var redirect_php_url = call_php_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign+'&is_android='+1;
	//save_recharge_log(redirect_php_url);

//http://192.168.1.111:3333/my_test_1?appid="3018063546"&logintoken="sdfasdfse"
	
// 测试使用
app.get('/my_test_1', function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    console.log('my_test_1 be call![time:%s, ip:%s]',cur_time, get_client_ip(req));
	
	var appid = req.query.appid;
	var logintoken = req.query.logintoken;	

	var call_php_url = 'http://127.0.0.1/game_php_util/iapppay/CheckLogin.php';
	var url = call_php_url+'?appid='+appid+'&logintoken='+logintoken;
	SendMsgToGameServer_EX(url, res);
});


var URL = require('url');
var url = 'http://pay2.badambiz.com/api/auth/verify_token';


// http post 请求
function my_http_post_json(url, extend_data, http, post_data){
		
    var content = JSON.stringify(post_data);  
    console.log('content:%s',content);  	
	 
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
		 //check_user_info_result(extend_data, result_data);
	   });
	});
	req.write(content);
	req.end();			
}	  
  
	// '{deviceid:"unknown"}'
var context = {deviceid:'unknown'};
	
var my_context = {
	appid : '6aa85eded277a415dc6a3fe203676ae0',
	context : context
  };


var result_data  = {"data":{"userID":1090860,"username":"1523257429500.vg"},"state":1};

var fuck1 = {"goods_id":0,"cp_channel_type":11601,"cp_order_id":"1523861954","recharge_type":2,"ip":"119.129.120.21","role_id":16551,"recharge_id":2001};

var extension = fuck1;

var extra_params = {
	"goods_id":extension.goods_id,
	"cp_channel_type":extension.cp_channel_type,
	"cp_order_id":extension.cp_order_id,
	"recharge_type":extension.recharge_type,
	"ip":extension.ip,
	"role_id":extension.role_id,
	"recharge_id":extension.recharge_id
};
			


//channelID=22910000&currency=RMB&extension={"recharge_id":2001,"cp_channel_type":11601,"recharge_type":2,"role_id":16551,"cp_order_id":"1523860277","ip":"119.129.120.21","goods_id":0}&gameID=45&money=100&orderID=1519093872049782935&productID=2001&serverID=1&userID=1090860&346847fb53ddb3eafafa51ae240bac8a


//499.00USD15262999691526300002{"goods_id":0,"cp_channel_type":20202,"cp_order_id":"1526299969","recharge_type":2,"ip":"42.3.230.215","role_id":2813,"recharge_id":2002}10013unlock.skfb.2002300钻201805142012512334915079051google2813Douglas(s).1832170141df0eeda15dc5a461b03a024c57a15
var fuck = '499.00USD15262999691526300002{"goods_id":0,"cp_channel_type":20202,"cp_order_id":"1526299969","recharge_type":2,"ip":"42.3.230.215","role_id":2813,"recharge_id":2002}10013unlock.skfb.2002300钻201805142012512334915079051google2813Douglas(s).1832170141df0eeda15dc5a461b03a024c57a15';
fuck = common_logic.my_md5(fuck);
console.log('fuck:%s', fuck);



var return_value_list = [];
for(var i = 0; i < 3; ++i)
{
	//var record_data = result_list[i].data;			
	//console.log('record_data:%s',record_data);
	//var record_data_json = JSON.parse(record_data);			
	// INGAME,KDS1501010000001,MXXXXX0000000001,FacTradeSeq00001,Cust001,150,TWD,2015-01-01T18:18:18<BR>
	// PaymentType,TradeSeq,MyCardTradeNo,FacTradeSeq,CustomerId,Amount,Currency,TradeDateTime		
	return_value_list[i] = util.format('%s,%s,<BR>','kkkk', 'ttttt');
	//console.log('return_value:%s)', return_value);
}
var return_value_list_string = return_value_list.toString();

///console.log('return_value_list_string:%s)', return_value_list_string);


var return_value_list_ex = [];
return_value_list_ex[0] = 'Kevin';
return_value_list_ex[1] = 'Chen';
return_value_list_ex[2] = 'Jim';

var my_value_list = '' ;
for(var i = 0; i < 3; ++i)
{
	my_value_list += return_value_list[i];
}

// 6499
var str_money =  '64.99';
//var ddd = str_money;
//ddd = Math.round(ddd);
ddd = str_money * 100;
//ddd = Math.round(ddd);
console.log(ddd);

//datename.getTimezoneOffset()
//取得当地时间和GMT时间（格林威治时间）的差值，返回值以分钟为单位
//====================
//举例：根据本地时间获得GMT时间和任意时区的时间
d=new Date(); //创建一个Date对象
localTime = d.getTime();
localOffset=d.getTimezoneOffset()*60000; //获得当地时间偏移的毫秒数
utc = localTime + localOffset; //utc即GMT时间
offset =0; //以夏威夷时间为例，东10区
hawaii = utc + (3600000*offset); 
nd = new Date(hawaii); 
console.log('nd:%s)', nd.toLocaleString());
//document.writeln("Hawaii time is " + nd.toLocaleString() + <br>");

var dddd = {
	first_name : 'Chen',
	last_name : 'Kevin'
}

dddd['level'] = 45;

console.log(dddd.level);


/*
common_dao.update_create_role_num(1);
common_dao.update_recharge_amount(1,222);
common_dao.update_recharge_role_num(1);
common_dao.update_create_role_num_ex(3,20180531,1);
common_dao.update_role_active_num(3,20180531);
common_dao.update_new_role_payment_num(3,20180531);
*/

/*
var spawn = require('child_process').spawn;
var server1 = null;
function startServer(){
    console.log('start server');
    server1 = spawn('node',['app.js']);
    console.log('node js pid is '+server1.pid);
    server1.on('close',function(code,signal){
        server1.kill(signal);
        server1 = startServer();
    });
    server1.on('error',function(code,signal){
        server1.kill(signal);
        server1 = startServer();
    });
    return server1;
};
startServer();*/

//var content = JSON.stringify(post_data);  
//console.log('content:%s',content);  	

//var ttt = '{"amount":"6","sign":"be868065f181c5890d407bb3036e1716","appId":"gzc2cb557eb0f9946f","status":"SUCCESS","sandbox":"1","payWay":"4","uuid":"186bf13797d349e38f4b1d8ff17b6b68","notifyParameters":"{\"goods_id\":0,\"cp_channel_type\":11205,\"cp_order_id\":\"1528170999\",\"recharge_type\":2,\"ip\":\"113.66.249.161\",\"role_id\":230,\"recharge_id\":2001}","orderId":"1528170999"}';
//var ttt = '{"amount":"6","sign":"be868065f181c5890d407bb3036e1716","appId":"gzc2cb557eb0f9946f","status":"SUCCESS","sandbox":"1","payWay":"4","uuid":"186bf13797d349e38f4b1d8ff17b6b68","notifyParameters":"{\"goods_id\":0}","orderId":"1528170999"}";
//var ddd = "{\"goods_id\":0,\"cp_channel_type\":11901,\"cp_order_id\":\"1528190405\",\"recharge_type\":2,\"ip\":\"113.66.249.161\",\"role_id\":11444,\"recharge_id\":2001}";

var fff = '{"amount":"6","sign":"ceaa84423b2e99b747d1de2074e3b93a","appId":"gzc2cb557eb0f9946f","status":"SUCCESS","sandbox":"1","payWay":"4","uuid":"186bf13797d349e38f4b1d8ff17b6b68","notifyParameters":"%7B%22goods%5Fid%22%3A0%2C%22cp%5Fchannel%5Ftype%22%3A11205%2C%22cp%5Forder%5Fid%22%3A%221528199204%22%2C%22recharge%5Ftype%22%3A2%2C%22ip%22%3A%22113%2E66%2E249%2E161%22%2C%22role%5Fid%22%3A11447%2C%22recharge%5Fid%22%3A2001%7D","orderId":"1528199204"}';

var json_ttt = JSON.parse(fff);

//sign:{"goods_id":0,"cp_channel_type":11205,"cp_order_id":"1528199204","recharge_type":2,"ip":"113.66.249.161","role_id":11447,"recharge_id":2001}

var uuu = common_logic.url_decode_ex(json_ttt.notifyParameters)

var json_ttt = JSON.parse(uuu);

//console.log('sign:%s',json_ttt);  

//var json_ttt = JSON.parse(ttt);
//var fuckandfuck = 'KevinChen';

//fuckandfuck = fuckandfuck.toUpperCase();

//console.log('fuckandfuck:%s',fuckandfuck);  

//var ppp = 'amount=6&appId=gzc2cb557eb0f9946f&notifyParameters={"goods_id":0,"cp_channel_type":11205,"cp_order_id":"1528203074","recharge_type":2,"ip":"113.66.249.161","role_id":11447,"recharge_id":2001}&orderId=1528203074&payWay=4&status=SUCCESS&uuid=186bf13797d349e38f4b1d8ff17b6b68&appsecret=e9f942c1002ae7846650efbf9fd0203b';



var srcArr = [];  
for (var i=0; i<10; i++) {  
    srcArr[i] = parseInt(Math.random() * 1000);  
}  

//console.log('srcArr.length:%d',srcArr.length); 
// 1530410400 time:1530410400

var data_time = 20180701;
var year = parseInt(data_time / 10000);
var month = parseInt(data_time % 10000 * 0.01);
var day = parseInt(data_time % 100);
var params = util.format('%d-%d-%d 10:00:00', year, month, day);	
console.log('data_time:%s',params);  

// 24 * 60 * 60
//params = params - 24 * 60 * 60;

var date = new Date(params);
var time = date.getTime()/1000;//转换成秒；

time = time + 24 * 60 * 60 * 5;
console.log('当前时间:%s',time);  

// var date = new Date( time * 1000 );//.转换成毫秒
//  var time = date.getFullYear() + "-" + (date.getMonth() < 10 ? '0' + (date.getMonth()+1) : (date.getMonth()+1)) + "-" + (date.getDate() < 10 ? '0' + date.getDate() : date.getDate()) ; 

var date1 = new Date(time * 1000);//.转换成毫秒
var time1 = date1.getFullYear() + "-" + (date1.getMonth() + 1) + "-" + date1.getDate(); //(date1.getMonth() < 10 ? '0' + (date1.getMonth()+1)
console.log('转换后时间:%s',time1);


//common_dao.new_day(20180612);

