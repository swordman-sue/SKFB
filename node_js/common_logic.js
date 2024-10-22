var crypto = require('crypto');
var util = require('util');
var http = require('http');
var dateformat = require('./utils/dateformat');
var url = require('url');


// 存储日志
exports.save_logs = function(log){
		
};


// 排序
exports.key_sort = function(key,sort_type){
	return function(a,b){
		return sort_type ? ~~(a[key] < b[key]) : ~~(a[key] > b[key]);
	}		
};


// md5(带中文的md5)
exports.my_md5 = function(key){
    var Buffer = require("buffer").Buffer;  
    var buf = new Buffer(key);  
    var str = buf.toString("binary");  
    var crypto = require("crypto");  
    return crypto.createHash("md5").update(str).digest("hex");  
};

// sha256
exports.my_sha256 = function(key){
    //var Buffer = require("buffer").Buffer;  
    //var buf = new Buffer(key);  
    //var str = buf.toString("binary");  
    //var crypto = require("crypto");  
    return crypto.createHash("RSA-SHA256").update(key).digest("hex");  
};

// 获取客户端IP
exports.get_client_ip = function(req) {
        return req.headers['x-forwarded-for'] ||
        req.connection.remoteAddress ||
        req.socket.remoteAddress ||
        req.connection.socket.remoteAddress;
};

// 获取客户端IP
exports.get_client_ip_ex = function(req) {
	var src_ip = req.headers['x-forwarded-for'] ||
        req.connection.remoteAddress ||
        req.socket.remoteAddress ||
        req.connection.socket.remoteAddress;
		
	var ip = "127.0.0.1";
	var i = 0;
	// 查找到指定的字符串，并把字符串前后拆开成两个串
	var arr = src_ip.split("::ffff:").map(function (val) {
		i++;
		// 取第二个值
		if(i == 2){
			ip = val;
		}
	});	
	return ip;
};

exports.url_encode = function(url){
    //url = encodeURIComponent(url);
	url = encodeURI(url);
	//url = escape(url);
    //url = url.replace(/\%3A/g, ":");
    //url = url.replace(/\%2F/g, "/");
    //url = url.replace(/\%3F/g, "?");
    //url = url.replace(/\%3D/g, "=");
    //url = url.replace(/\%26/g, "&");
    return url;
}

exports.url_decode = function(url){
	url = decodeURI(url);    
    return url;
}

// 完整版本
exports.url_encode_ex = function(url){
    url = encodeURIComponent(url);
	//url = encodeURI(url);
	//url = escape(url);
    //url = url.replace(/\%3A/g, ":");
    //url = url.replace(/\%2F/g, "/");
    //url = url.replace(/\%3F/g, "?");
    //url = url.replace(/\%3D/g, "=");
    //url = url.replace(/\%26/g, "&");
    return url;
}

// 完整版本
exports.url_decode_ex = function(url){
    url = decodeURIComponent(url);
	//url = encodeURI(url);
	//url = escape(url);
    //url = url.replace(/\%3A/g, ":");
    //url = url.replace(/\%2F/g, "/");
    //url = url.replace(/\%3F/g, "?");
    //url = url.replace(/\%3D/g, "=");
    //url = url.replace(/\%26/g, "&");
    return url;
}

// 发送消息到游戏服务器
exports.SendMsgToGameServer = function(url, res){
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
			console.log("SendMsgToGameServerResult:" + _data);
			client.send(_data);
			//client.send("SUCCESS");	
        });
    }).on('error', function (e) {
		console.log('发送消息到游戏服务器出错:'+e);
    });		
}

// 重定向消息到游戏服务器
exports.redirect_to_game_server = function(url, res, resp_data){
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
			console.log("redirect_to_game_server:"+_data);
			client.send(resp_data);
        });
    }).on('error', function (e) {
		console.log('重定向消息到游戏服务器出错:'+e);
    });		
}


// 向游戏服务器发送消息
exports.send_msg_to_game_server = function(url, client, success_resp, failure_resp){
	http.get(url, function (res) {
        var list = [];
        var len = 0;
        res.on('data', function (data) {
            list.push(data);
            len += data.length;
        });
        res.on('end', function () {
            var result = Buffer.concat(list, len).toString("utf-8");
			var result = JSON.parse(result);
			// 1 表示成功
			if(result.state == 1){
				client.send(success_resp);						
			}else{
				console.log(result);
				client.send(failure_resp);	
			}
        });
    }).on('error', function (e) {
		console.log('发送消息到游戏服务器失败:'+e);
    });		
}

// 重定向消息到第三方服务器
exports.redirect_to_third_server = function(url, res){
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
			console.log("redirect_to_thr_server_result:"+_data);
			client.send(_data);
        });
    }).on('error', function (e) {
		console.log('重定向消息到第三方服务器出错:'+e);
    });		
}

// 重定向消息到目标
exports.redirect_to_target = function(ip, port, path, res, post_data){
	
	var http = require('http');
	var qs = require('querystring');	
		 
	var content = qs.stringify(post_data);
	 
	var options = {
	  host: ip,
	  port: port,
	  path: path,
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
			client.send(result_data);
	   });
	});
	 
	req.write(content);
	req.end();	
}

// 重定向消息到目标
exports.redirect_to_target_ex = function(ip, port, path, res, post_data){
	
	var http = require('http');
	var qs = require('querystring');	
		 
	var content = qs.stringify(post_data);
	 
	var options = {
	  host: ip,
	  port: port,
	  path: path,
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
			client.send(result_data);
	   });
	});
	 
	req.write(content);
	req.end();	
}

// 充值回调通知(游戏服务器)
exports.RechargeCallbackNotify = function(url, res){
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
			//client.send(_data);
			console.log("RechargeCallbackNotifyResult:" + _data);
			client.send("SUCCESS");
        });
    }).on('error', function (e) {
		console.log('发送消息到游戏服务器出错:'+e);
    });		
}

// https post 请求
exports.https_post = function(call_url, https, post_data){
	
	var qs = require('querystring');	
	var content = qs.stringify(post_data);
	//console.log(content); 
	var p = url.parse(call_url);	
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
		//callback_func(extend_data, result_data);
	   });
	});
	req.write(content);
	req.end();			
}

// http post 请求
exports.http_post = function(call_url, http, post_data){
	
	var qs = require('querystring');	
	var content = qs.stringify(post_data);
	console.log(content); 
	var p = url.parse(call_url);	
	var options = {
		host: p.hostname,
		port: p.port || 443,
		//port: 443, // 因为是https, 所以端口为443
		path: p.path,
		method: 'POST',
		rejectUnauthorized: false,
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
		console.log(result_data);  
		//callback_func(extend_data, result_data);
	   });
	});
	req.write(content);
	req.end();			
}

// http post 请求
exports.http_post_ex = function(call_url, http, post_data, res){
	
	var client = res;
	var qs = require('querystring');	
	var content = qs.stringify(post_data);
	console.log(content); 
	var p = url.parse(call_url);	
	var options = {
		host: p.hostname,
		port: p.port || 443,
		//port: 443, // 因为是https, 所以端口为443
		path: p.path,
		method: 'POST',
		rejectUnauthorized: false,
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
		console.log(result_data);  
		client.send(result_data);
		//callback_func(extend_data, result_data);
	   });
	});
	req.write(content);
	req.end();			
}




























