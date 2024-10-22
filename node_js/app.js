var express = require('express');
var app = express();
var path = require('path');
var fs = require('fs');
var util = require('util');
var crypto = require('crypto');
var bodyParser = require("body-parser"); 
app.use(bodyParser.urlencoded({ extended: false })); 
var dateformat = require('./utils/dateformat');
var http = require('http');
var common_dao = require('./dao/common_dao');
var common_logic = require('./common_logic');
var mycard = require('./mycard');
var game = require('./game');
var global_english = require('./global_english');
var xmgta_xtong = require('./xmgta_xtong');
var ios_ly = require('./ios_ly');
var check_user_info = require('./check_user_info');
var config = require('./utils/config');
var quick_sdk = require('./quick_sdk');
var recharge = require('./recharge');
var recharge_xtone_dz = require('./recharge_xtone_dz');
var recharge_unlock = require('./recharge_unlock');
var recharge_callback_huiyao = require('./recharge_callback_huiyao');
var game_statistics = require('./game_statistics');
var recharge_aoyou = require('./recharge_aoyou');
var recharge_leciyuan = require('./recharge_leciyuan');
var recharge_xtone_bt = require('./recharge_xtone_bt');
var recharge_xtone_zf = require('./recharge_xtone_zf');
var recharge_yunding = require('./recharge_yunding');
var recharge_xtone_jt_ios = require('./recharge_xtone_jt_ios');
var recharge_xtone_zf2 = require('./recharge_xtone_zf2');
var recharge_bufan = require('./recharge_bufan');
var recharge_ygame = require('./recharge_ygame');
var recharge_weilai = require('./recharge_weilai');
var recharge_haodong = require('./recharge_haodong');
var activity_notice = require('./activity_notice');
var recharge_quicksdk = require('./recharge_quicksdk');

var Md5 = require('./utils/Md5');
var md5 = new Md5();

//var multer = require('multer'); 
app.use(bodyParser.json()); // for parsing application/json
//app.use(multer()); // for parsing multipart/form-data

//var approute = require('./app_route');
//approute.use(app);              //路由文件


var recharge_callback_url = config.recharge_callback_url;
var local_ip = config.ip;

app.use('/skyx', express.static(__dirname + '/public/skyx'));

var server = app.listen(16888, function () {
  var host = server.address().address;
  var port = server.address().port;

  console.log('Example app listening at http://%s:%s', host, port);
  console.log('dir_name:%s', __dirname);
});

// 充值回调通知(游戏服务器)
function RechargeCallbackNotify(url, res){
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
			client.send("SUCCESS");
        });
    }).on('error', function (e) {
		console.log('发送消息到游戏服务器出错:'+e);
    });		
}

// 测试使用
app.get('/my_ip', function (req, res) {
	var ip = common_logic.get_client_ip_ex(req);
	var msg = util.format('hello,your ip is:%s',ip);		
	console.log(msg);	
	res.send(msg);
});


// 统计服务器的统计
app.post('/statistics_from_server', function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
	game_statistics.game_statistics_from_server(req, res);
});


// 统计(混服)
app.get('/statistics_hf', function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
	//console.log('statistics_hf be call!(%s)',cur_time);
	
	//console.log('req.query.role_id = %d',req.query.role_id);
	//console.log('req.query.system_id =%d',req.query.system_id);	
	
	if(req.query.role_id == undefined){
		console.log('req.query.role_id == undefined')
		return;
	}
	
	if(req.query.system_id == undefined){
		console.log('req.query.system_id == undefined')
		return;
	}	

	//var result = common_dao.get_system_click_times(req.query.system_id).then(function (result) {	
	//	var click_times = 0;
	//	if (result.length > 0) {
	//		click_times = result[0].click_times;
    //    }
	//	common_dao.update_system_click_times(req.query.system_id, click_times+1);
   // });	
		
	res.sendStatus(200);
});


///////////////////////////////////////////
// quicksdk充值回调相关

// 发布服充值回调通知(安卓)(翔通繁体)
app.post('/recharge_callback/quicksdk/xtone_lhh/fanti/android', function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    console.log('recharge_callback/quicksdk/xtone_lhh/fanti/android call!(%s)',cur_time);
	
	var call_game_url = "http://172.31.211.165:40006/recharge_callback_quicksdk_android";
	var redirect_url = call_game_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign;	
	RechargeCallbackNotify(redirect_url, res);
});

// 发布服充值回调通知(ios)(翔通繁体)
app.post('/recharge_callback/quicksdk/xtone_lhh/fanti/ios', function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    console.log('recharge_callback/quicksdk/xtone_lhh/fanti/ios call!(%s)',cur_time);
	
	var call_game_url = "http://172.31.211.165:40006/recharge_callback_quicksdk_ios";
	var redirect_url = call_game_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign;	
	RechargeCallbackNotify(redirect_url, res);
});

// funpax充值回调通知(ios)
app.post('/recharge_callback/quicksdk/funpax/ios', function (req, res) {	
	var game_svr_ip = '192.168.1.78';
	var game_svr_port = 40006;
	var callback_key = '21299129384669034103404539411823';
	recharge_quicksdk.recharge_callback_II(req, res, game_svr_ip, game_svr_port, callback_key);
});

// funpax充值回调通知(android)
app.post('/recharge_callback/quicksdk/funpax/android', function (req, res) {	
	var game_svr_ip = '192.168.1.78';
	var game_svr_port = 40006;
	var callback_key = '30050995851096096404452209140124';
	recharge_quicksdk.recharge_callback_II(req, res, game_svr_ip, game_svr_port, callback_key);
});


// 发布服充值回调通知(安卓)(996_bt)
app.post('/recharge_callback/quicksdk/996_bt/android', function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    console.log('recharge_callback/quicksdk/996_bt/android call!(%s)',cur_time);
	
	var call_game_url = "http://10.46.225.165:40006/recharge_callback_quicksdk_android";
	var redirect_url = call_game_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign;	
	RechargeCallbackNotify(redirect_url, res);
});

// 发布服充值回调通知(ios)(996_bt)
app.post('/recharge_callback/quicksdk/996_bt/ios', function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    console.log('recharge_callback/quicksdk/996_bt/ios call!(%s)',cur_time);
	
	var call_game_url = "http://10.46.225.165:40006/recharge_callback_quicksdk_ios";
	var redirect_url = call_game_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign;	
	RechargeCallbackNotify(redirect_url, res);
});




// 测试使用
app.get('/get_my_ip', function (req, res) {
  console.log('get_my_ip:(%s)',req.query.ip);
  res.send('fuck_sword_man');
});	



////////////////////////////// 跳转接口 ////////////////////////////////

/////////////////////////////////////////////////////////
////////////// 通用接口
// 检测用户信息
app.post('/check_user_info', function (req, res) {	
	check_user_info['check_user_info'](req, res);
});	

// 苹果内购验票据
app.post('/apple_verify_receipt', function (req, res) {	
	game.apple_verify_receipt(req, res);
});	

// 请求订单号
app.post('/cp_order_id', function (req, res) {	
	game.cp_order_id(req, res);
});	

// 请求在线玩家数量
app.get('/online_player_num', function (req, res) {	
	game.online_player_num(req, res);
});

// 爱贝支付下单(手机)
app.post('/iapppay/order', function (req, res) {	
	game.iapppay_order(req, res);
});	

// 爱贝支付下单(h5或pc)
app.post('/iapppay/h5orpcpay', function (req, res) {	
	game.iapppay_h5orpcpay_order(req, res);
});	

// 请求服务器列表
app.get('/server_list', function (req, res) {	
	game.server_list(req, res);
});	

// 查看服务器列表
app.get('/role_info', function (req, res) {	
	game.role_info(req, res);	
});	


// 登录统计
app.post('/client_event_statistics', function (req, res) {	
	game.client_event_statistics(req, res);	
});	


// 登录统计
app.post('/client_event_statistics/error_log', function (req, res) {	
	game.event_statistics_error_log(req, res);	
});	


///////////////////////////////////////////
// 活动公告相关

// 请求活动公告(通用)
app.post('/activity_notice', function (req, res) {	
	activity_notice.activity_notice(req, res, 0);	
});	





/////////////////////////////////// 新马港台澳 ////////////////////////////////

// 翔通新马港台充值回调(ios)
app.post('/recharge_callback_xtone_xmgt_ios', function (req, res) {	
	xmgta_xtong.recharge_callback_xtone_xmgt_ios(req, res);
});	

// 翔通新马港台充值回调(ios)
app.post('/recharge_callback_xtone_xmgt_android', function (req, res) {	
	xmgta_xtong.recharge_callback_xtone_xmgt_android(req, res);
});	

// 向MyCard 要求交易授權碼
app.post('/mycard_authglobal', function (req, res) {	
	mycard.mycard_authglobal(req, res);
});	

// mycard充值回调 交易结果回传网址(正式环境):
app.post('/recharge_callback/mycard', function (req, res) {	
	mycard.recharge_callback_mycard(req, res);
});	

// mycard补单 补储讯息通知回传网址(正式环境):
app.post('/recharge_callback/mycard_ex', function (req, res) {	
	mycard.recharge_callback_mycard_ex(req, res);
});	

// 查看充值记录 商家提供差异比对网址(正式环境):
app.post('/recharge_record/mycard', function (req, res) {	
	mycard.recharge_record(req, res);
});	

// mycard充值回调 交易结果回传网址(测试环境):
app.post('/test/recharge_callback/mycard', function (req, res) {	
	mycard.recharge_callback_mycard_from_client(req, res);
});	

// mycard补单 补储讯息通知回传网址(测试环境):
app.post('/test/recharge_callback/mycard_ex', function (req, res) {	
	mycard.recharge_callback_mycard_ex(req, res);
});	

// 查看充值记录 商家提供差异比对网址(测试环境):
app.post('/test/recharge_record/mycard', function (req, res) {	
	mycard.recharge_record(req, res);
});	

// mycard充值回调（客户端）
app.post('/recharge_callback/mycard/client', function (req, res) {	
	mycard.recharge_callback_mycard_from_client(req, res);
});	



///////////////////////////////////////////
// 翔通定制相关接口(android)

// 定制取订单号(jt_android)
app.post('/xtone/dz/get_order_id', function (req, res) {
	recharge_xtone_dz.get_order_id(req, res);
});	

// 充值回调通知 (jt_android)
app.post('/xtone/dz/recharge_callback', function (req, res) {
	recharge_xtone_dz.recharge_callback(req, res);
});	

// 定制取订单号(ios越狱)
app.post('/xtone/dz/ios/yy/get_order_id', function (req, res) {
	recharge_xtone_dz.get_order_id_for_ios_yy(req, res);
});	


///////////////////////////////////////////
// 翔通定制相关接口(ios)

// 定制取订单号(new ios)
app.post('/xtone/dz/new/ios/get_order_id', function (req, res) {
	recharge_xtone_dz.get_order_id_for_new_ios(req, res);
});	

// 定制取订单号(jt_ios)
app.post('/xtone/dz/ios/get_order_id', function (req, res) {
	recharge_xtone_jt_ios.get_order_id_for_ios(req, res);
});

// 充值回调通知(jt_ios) 
app.post('/xtone/jt/ios/recharge_callback', function (req, res) {
	recharge_xtone_jt_ios.recharge_callback(req, res);
});	



///////////////////////////////////////////
// 翔通定制相关接口(zf)

// 专服订单号
app.post('/xtone/zf/get_order_id', function (req, res) {
	recharge_xtone_zf.get_order_id_for_zf(req, res);
});	

// 专服充值回调通知(专服)
app.post('/xtone/zf/recharge_callback', function (req, res) {
	recharge_xtone_zf.zf_recharge_callback(req, res);
});	


///////////////////////////////////////////
// 翔通定制相关接口(zf2)

// 专服订单号
app.post('/xtone/zf2/get_order_id', function (req, res) {
	recharge_xtone_zf2.get_order_id_for_zf(req, res);
});	

// 专服充值回调通知(专服)
app.post('/xtone/zf2/recharge_callback', function (req, res) {
	recharge_xtone_zf2.zf_recharge_callback(req, res);
});	




///////////////////////////////////////////
// 翔通定制相关接口(bt)

// 定制bt1取订单号(android)
app.post('/xtone/bt1/get_order_id', function (req, res) {
	recharge_xtone_bt.get_order_id(req, res);
});

// 定制bt2取订单号(android)
app.post('/xtone/bt2/get_order_id', function (req, res) {
	recharge_xtone_bt.get_order_id(req, res);
});

// 定制bt3取订单号(android)
app.post('/xtone/bt3/get_order_id', function (req, res) {
	recharge_xtone_bt.get_order_id(req, res);
});

// 定制bt充值回调通知 
app.post('/xtone/bt/recharge_callback', function (req, res) {
	recharge_xtone_bt.recharge_callback(req, res);
});	



///////////////////////////////////////////
// unlock制相关接口 

// 普通ios充值回调(unlock)
app.post('/recharge_callback/unlock/ios', function (req, res) {	
	recharge_unlock.recharge_callback1(req, res, '141df0eeda15dc5a461b03a024c57a15', 1);
});	

// 普通ios充值回调(unlock,提审接口)
app.post('/recharge_callback/unlock/ios/examine', function (req, res) {	
	recharge_unlock.recharge_callback_examine(req, res, '141df0eeda15dc5a461b03a024c57a15');
});	

// 普通android充值回调(unlock)
app.post('/recharge_callback/unlock/android', function (req, res) {	
	recharge_unlock.recharge_callback1(req, res, 'f5b1a78599566df0af0913c7e74bc8fb', 1);
});

// 第三方ios充值回调(unlock)
app.post('/recharge_callback/unlock/third/ios', function (req, res) {	
	recharge_unlock.recharge_callback2(req, res, '141df0eeda15dc5a461b03a024c57a15', 1);
});	

// 第三方android充值回调(unlock)
app.post('/recharge_callback/unlock/third/android', function (req, res) {	
	recharge_unlock.recharge_callback2(req, res, 'f5b1a78599566df0af0913c7e74bc8fb', 1);
});



// 新普通ios充值回调(unlock)
app.post('/recharge_callback/unlock/new/ios', function (req, res) {	
	recharge_unlock.recharge_callback1(req, res, 'd8f5h3k66l15dc5a461b03a0t9y82j2j', 2);
});	

// 新普通ios充值回调(unlock,提审接口)
app.post('/recharge_callback/unlock/new/ios/examine', function (req, res) {	
	recharge_unlock.recharge_callback_examine(req, res, 'd8f5h3k66l15dc5a461b03a0t9y82j2j');
});	

// 新第三方ios充值回调(unlock)
app.post('/recharge_callback/unlock/third/new/ios', function (req, res) {	
	recharge_unlock.recharge_callback2(req, res, 'd8f5h3k66l15dc5a461b03a0t9y82j2j', 1);
});	



///////////////////////////////////////////
// YGame相关接口
// 取服务器列表
app.get('/ygame/get_server_list', function (req, res) {	
	recharge_ygame.get_server_list(req, res);
});	

// 取角色列表
app.get('/ygame/get_role_list', function (req, res) {	
	recharge_ygame.get_role_list(req, res);
});	

// 取支付列表
app.get('/ygame/get_payment_list', function (req, res) {	
	recharge_ygame.get_payment_list(req, res);
});	

// 支付回调
app.get('/ygame/payment_callback', function (req, res) {	
	recharge_ygame.payment_callback(req, res);
});	

// 取现通知
app.get('/ygame/cashback_notify', function (req, res) {	
	recharge_ygame.cashback_notify(req, res);
});	

// 奖励通知
app.get('/ygame/bonus_notify', function (req, res) {	
	recharge_ygame.bonus_notify(req, res);
});	





////////////////////////////////////////////////////
// todo::以下接口日后迁移


//拦截请求 image
app.post("/upload_image",function (req,res) {	
    var form = new formidable.IncomingForm();
    form.encoding = 'utf-8';
    form.uploadDir = path.join(__dirname + "/../page/upload");
    form.keepExtensions = true;//保留后缀
    form.maxFieldsSize = 10 * 1024 * 1024;
	
    //处理图片
    form.parse(req, function (err, fields, files){
		
        console.log('files.the_file:%s',files.the_file);
        
		var filename = files.the_file.name
        var nameArray = filename.split('.');
        var type = nameArray[nameArray.length - 1];
        var name = '';
        
		for (var i = 0; i < nameArray.length - 1; i++) {
            name = name + nameArray[i];
        }
        
		var date = new Date();
        var time = '_' + date.getFullYear() + "_" + date.getMonth() + "_" + date.getDay() + "_" + date.getHours() + "_" + date.getMinutes();
        var avatarName = name + time + '.' + type;
        var newPath = form.uploadDir + "/" + avatarName;
        fs.renameSync(files.the_file.path, newPath);  //重命名
		
		console.log('avatarName:%s',avatarName);
		
        res.send({data:"/upload/"+avatarName})
		
    })	
});


// 手机号短信认证
app.get('/sms_sender', function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    console.log('sms_sender be call!(%s)',cur_time);	
	
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
		
	var url = "http://120.77.146.65/tools/export_tool/Qcloud/SmsSenderDemo.php";
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
	common_logic.SendMsgToGameServer(redirect_url, res);
});


