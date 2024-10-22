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



var Md5 = require('./utils/Md5');
var md5=new Md5();

//var multer = require('multer'); 
app.use(bodyParser.json()); // for parsing application/json
//app.use(multer()); // for parsing multipart/form-data

//var approute = require('./app_route');
//approute.use(app);              //路由文件


var recharge_callback_url = config.recharge_callback_url;
var local_ip = config.ip;

app.use('/skyx', express.static(__dirname + '/public/skyx'));

var server = app.listen(20001, function () {
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
app.get('/test', function (req, res) {
	var ip = common_logic.get_client_ip_ex(req);
	console.log('ip:%s',ip);	
	res.send(ip);
});


function save_recharge_log(url)
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
			//console.log("call_php_resp:" + _data);
			//client.send(_data);
			return _data;
        });
    }).on('error', function (e) {
		//console.log('发送消息到游戏服务器出错:'+e);
		var error_desc = "发送消息到游戏服务器出错"+e;
		return error_desc;
    });	
}


//开发服:
// recharge_callback_quicksdk_dev_android
// recharge_callback_quicksdk_dev_ios
// 开发服充值回调通知(安卓)(todo::目前是乐游)
app.post('/recharge_callback_quicksdk_dev_android', function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    console.log('recharge_callback_quicksdk_dev_android be call!(%s)',cur_time);
	
	var call_game_url = "http://172.18.120.86:40016/recharge_callback_quicksdk_android";
	var redirect_url = call_game_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign;	
	RechargeCallbackNotify(redirect_url, res);
});

// 开发服充值回调通知(IOS)(todo::目前是乐游)
app.post('/recharge_callback_quicksdk_dev_ios', function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    console.log('recharge_callback_quicksdk_dev_ios be call!(%s)',cur_time);
	
	var call_game_url = "http://172.18.120.86:40016/recharge_callback_quicksdk_ios";
	var redirect_url = call_game_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign;	
	RechargeCallbackNotify(redirect_url, res);
});


//发布服:
// recharge_callback_quicksdk_release_android
// recharge_callback_quicksdk_release_ios
// 发布服充值回调通知(安卓)(todo::目前是汇娱)
app.post('/recharge_callback_quicksdk_release_android', function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    console.log('recharge_callback_quicksdk_release_android be call!(%s)',cur_time);
	
	var call_game_url = "http://192.168.1.168:40006/recharge_callback_quicksdk_android";
	var redirect_url = call_game_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign;	
	RechargeCallbackNotify(redirect_url, res);
});

// 发布服充值回调通知(IOS)(todo::目前是汇娱)
app.post('/recharge_callback_quicksdk_release_ios', function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    console.log('recharge_callback_quicksdk_release_ios be call!(%s)',cur_time);
	
	var call_game_url = "http://192.168.1.168:40006/recharge_callback_quicksdk_ios";
	var redirect_url = call_game_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign;	
	RechargeCallbackNotify(redirect_url, res);
});


// 发布服充值回调通知(安卓)(创悦)
app.post('/recharge_callback/quicksdk/cw/cy/android', function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    console.log('recharge_callback/quicksdk/cw/cy/android be call!(%s)',cur_time);
	
	var call_game_url = "http://172.18.120.86:40026/recharge_callback_quicksdk_android";
	var redirect_url = call_game_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign;	
	RechargeCallbackNotify(redirect_url, res);
});



// 发布服充值回调通知(安卓)(todo::目前是汇娱专服)
app.post('/recharge_callback/quicksdk/hy/zf/android', function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    console.log('recharge_callback/quicksdk/hy/zf/android be call!(%s)',cur_time);
	
	var call_game_url = "http://172.18.120.87:40016/recharge_callback_quicksdk_android";
	var redirect_url = call_game_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign;	
	RechargeCallbackNotify(redirect_url, res);	
});

// 发布服充值回调通知(IOS)(todo::目前是汇娱专服)
app.post('/recharge_callback/quicksdk/hy/zf/ios', function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    console.log('recharge_callback/quicksdk/hy/zf/ios be call!(%s)',cur_time);
	
	var call_game_url = "http://172.18.120.87:40016/recharge_callback_quicksdk_ios";
	var redirect_url = call_game_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign;	
	RechargeCallbackNotify(redirect_url, res);
});


// 发布服充值回调通知(安卓)(目前是迈趣)
app.post('/recharge_callback/quicksdk/cw/mq/android', function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    console.log('recharge_callback/quicksdk/cw/mq/android be call!(%s)',cur_time);
	
	var call_game_url = "http://172.18.120.86:40016/recharge_callback_quicksdk_android";
	var redirect_url = call_game_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign;	
	RechargeCallbackNotify(redirect_url, res);
});


// 发布服充值回调通知(安卓)(bt cgame)
app.post('/recharge_callback/quicksdk/bt/cgame/android', function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    console.log('recharge_callback/quicksdk/bt/cgame/android be call!(%s)',cur_time);
	var call_game_url = "http://172.31.0.143:40016/recharge_callback_quicksdk_android";
	var redirect_url = call_game_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign;	
	RechargeCallbackNotify(redirect_url, res);
});

// 发布服充值回调通知(安卓)(bt cgame)
app.post('/recharge_callback/quicksdk/bt/cgame/ios', function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    console.log('recharge_callback/quicksdk/bt/cgame/ios be call!(%s)',cur_time);
	var call_game_url = "http://172.31.0.143:40016/recharge_callback_quicksdk_ios";
	var redirect_url = call_game_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign;	
	RechargeCallbackNotify(redirect_url, res);
});



// 发布服充值回调通知(安卓)(遨游bt乐嗨嗨)
app.post('/recharge_callback/quicksdk/aoyou/bt/lhh/android', function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    console.log('recharge_callback/quicksdk/aoyou/bt/lhh/android be call!(%s)',cur_time);
	
	var call_game_url = "http://172.31.0.143:40026/recharge_callback_quicksdk_android";
	var redirect_url = call_game_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign;	
	RechargeCallbackNotify(redirect_url, res);
});

// 发布服充值回调通知(ios)(遨游bt乐嗨嗨)
app.post('/recharge_callback/quicksdk/aoyou/bt/lhh/ios', function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    console.log('recharge_callback/quicksdk/aoyou/bt/lhh/ios be call!(%s)',cur_time);
	
	var call_game_url = "http://172.31.0.143:40026/recharge_callback_quicksdk_ios";
	var redirect_url = call_game_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign;	
	RechargeCallbackNotify(redirect_url, res);
});



// 发布服充值回调通知(安卓)(目前是迈趣)
app.post('/recharge_callback/quicksdk/huiyao/android', function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    console.log('recharge_callback/quicksdk/huiyao/android be call!(%s)',cur_time);
	
	var call_game_url = "http://172.18.120.83:40016/recharge_callback_quicksdk_android";
	var redirect_url = call_game_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign;	
	RechargeCallbackNotify(redirect_url, res);
});


// 发布服充值回调通知(安卓)(目前是乐次元)
app.post('/recharge_callback/quicksdk/lechiyuan/android', function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    console.log('recharge_callback/quicksdk/huiyao/android be call!(%s)',cur_time);
	
	var call_game_url = "http://172.18.120.83:40016/recharge_callback_quicksdk_android";
	var redirect_url = call_game_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign;	
	RechargeCallbackNotify(redirect_url, res);
});




// 充值回调通知(quick_android_sdk)
app.post('/recharge_callback_quicksdk', function (req, res) {
//app.post('/recharge_callback_quicksdk_android', function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    console.log('recharge_callback_quicksdk_android be call!(%s)',cur_time);
	
	var call_game_url = "http://192.168.1.88:40006/recharge_callback_quicksdk_android";
	var redirect_url = call_game_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign;	
	RechargeCallbackNotify(redirect_url, res);
});


// 充值回调通知(quick_ios_sdk)
app.post('/recharge_callback_quicksdk_ios', function (req, res) {
	var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    console.log('recharge_callback_quicksdk_ios be call!(%s)',cur_time);
	
	var call_game_url = "http://192.168.1.111:40006/recharge_callback_quicksdk_ios";
	var redirect_url = call_game_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign;	
	RechargeCallbackNotify(redirect_url, res);
	
	// 存储充值发货通知数据
	var call_php_url = 'http://192.168.1.111/game_php_util/quick_sdk/save_recharge_data.php';
	var redirect_php_url = call_php_url+'?nt_data='+req.body.nt_data+'&sign='+req.body.sign+'&md5Sign='+req.body.md5Sign+'&is_android='+0;
	save_recharge_log(redirect_php_url);
});




////////////////////////////// 跳转接口 ////////////////////////////////

// 苹果内购验票据
app.post('/apple_verify_receipt', function (req, res) {	
	game.apple_verify_receipt(req, res);
});	

// 请求订单号
app.post('/cp_order_id', function (req, res) {	
	game.cp_order_id(req, res);
});	

// 请求在线玩家数量
app.post('/online_player_num', function (req, res) {	
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



/////////////////////////////////// 全球英语版本 ////////////////////////////////

// 英文版充值回调(ios)
app.post('/recharge_callback/global_english_version/ios', function (req, res) {	
	global_english.recharge_callback_ios(req, res);
});	

// 英文版充值回调(ios)
app.post('/recharge_callback/global_english_version/ios/ex', function (req, res) {	
	global_english.recharge_callback_ios_ex(req, res);
});	

// 英文版充值回调(android)
app.post('/recharge_callback/global_english_version/android', function (req, res) {	
	global_english.recharge_callback_android(req, res);
});	

// 创建签名
app.post('/create_sign_for_sb', function (req, res) {	
	global_english.create_sign_for_sb(req, res);
});	

// 礼包通知
app.post('/global_english_version/gift_notify', function (req, res) {	
	global_english.gift_notify(req, res);
});	

// 角色登录上报
app.post('/role_login_report', function (req, res) {	
	global_english.role_login_report(req, res);
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
// IOS联运接口

// 万米联运充值回调(ios)
app.post('/recharge_callback_ios_ly_wm', function (req, res) {	
	ios_ly.recharge_callback_ios_ly_wm(req, res);
});	

// 娱丸联运充值回调(ios)
app.post('/recharge_callback_ios_ly_yw', function (req, res) {	
	ios_ly.recharge_callback_ios_ly_yw(req, res);
});	

// 清源联运充值回调(ios)
app.post('/recharge_callback_ios_ly_qingyuan', function (req, res) {	
	ios_ly.recharge_callback_ios_ly_qingyuan(req, res);
});	

// 云锐联运充值回调(ios)
app.post('/recharge_callback/ios_ly/yunrui', function (req, res) {
	ios_ly.recharge_callback_ios_ly_yunrui(req, res);
});	

// 天娱联运充值回调(ios)
app.post('/recharge_callback/ios_ly/tianyu', function (req, res) {
	ios_ly.recharge_callback_ios_ly_tianyu(req, res);
});	

// 浩动联运充值回调(ios)
app.post('/recharge_callback/ios_ly/haodong', function (req, res) {
	ios_ly.recharge_callback_ios_ly_haodong(req, res);
});	

// 浩动联运充值回调(ios)
app.post('/recharge_callback/ios_ly/haodong/new', function (req, res) {
	ios_ly.recharge_callback_ios_ly_haodong_new(req, res);
});	

// 29联运充值回调(ios)
app.post('/recharge_callback_ios_ly_29', function (req, res) {
	ios_ly.recharge_callback_ios_ly_29(req, res);
});	

// 酷炫联运充值回调(ios) 
app.post('/recharge_callback/ios_ly/kuxuan', function (req, res) {
	ios_ly.recharge_callback_ios_ly_kuxuan(req, res);
});

// 糖谷联运充值回调(ios) 
app.post('/recharge_callback/ios_ly/tanggu', function (req, res) {
	ios_ly.recharge_callback_ios_ly_tanggu(req, res);
});		


// 酷炫联运下单
app.post('/kuxuan/ios_ly/order', function (req, res) {
	ios_ly.recharge_callback_ios_ly_kuxuan(req, res);
});	



///////////////////////////////////////////
// 长尾联运接口 recharge_callback_cw_leyou

// 乐游长尾充值回调
app.post('/recharge_callback/cw/leyou', function (req, res) {
	quick_sdk.recharge_callback_cw_leyou(req, res);
});	


// 汇娱长尾充值回调(ios)
app.post('/recharge_callback/cw/huiyu/ios', function (req, res) {
	quick_sdk.recharge_callback_cw_huiyu_ios(req, res);
});	


// 汇娱长尾充值回调(android)
app.post('/recharge_callback/cw/huiyu/android', function (req, res) {
	quick_sdk.recharge_callback_cw_huiyu_android(req, res);
});	


///////////////////////////////////////////
// 智品维语相关接口

///////////////////////////////////////////
// 充值回调相关接口
app.get('/recharge_callback/zhipin', function (req, res) {
	recharge.recharge_callback_zhipin(req, res);
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
// 辉耀相关接口
app.get('/recharge_callback/huiyao/android', function (req, res) {	
	recharge_callback_huiyao.recharge_callback_android(req, res);
});	

app.post('/recharge_callback/huiyao/ios', function (req, res) {	
	recharge_callback_huiyao.recharge_callback_ios(req, res);
});	

///////////////////////////////////////////
// 遨游相关接口
app.post('/recharge_callback/aoyou', function (req, res) {	
	recharge_aoyou.recharge_callback(req, res);
});	


///////////////////////////////////////////
// 乐次相关接口
// 取订单号
app.post('/leciyuan/get_order_id', function (req, res) {	
	recharge_leciyuan.get_order_id(req, res);
});

// 充值发货
app.post('/recharge_callback/leciyuan/android', function (req, res) {	
	recharge_leciyuan.recharge_callback(req, res);
});	


///////////////////////////////////////////
// 云顶相关接口
app.post('/yunding/get_order_id', function (req, res) {	
	recharge_yunding.get_order_id(req, res);
});

// 充值发货(ios)
app.post('/recharge_callback/yunding/ios', function (req, res) {	
	recharge_yunding.recharge_callback(req, res);
});	


///////////////////////////////////////////
// 不凡相关接口
// 充值发货
app.post('/recharge_callback/bufan/ios', function (req, res) {	
	recharge_bufan.recharge_callback(req, res);
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
