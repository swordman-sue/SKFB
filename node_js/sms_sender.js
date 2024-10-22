//var httputils = require('./utils/httputils');
//var https = require('https');
var http = require('http');

var express = require('express');
var app = express();
var path = require('path');
var fs = require('fs');

app.use('/skyx', express.static(__dirname + '/public/skyx'));


var https = require('https');
var zlib = require('zlib');


var server = app.listen(7878, function () {
  var host = server.address().address;
  var port = server.address().port;
  console.log('Example app listening at http://%s:%s', host, port);
  console.log('__dirname:%s', __dirname);
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
		hostname: '183.61.56.115',
		port: '443',
		path: '/v5/tlssmssvr/sendsms',
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
			default:req.end();
				break;
		}
	});
	req.on('error', function (e) {
		console.log(new Error('problem with request: ' + e.message));
		req.end();
		//setTimeout(cb, 10);
	});

	console.log('https_post_test');	
}

//https_post_test();

app.get('/test', function (req, res) {
	var post_data="………………";//请求数据
	var reqdata = JSON.stringify(post_data);
	var options = {
		hostname: '183.61.56.115',
		port: '8443',
		path: '/v5/tlssmssvr/sendsms',
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
			default:req.end();
				break;
		}
	});
	req.on('error', function (e) {
		console.log(new Error('problem with request: ' + e.message));
		req.end();
		//setTimeout(cb, 10);
	});
	
	res.send('hello,Kevin!');
});


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
			//SendMsgToUser(client,_data,nMsgId);
        });
    }).on('error', function (e) {
		console.log('发送消息到游戏服务器出错:'+e);
        //defered.reject(e);
    });		
}

app.get('/SmsSender', function (req, res) {
	
	// 检测随机数据
	if(req.query.random == undefined){
		res.send('random param undefined!');
		console.info('random param undefined');
		return;
	}	
	
	// 检测手机号
	if(req.query.phoneNumber == undefined){
		res.send('phoneNumber param undefined!');
		console.info('phoneNumber param undefined');
		return;
	}	
		
	var url = "http://120.77.146.65:8888//tools/export_tool/Qcloud/SmsSenderDemo.php";
	//var url = "http://192.168.1.111//tools/export_tool/Qcloud/SmsSenderDemo.php";
	var random = req.query.random;
	var phoneNumber = req.query.phoneNumber;
	var ext = req.query.ext;
	var templId = 16247;
	var appid = 1400028903;
	var appkey = 'ca6a1c9327c53603a50fc056c15c4c18';
	//console.log('do SmsSender random=%d phoneNumber=%d ext=%d templId=%d',random, phoneNumber,ext,templId);	
	var redirect_url = url+'?random='+random+'&phoneNumber='+phoneNumber+'&ext='+ext+'&templId='+templId+'&appid='+appid+'&appkey='+appkey;
	console.log(redirect_url);	
	SendMsgToGameServer(redirect_url, res);
});

app.get('/sms_fengce', function (req, res) {
	
	// 检测随机数据
	if(req.query.random == undefined){
		res.send('random param undefined!');
		console.info('random param undefined');
		return;
	}	
	
	// 检测手机号
	if(req.query.phoneNumber == undefined){
		res.send('phoneNumber param undefined!');
		console.info('phoneNumber param undefined');
		return;
	}	
		
	var url = "http://120.77.146.65:8888//tools/export_tool/Qcloud/SmsSenderDemo.php";
	//var url = "http://192.168.1.111//tools/export_tool/Qcloud/SmsSenderDemo.php";
	var random = req.query.random;
	var phoneNumber = req.query.phoneNumber;
	var ext = req.query.ext;
	var templId = 23540;
	var appid = 1400032890;
	var appkey = '31e223497d771fb0622e3246b134527b';
	//console.log('do SmsSender random=%d phoneNumber=%d ext=%d templId=%d',random, phoneNumber,ext,templId);	
	var redirect_url = url+'?random='+random+'&phoneNumber='+phoneNumber+'&ext='+ext+'&templId='+templId+'&appid='+appid+'&appkey='+appkey;
	console.log(redirect_url);	
	SendMsgToGameServer(redirect_url, res);
});


