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
var game = require('./game');
var global_english = require('./global_english');
var check_user_info = require('./check_user_info');
var config = require('./utils/config');


var Md5 = require('./utils/Md5');
var md5=new Md5();

//var multer = require('multer'); 
app.use(bodyParser.json()); // for parsing application/json

//app.use('/skyx', express.static(__dirname + '/public/skyx'));

var server = app.listen(20001, function () {
  var host = server.address().address;
  var port = server.address().port;

  console.log('login app listening at http://%s:%s', host, port);
  console.log('dir_name:%s', __dirname);
});


// 测试使用
app.get('/test', function (req, res) {
	var ip = common_logic.get_client_ip_ex(req);
	console.log('ip:%s',ip);	
	res.send(ip);
});


// 登录检验
app.post('/check_user_info', function (req, res) {	
	check_user_info['check_user_info'](req, res);
});	

