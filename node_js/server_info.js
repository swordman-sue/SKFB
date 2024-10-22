var https = require('https');
var http = require('http');
var path = require('path');
var fs = require('fs');
var util = require('util');
var mysql  = require('mysql'); 


// 配置链接数据信息
var dbconfig=
[
	// 测试服1
	{
		  host     : 'localhost',       
		  user     : 'root',              
		  password : 'debug',       
		  port: '3306',                   
		  database: 'skfb' 
	},
]

// 链接列表
var connection_list = [];

function handleDisconnect(index) {
    connection_list[index] = mysql.createConnection(dbconfig[index]); // Recreate the connection, since
    // the old one cannot be reused.

    connection_list[index].connect(function(err) {              // The server is either down
        if(err) {                                     // or restarting (takes a while sometimes).
            console.log('error when connecting to db:', err);
            setTimeout(handleDisconnect, 2000, index); // We introduce a delay before attempting to reconnect,
        }                                     // to avoid a hot loop, and to allow our node script to
    });                                     // process asynchronous requests in the meantime.
    // If you're also serving http, display a 503 error.
    connection_list[index].on('error', function(err) {
        console.log('db error', err);
        if(err.code === 'PROTOCOL_CONNECTION_LOST') { // Connection to the MySQL server is usually
            handleDisconnect(index);                         // lost due to either server restart, or a
        } else {                                      // connnection idle timeout (the wait_timeout
            throw err;                                  // server variable configures this)
        }
    });
}

handleDisconnect(0);

function getConnection(index){
    //console.log("connection.state="+connection.state);
    if(connection_list[index]==undefined || connection_list[index].state=='protocol_error'){
        handleDisconnect(index);
    }
    return connection_list[index];
};


// 获取客户端IP
function get_client_ip(req) {
        return req.headers['x-forwarded-for'] ||
        req.connection.remoteAddress ||
        req.socket.remoteAddress ||
        req.connection.socket.remoteAddress;
};

exports.get_server_list = function (req, res, index) {
		
	var resp_data = {
	code:200,
	str:"操作成功",
	msg:"",
	ip:get_client_ip(req)
	};
		
	// {"id":1, "name":"群雄逐鹿", "status":1, "recommend":1, "open_time":"1539826200"},	
	var myconnect = getConnection(index);
	 
	var sql = 'SELECT * FROM  server_list';
	//查
	myconnect.query(sql,function (err, result) {
			if(err){
			  console.log('[SELECT ERROR] - ',err.message);
			  return;
			}
			
			var server_list = [];
			for(var i = 0; i < result.length; ++i){
				var server_data = {
					id:result[i].server_id,
					name:result[i].name,
					status:result[i].status,	
					recommend:result[i].recommend,
					open_time:result[i].open_time
				};
				server_list[i] = server_data;
			}			
			
			
		var msg = {server_list:server_list};
		resp_data.msg = msg;
		res.send(resp_data);			
	}); 
	//myconnect.end();	
}












