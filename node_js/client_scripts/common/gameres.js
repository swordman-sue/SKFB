
var express = require('express');
var app = express();
var path = require('path');
var fs = require('fs');
var util = require('util');
var dateformat = require('../../utils/dateformat');

// 注册静态资源目录
app.use('/skfb/develop', express.static(__dirname + '/../../public/skfb/develop', {maxAge:1000}));
app.use('/skfb/nnsbt', express.static(__dirname + '/../../public/skfb/nnsbt', {maxAge:1000}));

var server = app.listen(80, function () {
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

// 获取当前时间
function get_cur_time() 
{		
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
	return cur_time;		
};

// 重定向到客户端
function redirect_version_to_client(req, res, cdn_url, cdn_url_examine, md5_file_path, md5_file_path_examine, cdn_url_ex, cdn_url_examine_ex, md5_file_path_ex, md5_file_path_examine_ex) {			
	var	check_examine	= req.query.check_examine
	var channel_type 	= req.query.channel_type;
	var sub_channel_type= req.query.sub_channel_type;
	var app_version 	= req.query.app_version;
	
	var md5 = fs.readFileSync(__dirname + md5_file_path, 'utf-8');
	var md5_examine = null;
	if (md5_file_path_examine != null)
	{
		md5_examine = fs.readFileSync(__dirname + md5_file_path_examine, 'utf-8');	
	}
	
	if (check_examine == undefined || check_examine != "1")
	{
		// 响应数据
		var szParam = {
		code:200,
		str:"操作成功",
		msg:{
				"cdn":cdn_url, 
				"md5":md5,
				"is_skip_update":0
			}
		};
		res.send(szParam);
		console.log("[redirect_version_to_client] 返回cdn_url: " + cdn_url);		
		console.log("[redirect_version_to_client] 返回md5: " + md5);				
		return;
	}
	
	// 检测参数合法性
	if(channel_type == undefined || sub_channel_type == undefined || app_version == undefined)
	{
		console.log('[redirect_version_to_client] param invalid!');
		return;
	}
		
	// CP渠道类型
	var examine_toggle_list = [];
	//examine_toggle_list[1] = { Version: "100", IsSkipUpdate:1};
	
	var isInExamine = false; // 是否在审核中
	var isSkipUpdate = 0; 	 // 是否跳过更新
	
	var examineData = examine_toggle_list[channel_type];
	if (null != examineData)
	{			
		// 客户端的版本号大于等于服务器的版本号时属于审核状态
		if(app_version >= examineData.Version)
		{
			console.log("[redirect_version_to_client] 处于审核状态：channel_type: " + channel_type);
			isInExamine = true;
			isSkipUpdate = examineData.IsSkipUpdate;
		}			
	}

	var tmp_cdn_url = "";
	var tmp_md5 = "";
	if(isInExamine)
	{
		// 审核中的url
		tmp_cdn_url = cdn_url_examine != null ? cdn_url_examine : cdn_url;  
		// 审核中的md5
		tmp_md5 = md5_examine != null ? md5_examine : md5;
	}
	else
	{
		// 非审核中的url
		tmp_cdn_url = cdn_url; 
		// 非审核中的md5
		tmp_md5 = md5;
	}

	console.log("[redirect_version_to_client] 返回cdn_url: " + tmp_cdn_url);		
	console.log("[redirect_version_to_client] 返回md5: " + tmp_md5);		
	console.log("[redirect_version_to_client] 返回IsSkipUpdate: " + isSkipUpdate);		
	console.log("[redirect_version_to_client] 返回IsInExamine: " + (isInExamine ? 1 : 0));		

	// 响应数据
	var szParam = {
	code:200,
	str:"操作成功",
	msg:{
			"cdn":tmp_cdn_url, 
			"md5":tmp_md5,
			"is_skip_update":isSkipUpdate,
			"is_in_examine":(isInExamine ? 1 : 0)
		}
	};
	res.send(szParam);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
//开发服
////////////////////////////////////////////////////////////////////////////////////////////////////////
// 获取资源下载根路径和version.ios的MD5
app.get('/skfb/develop/version/callback', function (req, res) {
	console.log('\n[%s] 请求/skfb/develop/version/callback ip:%s', get_cur_time(), get_client_ip(req));

	redirect_version_to_client(req, res, "http://cdn.skfb.szbagame.com/skfb/develop/cdnfiles", null, './../../public/skfb/develop/cdnfiles/version_md5')	
});
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////
//996BT
////////////////////////////////////////////////////////////////////////////////////////////////////////
// 获取资源下载根路径和version.ios的MD5
app.get('/skfb/nnsbt/version/callback', function (req, res) {
	console.log('\n[%s] 请求/skfb/nnsbt/version/callback ip:%s', get_cur_time(), get_client_ip(req));

	redirect_version_to_client(req, res, "http://cdn.skfb.szbagame.com/skfb/nnsbt/cdnfiles", null, './../../public/skfb/nnsbt/cdnfiles/version_md5')	
});
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
