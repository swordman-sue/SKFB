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

var recharge_callback_url = config.recharge_callback_url;


// 游戏统计
exports.game_statistics_from_server = function (req, res, KEY) {
	//var cur_time = dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    //console.log('statistics_from_server be call!(%s)',cur_time);	

	//console.log(req.body);
	
	// 统计数据
	var statistics_data = JSON.parse(req.body.context);
	
	//console.log(statistics_data);
	
	if(req.body.app_key == undefined){
		console.log('req.body.app_key == undefined')
	}
	
	// check type
	if(req.body.type == undefined){
		console.log('req.body.type == undefined')
	}	
	
	if(req.body.app_key != 20160602){
		console.log('req.body.app_key != 20160602')
	}	
	
	var statistics_type = req.body.type;
	var server_id = statistics_data.server_id;
	var role_id = statistics_data.role_id;
	var datetime = statistics_data.datetime;
	
	var year = parseInt(datetime / 10000);
	var month = parseInt(datetime % 10000 * 0.01);
	var day = parseInt(datetime % 100);
	var params = util.format('%d-%d-%d 10:00:00', year, month, day);
	var game_cur_date = new Date(params);
	var game_cur_time = game_cur_date.getTime()/1000;//转换成秒；
	
	
	// 创建角色
	if(statistics_type == 10){
		
		// 更新总的创建角色数量
		common_dao.update_total_create_role_num(server_id);
		
		// 更新单个服每日的角色数量
		common_dao.update_daily_create_role_num(server_id, datetime, 1);
	}
	// 登录
	else if(statistics_type == 11){
		
		// 同一天多次登录，则记录第一次
		if(statistics_data.is_new_day  == true){
			common_dao.update_role_active_num(server_id, datetime);
		}
	}
	// 充值
	else if(statistics_type == 12){
		var amount = statistics_data.amount;
		var is_today_first_payment = statistics_data.is_today_first_payment;
		var is_new_role =  statistics_data.is_new_role;
		
		// 更新总充值
		common_dao.update_total_recharge_amount(server_id, amount);
		
		// 更新每日充值
		common_dao.update_daily_recharge_amount(server_id, datetime, amount);
		
		// 今日首充值
		if(is_today_first_payment == true){
			common_dao.update_total_recharge_role_num(server_id);
			common_dao.update_daily_recharge_role_num(server_id, datetime);
		}
		
		// 今日新角色付费
		if(is_new_role == true){
			common_dao.update_new_role_payment_amount(server_id, datetime, amount);
			
			if(is_today_first_payment == true){
				common_dao.update_new_role_payment_num(server_id, datetime);
			}
		}
		
	// 新的一天事件
	}else if(statistics_type == 13){
		new_day(datetime);
	}	
	
	var szParam = {
	result:200,
	desc:"operate success"
	};
	res.send(szParam);
}


// 通过时间错取日期
function get_date_by_time(time){
	var date = new Date( time * 1000 );//.转换成毫秒
	var date_time = date.getFullYear() + "-" + (date.getMonth() < 10 ? '0' + (date.getMonth()+1) : (date.getMonth()+1)) + "-" + (date.getDate() < 10 ? '0' + date.getDate() : date.getDate() + ' 00:00:00') ;
	return date_time;
}


// 新的一天
function new_day(datetime) {
	
	var one_day_total_sec = 24 * 60 * 60;
	var data_time = datetime;
	var year = parseInt(data_time / 10000);
	var month = parseInt(data_time % 10000 * 0.01);
	var day = parseInt(data_time % 100);
	
	// 今日开始时间
	var today_start_time = util.format('%d-%d-%d 00:00:00', year, month, day);	
	
	var today_date = new Date(today_start_time);
	
	var today_time = today_date.getTime()/1000;//转换成秒；	
	
	var yesterday_time = today_time - one_day_total_sec;
	
	var the_day_before_yesterday_time = today_time - one_day_total_sec * 2;
	
	// 今日开始时间
	today_start_time = get_date_by_time(today_time);
	
	// 昨日开始时间
	var yesterday_start_time = get_date_by_time(yesterday_time);
	
	// 前天开始时间
	var the_day_before_yesterday_start_time = get_date_by_time(the_day_before_yesterday_time);
	
	console.log('today_start_time:%s',today_start_time);  
	console.log('yesterday_start_time:%s',yesterday_start_time);  
	console.log('the_day_before_yesterday_start_time:%s',the_day_before_yesterday_start_time);  
	
	common_dao.new_day(datetime, today_start_time, yesterday_start_time, the_day_before_yesterday_start_time);
};

// 新的一天
function new_day_ex(datetime) {
	
	var one_day_total_sec = 24 * 60 * 60;
	var data_time = datetime;
	var year = parseInt(data_time / 10000);
	var month = parseInt(data_time % 10000 * 0.01);
	var day = parseInt(data_time % 100);
	
	// 今日开始时间
	var today_start_time = util.format('%d-%d-%d 00:00:00', year, month, day);	
	
	var today_date = new Date(today_start_time);
	
	var today_time = today_date.getTime()/1000;//转换成秒；	
	
	var yesterday_time = today_time - one_day_total_sec;
	
	// 今日开始时间
	today_start_time = get_date_by_time(today_time);
	
	// 昨日开始时间
	var yesterday_start_time = get_date_by_time(yesterday_time);
	
	// 前天
	
	console.log('today_start_time:%s',today_start_time);  
	console.log('yesterday_start_time:%s',yesterday_start_time);  
	
	common_dao.new_day(datetime, today_start_time, yesterday_start_time);
};

// 2018-05-24 00:03:06
//new_day(20180525);















