
var express = require('express');
var app = express();
var path = require('path');
var fs = require('fs');
var util = require('util');
var dateformat = require('../../utils/dateformat');

// 注册静态资源目录
app.use('/skfb/package', express.static(__dirname + '/../../public/skfb/package', {maxAge:1000}));

var server = app.listen(8080, function () {
  var host = server.address().address;
  var port = server.address().port;
  console.log('Example app listening at http://%s:%s', host, port);
  console.log('__dirname:%s', __dirname);
});

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

function get_cur_time() 
{		
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
	return cur_time;		
};

// 获取安装包下载信息
function get_package_info(req, res, ver_path) {
	var package_path = '';
	var package_info = '';
	var package_suffix = '';
	var type = req.query.type;	
	var ver = fs.readFileSync(__dirname + ver_path, 'utf-8');
	ver = ver.replace(new RegExp("\\.", "gm"), '');

	switch(type)
	{
		case 'win32':
			package_suffix = 'exe';
			package_path = 'http://skfb-1254006149.cosgz.myqcloud.com/package_win32/%s';
			break;

		case 'android':
			package_suffix = 'apk';
			package_path = 'http://skfb-1254006149.cosgz.myqcloud.com/package/%s';
			break;

		case 'apple':
			package_suffix = 'ipa';
			package_path = 'http://skfb-1254006149.cosgz.myqcloud.com/package_ios/%s';
			break;
	}

	var package_name = req.query.pkgname.replace(new RegExp("\\.", "gm"), '_');
	package_name = req.query.fromid != '' ? util.format('%s_%s', package_name, req.query.fromid) : package_name;
	package_name = util.format('%s_%s.%s', package_name, ver, package_suffix)
	var package_url = util.format(package_path, package_name);
	package_info = package_name + package_url;
	
	return package_info
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
//开发服
////////////////////////////////////////////////////////////////////////////////////////////////////////
// 获取服务器配置
app.get('/skfb/develop/server_config', function (req, res) {
	console.log('\n[%s] 请求/skfb/develop/server_config ip:%s', get_cur_time(), get_client_ip(req));

	var server_config = JSON.parse(fs.readFileSync(__dirname + './../../public/skfb/develop/server_config.json','utf-8'));
		
	var szParam = {
	code:200,
	str:"操作成功",
	msg:server_config,
	ip:get_client_ip(req)
	};
		
	res.send(szParam);
});

app.get('/skfb/develop/server_config_test', function (req, res) {
	console.log('\n[%s] 请求/skfb/develop/server_config_test ip:%s', get_cur_time(), get_client_ip(req));

	var server_config = JSON.parse(fs.readFileSync(__dirname + '/../../public/skfb/develop/server_config_test.json','utf-8'));
		
	var szParam = {
	code:200,
	str:"操作成功",
	msg:server_config,
	ip:get_client_ip(req)
	};
		
	res.send(szParam);
});

// 获取服务器列表
app.get('/skfb/develop/server_list', function (req, res) {
	console.log('\n[%s] 请求/skfb/develop/server_list ip:%s', get_cur_time(), get_client_ip(req));

	var config_list = JSON.parse(fs.readFileSync(__dirname + './../../public/skfb/develop/server_list.json','utf-8'));
	
	var szParam = {
	code:200,
	str:"操作成功",
	msg:config_list
	};
	
	res.send(szParam);
});

// 获取安装包下载信息
app.get('/skfb/develop/packageinfo', function (req, res) {	
	console.log('\n[%s] 请求/skfb/develop/packageinfo ip:%s', get_cur_time(), get_client_ip(req));

	var package_info = get_package_info(req, res, './../../public/skfb/develop/cdnfiles/clientcppversioin');
	res.send(package_info);
});
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////
//996BT
////////////////////////////////////////////////////////////////////////////////////////////////////////
// 获取服务器配置
app.get('/skfb/nnsbt/server_config', function (req, res) {
	console.log('\n[%s] 请求/skfb/nnsbt/server_config ip:%s', get_cur_time(), get_client_ip(req));

	var server_config = JSON.parse(fs.readFileSync(__dirname + './../../public/skfb/nnsbt/server_config.json','utf-8'));
		
	var szParam = {
	code:200,
	str:"操作成功",
	msg:server_config,
	ip:get_client_ip(req)
	};
		
	res.send(szParam);
});

// 获取服务器列表
app.get('/skfb/nnsbt/server_list', function (req, res) {
	console.log('\n[%s] 请求/skfb/nnsbt/server_list ip:%s', get_cur_time(), get_client_ip(req));

	var config_list = JSON.parse(fs.readFileSync(__dirname + './../../public/skfb/nnsbt/server_list.json','utf-8'));
	
	var szParam = {
	code:200,
	str:"操作成功",
	msg:config_list
	};
	
	res.send(szParam);
});

// 获取安装包下载信息
app.get('/skfb/nnsbt/packageinfo', function (req, res) {	
	console.log('\n[%s] 请求/skfb/nnsbt/packageinfo ip:%s', get_cur_time(), get_client_ip(req));

	var package_info = get_package_info(req, res, './../../public/skfb/nnsbt/cdnfiles/clientcppversioin');
	res.send(package_info);
});
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

