var when=require('when');
var dao = require('../mysql/dao');
var util = require('util');
//var tokendao = require('../dao/tokendao');

// 取系统点击次数
exports.get_system_click_times = function (system_id) {
    var sql = 'SELECT * FROM system_click_record WHERE system_id=?';
    var args = [system_id];
    return dao.exeSql(sql, args);
};

// 更新系统点击次数
exports.update_system_click_times = function (system_id,click_times) {
	
	var sql = 'Replace INTO system_click_record(system_id,click_times) VALUES(?,?)';
    var args = [system_id,click_times];
	return dao.exeSql(sql, args);
	
    //var sql = 'SELECT * FROM system_click_record WHERE system_id=?';
    //var args = [system_id];
    //return dao.exeSql(sql, args);
};


// 系统点击次数
exports.test = function (role_id,role_name) {	
	var sql = 'INSERT INTO tmp_role_list(role_id,role_name) VALUES(?,?)';
    var args = [role_id, role_name];
    return dao.exeSql(sql, args);
};


// object_record
exports.object_record = function (role_id,obj_id,obj_num,is_add,time) {	
	var sql = 'INSERT INTO object_record(role_id,obj_id,obj_num,is_add,time) VALUES(?,?,?,?,?)';
    var args = [role_id, obj_id,obj_num,is_add,time];
    return dao.exeSql(sql, args);
};

// get_order_series_id
exports.get_order_series_id = function () {
    var sql = 'SELECT * FROM series_id';
    var args = [];
    return dao.exeSql(sql, args);
};

// update_order_series_id
exports.update_order_series_id = function (order_series_id) {	
    var sql='update series_id SET order_series_id = ?';
    var args = [order_series_id];
    return dao.exeSql(sql,args);	
};

// UPDATE series_id SET order_series_id = order_series_id + 1;SELECT order_series_id FROM series_id ;
exports.get_order_series_id_with_update = function () {	
	
	var sqlArray = [];
	
	var select_sql = []; 
	select_sql['sql'] = 'SELECT order_series_id FROM series_id';
	select_sql['params'] = [];		
		
	var update_sql = []; 
	update_sql['sql'] = 'UPDATE series_id SET order_series_id = order_series_id + 1';
	update_sql['params'] = [];
	
	sqlArray[0] = select_sql;
	sqlArray[1] = update_sql;
	
    return dao.patchTransaction(sqlArray);	
};

// 添加MyCard交易授權碼
exports.add_mycard_authglobal = function (auth_global,cp_order_id,extend_data) {	
	var sql = 'INSERT INTO mycard_authglobal(auth_global,cp_order_id,extend_data) VALUES(?,?,?)';
    var args = [auth_global, cp_order_id, extend_data];
    return dao.exeSql(sql, args);
};

// 取MyCard交易授權碼
exports.get_mycard_authglobal = function (cp_order_id) {
    var sql = 'SELECT * FROM mycard_authglobal WHERE cp_order_id=?';
    var args = [cp_order_id];
    return dao.exeSql(sql, args);
};

// 存储日志
exports.save_logs = function (logs) {
	var sql = 'INSERT INTO nodejs_logs(logs) VALUES(?)';
    var args = [logs];
    return dao.exeSql(sql, args);
};

// 添加MyCard充值记录
exports.add_mycard_recharge_record = function (mycard_tradeno,record_time,data) {	
	var sql = 'INSERT INTO mycard_recharge_record(mycard_tradeno,record_time,data) VALUES(?,?,?)';
    var args = [mycard_tradeno,record_time,data];
    return dao.exeSql(sql, args);
};

// 取MyCard充值记录
exports.get_mycard_recharge_record_by_tradeno = function (mycard_tradeno) {
    var sql = 'SELECT * FROM mycard_recharge_record WHERE mycard_tradeno=?';
    var args = [mycard_tradeno];
    return dao.exeSql(sql, args);
};

// 取MyCard充值记录
exports.get_mycard_recharge_record_by_datatime = function (StartDateTime, EndDateTime) {
    var sql = 'SELECT * FROM mycard_recharge_record WHERE record_time >= ? and record_time <= ?';
    var args = [StartDateTime, EndDateTime];
    return dao.exeSql(sql, args);
};

// 添加订单信息
exports.add_order_info= function (cp_order_id, order_info) {
	var sql = 'INSERT INTO order_info(cp_order_id,order_info) VALUES(?,?)';
    var args = [cp_order_id,order_info];
    return dao.exeSql(sql, args);
};

// 更新创建总的角色数量
exports.update_total_create_role_num = function (server_id) {	
    var sql='UPDATE server_list SET role_num = role_num + 1 where server_id = ?';
    var args = [server_id];
    return dao.exeSql(sql,args);	
};

// 更新总充值额度
exports.update_total_recharge_amount = function (server_id, amount) {	
    var sql='UPDATE server_list SET amount = amount + ? where server_id = ?';
    var args = [amount, server_id];
    return dao.exeSql(sql,args);	
};

// 更新每日充值额度
exports.update_daily_recharge_amount = function (server_id, datetime, amount) {	
     var sql='UPDATE statistics_server_detail SET amount = amount + ? where day_datetime = ? and server_id = ?';
    var args = [amount, datetime, server_id];
    return dao.exeSql(sql,args);	
};


// 更新总的充值角色数量
exports.update_total_recharge_role_num = function (server_id) {	
    var sql='UPDATE server_list SET recharge_role_num = recharge_role_num + 1 where server_id = ?';
    var args = [server_id];
    return dao.exeSql(sql,args);	
};

// 更新每日充值角色数量
exports.update_daily_recharge_role_num = function (server_id, datetime) {	
    var sql='UPDATE statistics_server_detail SET recharge_role_num = recharge_role_num + 1 where day_datetime = ? and server_id = ?';
    var args = [datetime, server_id];
    return dao.exeSql(sql,args);	
};


// 更新每日的角色数量
exports.update_daily_create_role_num = function (server_id, datetime, create_role_num) {	
	var select_sql = 'SELECT * FROM statistics_server_detail where day_datetime = ? and server_id = ?';	
    var select_args = [datetime,server_id];
    return dao.exeSql(select_sql,select_args).then(function(result){
		if(result.length > 0){
		    var update_sql='UPDATE statistics_server_detail SET create_role_num = create_role_num + 1 where day_datetime = ? and server_id = ?';
			var update_args = [datetime, server_id];
			return dao.exeSql(update_sql,update_args);	
		}else{
			var insert_sql = 'INSERT INTO statistics_server_detail(day_datetime,server_id,create_role_num) VALUES(?,?,?)';
			var insert_args = [datetime,server_id,create_role_num];
			return dao.exeSql(insert_sql, insert_args);
		}
    });	
};

// 更新角色活跃数量
exports.update_role_active_num = function (server_id, datetime) {	
	var sql='UPDATE statistics_server_detail SET active_num = active_num + 1 where day_datetime = ? and server_id = ?';
	var args = [datetime, server_id];
	return dao.exeSql(sql,args);	
};

// 更新新角色付费数量
exports.update_new_role_payment_num = function (server_id, datetime) {	
	var sql='UPDATE statistics_server_detail SET new_role_payment_num = new_role_payment_num + 1 where day_datetime = ? and server_id = ?';
	var args = [datetime, server_id];
	return dao.exeSql(sql,args);	
};

// 更新新角色付费金额
exports.update_new_role_payment_amount = function (server_id, datetime, add_amount) {	
	var sql='UPDATE statistics_server_detail SET new_role_payment_amount = new_role_payment_amount + ? where day_datetime = ? and server_id = ?';
	var args = [add_amount,datetime, server_id];
	return dao.exeSql(sql,args);	
};

// 取服务器列表
exports.get_server_list = function () {
    var sql = 'SELECT * FROM server_list';
    var args = [];
    return dao.exeSql(sql, args);
};

// 查询角色info
exports.get_role_info = function (account,server_id) {
    var sql = 'SELECT role_id,role_name,level FROM role where account = ? and server_id = ?';
    var args = [account,server_id];
    return dao.exeSql(sql, args);
};

// 查询角色info
exports.get_role_info_by_role_id = function (role_id) {
    var sql = 'SELECT role_id,role_name,level,vip_level FROM role where role_id = ?';
    var args = [role_id];
    return dao.exeSql(sql, args);
};


// 添加客户端事件 data_id,device_id,cp_channel_type,event_type,is_new,record_time, req_version_time,req_server_config_time,req_server_list_time
exports.add_client_event = function (table_name,data_id,device_id,cp_channel_type,event_type,is_new,ip,record_time,device_model,mem_size,app_version,os,record_time_ex) {
	var sql = util.format('INSERT INTO %s (data_id,device_id,cp_channel_type,event_type,is_new,ip,record_time,device_model,mem_size,app_version,os,record_time_ex) VALUES(?,?,?,?,?,?,?,?,?,?,?,?)',table_name);
    var args = [data_id,device_id,cp_channel_type,event_type,is_new,ip,record_time,device_model,mem_size,app_version,os,record_time_ex];
    return dao.exeSql(sql, args);
};


// 更新客户端事件
exports.update_client_event = function (table_name,data_id,device_id,event_type,cur_time,last_update_time) {
	var sql = util.format('UPDATE %s SET event_type = ?,last_update_cost_time = ? - record_time_ex, last_update_time = ? where data_id = ? and device_id = ?',table_name);
	var args = [event_type,cur_time,last_update_time,data_id,device_id];
	return dao.exeSql(sql,args);	
};

// 更新客户端事件
exports.update_client_event_ex = function (table_name,data_id,device_id,event_type,account,server_id,role_id) {
	var sql = util.format('UPDATE %s SET event_type = ?,account = ?,server_id = ?,role_id = ? where data_id = ? and device_id = ?',table_name);
	var args = [event_type,account,server_id,role_id,data_id,device_id];
	return dao.exeSql(sql,args);	
};

// 更新客户端事件(目前已不使用)
exports.update_client_event_ex_backup = function (table_name,data_id,device_id,event_type,key_name,key_value) {
	var sql = util.format('UPDATE %s SET event_type = ?,%s = ? where data_id = ? and device_id = ?',table_name, key_name);
	var args = [event_type,key_value,data_id,device_id];
	return dao.exeSql(sql,args);	
};


// 添加事件日志 'error_log',data_id,device_id,channel_type,event_type,device_model,mem_size,app_version,os,error_msg,cur_time
exports.add_event_statistics_error_log = function (table_name,data_id,device_id,cp_channel_type,event_type,device_model,mem_size,app_version,os,error_msg,record_time) {
	var sql = util.format('INSERT INTO %s (data_id,device_id,cp_channel_type,event_type,device_model,mem_size,app_version,os,error_msg,record_time) VALUES(?,?,?,?,?,?,?,?,?,?)',table_name);
    var args = [data_id,device_id,cp_channel_type,event_type,device_model,mem_size,app_version,os,error_msg,record_time];
    return dao.exeSql(sql, args);
};


// 取充值回调数据
exports.get_recharge_callback_data = function (uid) {
    var sql = 'SELECT * FROM recharge_callback_data WHERE uid = ?';
    var args = [uid];
    return dao.exeSql(sql, args);
};

// 添加充值回调数据
exports.add_recharge_callback_data= function (uid, callback_data,record_time) {
	var sql = 'INSERT INTO recharge_callback_data(uid,callback_data,record_time) VALUES(?,?,?)';
    var args = [uid,callback_data,record_time];
    return dao.exeSql(sql, args);
};


// 取ygame支付记录
exports.get_ygame_payment_record = function (role_id, payment_id) {
    var sql = 'SELECT * FROM ygame_payment_record WHERE role_id = ? and payment_id = ?';
    var args = [role_id,payment_id];
    return dao.exeSql(sql, args);
};

// 取ygame所有支付记录
exports.get_ygame_all_payment_record = function (role_id) {
    var sql = 'SELECT * FROM ygame_payment_record WHERE role_id = ?';
    var args = [role_id];
    return dao.exeSql(sql, args);
};

// 添加ygame支付记录
exports.add_ygame_payment_record = function (role_id, payment_id,record_time) {
	var sql = 'INSERT INTO ygame_payment_record(role_id,payment_id,record_time) VALUES(?,?,?)';
    var args = [role_id,payment_id,record_time];
    return dao.exeSql(sql, args);
};



// 新的一天事件
exports.new_day = function (datetime,today_start_time, yesterday_start_time, the_day_before_yesterday_start_time) {	

	// 服务器详情
	var select_sql = 'SELECT server_id FROM server_list';	
    var select_args = [];
    dao.exeSql(select_sql,select_args).then(function(result){		
		for(var i = 0; i < result.length; ++i)
		{
			var server_id = result[i].server_id;
			var insert_sql = 'INSERT INTO statistics_server_detail(day_datetime,server_id) VALUES(?,?)';
			var insert_args = [datetime,server_id];
			//dao.exeSql(insert_sql, insert_args);	
		}
    });	
	
	
	// 插入日报记录
	var insert_sql = 'INSERT INTO server_day_report(day_time) VALUES(?)';
    var insert_args = [datetime];
    dao.exeSql(insert_sql,insert_args).then(function(result){		
		// 角色数量
		{
			var sql = 'SELECT count(*) as create_role_num FROM role where create_time >= ? and create_time < ?';	
			var args = [yesterday_start_time,today_start_time];
			dao.exeSql(sql,args).then(function(result){	
				if(result.length > 0){
					var create_role_num = result[0].create_role_num;
					var sql='UPDATE server_day_report SET create_role_num = ? where day_time = ?';
					var args = [create_role_num,datetime];
					dao.exeSql(sql,args);						
				}
			});			
		}
		
		// 活跃人数
		{
			var sql = 'SELECT count(*) as active_role_num FROM role where last_update_time >= ? or create_time >= ?';	
			var args = [yesterday_start_time,yesterday_start_time];
			dao.exeSql(sql,args).then(function(result){	
				if(result.length > 0){
					var active_role_num = result[0].active_role_num;
					var sql='UPDATE server_day_report SET active_role_num = ? where day_time = ?';
					var args = [active_role_num,datetime];
					dao.exeSql(sql,args);						
				}
			});			
		}		

		
		// 充值额度
		{
			var sql = 'SELECT sum(amount) as amount FROM recharge_record where record_time >= ? and record_time < ?';	
			var args = [yesterday_start_time,today_start_time];
			dao.exeSql(sql,args).then(function(result){	
				if(result.length > 0){
					var amount = result[0].amount;
					var sql='UPDATE server_day_report SET payment_amount = ? where day_time = ?';
					var args = [amount,datetime];
					dao.exeSql(sql,args);						
				}
			});			
		}

		// 充值人数
		{
			var sql = 'SELECT COUNT(*) as payment_role_num FROM (SELECT DISTINCT role_id  FROM recharge_record WHERE record_time >= ?) temp';	
			var args = [yesterday_start_time];
			dao.exeSql(sql,args).then(function(result){	
				if(result.length > 0){
					var payment_role_num = result[0].payment_role_num;
					var sql='UPDATE server_day_report SET payment_role_num = ? where day_time = ?';
					var args = [payment_role_num,datetime];
					dao.exeSql(sql,args);						
				}
			});			
		}		
		
		// 次留
		{
			var sql = 'SELECT count(*) as create_role_num FROM role where create_time >= ? and create_time < ?';	
			var args = [the_day_before_yesterday_start_time,yesterday_start_time];
			dao.exeSql(sql,args).then(function(result){	
				if(result.length > 0){
					
					// 前天创建数量
					var create_role_num = result[0].create_role_num;
				
					var sql1 = 'SELECT count(*) as return_role_num FROM role where last_update_time >= ?';	
					var args1 = [yesterday_start_time];
					dao.exeSql(sql1,args1).then(function(result1){
						if(result1.length > 0){
							// 昨天回流数量
							var return_role_num = result1[0].return_role_num;
							
							// 次留
							var retention_rate_1 = return_role_num / create_role_num * 100;
							retention_rate_1 = retention_rate_1.toFixed(2);							
							var sql2 = 'UPDATE server_day_report SET retention_rate_1 = ? where day_time = ?';
							var args2 = [retention_rate_1,datetime];
							dao.exeSql(sql2,args2);									
						}						
					});			
				}
			});			
		}
    });		
};

// 取服务器列表
exports.get_server_list = function () {
    var sql = 'SELECT * FROM server_list';
    var args = [];
    return dao.exeSql(sql, args);
};


// 新的一天事件
exports.new_day_ex = function (datetime,today_start_time, yesterday_start_time) {	

	// 服务器详情
	var select_sql = 'SELECT server_id FROM server_list';	
    var select_args = [];
    dao.exeSql(select_sql,select_args).then(function(result){		
		for(var i = 0; i < result.length; ++i)
		{
			var server_id = result[i].server_id;
			var insert_sql = 'INSERT INTO statistics_server_detail(day_datetime,server_id) VALUES(?,?)';
			var insert_args = [datetime,server_id];
			//dao.exeSql(insert_sql, insert_args);	
		}
    });	
	
	
	// 插入日报记录
	var insert_sql = 'INSERT INTO server_day_report(day_time) VALUES(?)';
    var insert_args = [datetime];
    dao.exeSql(insert_sql,insert_args).then(function(result){		
		// 角色数量
		{
			var sql = 'SELECT count(*) as create_role_num FROM role where create_time >= ? and create_time < ?';	
			var args = [yesterday_start_time,today_start_time];
			dao.exeSql(sql,args).then(function(result){	
				if(result.length > 0){
					var create_role_num = result[0].create_role_num;
					var sql='UPDATE server_day_report SET create_role_num = ? where day_time = ?';
					var args = [create_role_num,datetime];
					dao.exeSql(sql,args);						
				}
			});			
		}
		
		// 活跃人数
		{
			var sql = 'SELECT count(*) as active_role_num FROM role where last_update_time >= ? or create_time >= ? and create_time < ?';	
			var args = [yesterday_start_time,yesterday_start_time,today_start_time];
			dao.exeSql(sql,args).then(function(result){	
				if(result.length > 0){
					var active_role_num = result[0].active_role_num;
					var sql='UPDATE server_day_report SET active_role_num = ? where day_time = ?';
					var args = [active_role_num,datetime];
					dao.exeSql(sql,args);						
				}
			});			
		}		

		
		// 充值额度
		{
			var sql = 'SELECT sum(amount) as amount FROM recharge_record where record_time >= ? and record_time < ?';	
			var args = [yesterday_start_time,today_start_time];
			dao.exeSql(sql,args).then(function(result){	
				if(result.length > 0){
					var amount = result[0].amount;
					var sql='UPDATE server_day_report SET payment_amount = ? where day_time = ?';
					var args = [amount,datetime];
					dao.exeSql(sql,args);						
				}
			});			
		}

		// 充值人数
		{
			var sql = 'SELECT COUNT(*) as payment_role_num FROM (SELECT DISTINCT role_id  FROM recharge_record WHERE record_time >= ?) temp';	
			var args = [yesterday_start_time];
			dao.exeSql(sql,args).then(function(result){	
				if(result.length > 0){
					var payment_role_num = result[0].payment_role_num;
					var sql='UPDATE server_day_report SET payment_role_num = ? where day_time = ?';
					var args = [payment_role_num,datetime];
					dao.exeSql(sql,args);						
				}
			});			
		}

		// 	
		
    });		
};











