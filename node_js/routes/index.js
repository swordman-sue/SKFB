var express = require('express');
var router = express.Router();

var when = require('when');
var dao = require('../mysql/dao');
var taskdao = require('../dao/taskdao');
var msgdao = require('../dao/msgdao');
var userdao = require('../dao/userdao');
var tokendao = require('../dao/tokendao');
var taskservice = require('../service/taskservice');

var Md5 = require('../utils/Md5');
var log4j = require('../utils/log4j');
var config = require('../utils/config');

var authUtils = require('../utils/AuthUtils');
var base64 = require('../utils/base64.js').Base64;

var MixCrypto = require('../utils/MixCrypto');
var mixCrypto = new MixCrypto({});
var join = require('path').join;

function trim(s){
    return s.replace(/(^\s*)|(\s*$)/g, "");
}

router.get('/', a, function (req, res, next) {
   res.render('exception', {msg: '欢迎来到好习惯,习惯是一种顽强而巨大的力量，它可以主宰人生。”是的，没有人天生就拥有超人的智慧，成功的捷径恰恰在于貌似不起眼的良好习惯,赶快加入我们吧'})
});


function a(req, res, next) {
    next();
}

router.get('/change',function (req, res, next) {
//   var sql = 'SELECT t.id ,DATEDIFF(CURRENT_DATE(),t.createTime) days,DATEDIFF(CURRENT_DATE(),t.lastSigninTime) days2,t.totalSignNum FROM t_tasks t WHERE t.state = 1'
//   var args = [];
//   return dao.exeSql(sql,args).then(function(tasks){
//        for(var i = 0;i<tasks.length;i++){
//            var t = tasks[i];
//            b(t);
//        }
//   });

    var sql = 'SELECT t.joinTaskId FROM t_tasks t WHERE t.state =1 AND t.isCompleted = 2';
    var args = [];
    return dao.exeSql(sql,args).then(function(tasks){
        for(var i = 0 ;i<tasks.length;i++){
            var t = tasks[i];
            b(t);
        }
    });
});

function b(t){
    var sql ='UPDATE t_tasks SET isCompleted = 2 WHERE id = ?';
    var args = [t.joinTaskId];
    dao.exeSql(sql,args);

//    if(t.days2 == 0){//今天已经签到
//        if(t.totalSignNum > t.days){
//            var sql ='UPDATE t_tasks SET isCompleted = 1 WHERE id = ?';
//            var args = [t.id];
//            dao.exeSql(sql,args);
//        }else{
//
//        }
//    }else{//今天没有签到
//        if(t.totalSignNum == t.days){
//            var sql ='UPDATE t_tasks SET isCompleted = 1 WHERE id = ?';
//            var args = [t.id];
//            dao.exeSql(sql,args);
//        }else{
//
//        }
//    }
}

/**
 * 欢迎页面
 */
router.get('/welcome',function (req, res, next) {
    res.render('welcome');
});

/**
 * 异常响应页面
 */
router.get('/exception', function (req, res, next) {
    res.render('exception', {msg: "Server busy, please try again later"});
});

/**
 * 浏览器异常响应页面
 */
router.get('/browserError', function (req, res, next) {
    res.render('exception', {msg: "无权访问，请在微信端打开"});
});

/**
 * 获取南网组织架构
 */
router.get('/getDepartMentInfo', function (req, res, next) {
    tokendao.getCacheToken().then(function(token){
        authUtils.getDepartMentInfo(token).then(function(result){
            res.send(result);
        });
    });
});


/**
 * 任务详情页面（（可分享））
 */
router.all('/users/userSignInCase', function (req, res, next) {
    var userId, taskId, accountId;
    userId = (req.query.userId == undefined ? req.body.userId : req.query.userId);
    taskId = (req.query.taskId == undefined ? req.body.taskId : req.query.taskId);
    accountId = (req.query.accountId == undefined ? req.body.accountId : req.query.accountId);

    taskservice.userSignInCase(userId, taskId, accountId, res);
});

router.all('/game/shareSignIncase', function (req, res, next) {
    var params = req.query.params;
    var paramArray = params.split('|');
    var content, sign_in_num, cardId, sign_in_num2, total_integral, integral, accountId, totalSignNum;
    content = mixCrypto.decrypt(paramArray[0]);
    sign_in_num = paramArray[1];
    cardId = paramArray[2];
    sign_in_num2 = paramArray[3];
    total_integral = paramArray[4];
    integral = paramArray[5];
    accountId = paramArray[6];
    totalSignNum = paramArray[7];
    var _data = {content: content,
        sign_in_num: sign_in_num,
        cardId: cardId,
        sign_in_num2: sign_in_num2,
        total_integral: total_integral,
        integral: integral,
        totalSignNum: totalSignNum,
        appId: "", timestamp: "", nonceStr: "", signature: "", url: "", imgUrl: "", domain: config.domain, port: config.port
    };
    if (accountId == 'gh_null' || accountId == '' || accountId == undefined) {
        res.render('shareSignIncase', _data);
    } else {
        var url = config.auth.registeredUriForJssdk + req.url;
        authUtils.getJssdkTicket(accountId, encodeURIComponent(url)).then(function (jsapiSignature) {
            if (jsapiSignature.code == -1) {
                //生成签名出现异常
                //res.render('exception', {msg: '生成签名出现异常'});
                res.render('shareSignIncase', _data);
            } else if (jsapiSignature.code == 500) {
                //系统异常
                //res.render('exception', {msg: '系统异常'});
                res.render('shareSignIncase', _data);
            } else {
                var picUrl = '';
                picUrl = 'http://' + config.domain + ':' + config.port + '/img/mw.png';
                _data = {content: content,
                    sign_in_num: sign_in_num,
                    cardId: cardId,
                    sign_in_num2: sign_in_num2,
                    total_integral: total_integral, integral: integral, totalSignNum: totalSignNum,
                    appId: jsapiSignature.data.appId,
                    timestamp: jsapiSignature.data.timestamp,
                    nonceStr: jsapiSignature.data.noncestr,
                    signature: jsapiSignature.data.signature,
                    url: jsapiSignature.data.url,
                    imgUrl: picUrl,
                    domain: config.domain,
                    port: config.port
                };
                res.render('shareSignIncase', _data);
            }
        });
    }
});

//游戏入口
router.all('/html5src/main', function (req, res) {
    if (req.headers['user-agent'].indexOf("MicroMessenger") != -1) {//微信
        res.render('game', {'params': req.query});
        return;
    }
    res.redirect("/browserError");
});

router.all('/html5/main', function (req, res) {
    //res.render('game', {'params': req.query});
    if (req.headers['user-agent'].indexOf("MicroMessenger") != -1) {//微信
        res.render('game', {'params': req.query});
        return;
    }
    res.redirect("/browserError");
});

/**
 * 去邀请页面（可分享）
 */
router.all('/game/inviteFriends', function (req, res) {
    var taskId = (req.query.taskId == undefined ? req.body.taskId : req.query.taskId);
    var accountId = (req.query.accountId == undefined ? req.body.accountId : req.query.accountId);
    var params = taskId + '|' + accountId;
    res.redirect(config.auth.registeredUriForJssdk + '/game/inviteFriend?params=' + params);
});

router.all('/game/inviteFriend', function (req, res) {
    var params = req.query.params;
    var paramArray = params.split('|');
    var taskId = paramArray[0];
    var accountId = paramArray[1];

    var _data;
    if (accountId == 'gh_null' || accountId == '' || accountId == undefined) {
        taskdao.findSimpleById(taskId).then(function (tasks) {
            var task = tasks[0];
            _data ={taskId: task.id,
                taskCode: task.taskCode,
                content: task.content,
                appId: "",
                timestamp: "",
                nonceStr: "",
                signature: "",
                url: "",
                imgUrl: "",
                domain: config.domain,
                port: config.port
            };
            res.render('inviteFriends', _data);
        });
    } else {
        taskdao.findSimpleById(taskId).then(function (tasks) {
            var task = tasks[0];
            var url = config.auth.registeredUriForJssdk + req.url;
            authUtils.getJssdkTicket(accountId, encodeURIComponent(url)).then(function (jsapiSignature) {
                if (jsapiSignature.code == -1) {
                    //生成签名出现异常
                    //res.render('exception', {msg: '生成签名出现异常'});
                    _data ={taskId: task.id,
                        taskCode: task.taskCode,
                        content: task.content,
                        appId: "",
                        timestamp: "",
                        nonceStr: "",
                        signature: "",
                        url: "",
                        imgUrl: "",
                        domain: config.domain,
                        port: config.port
                    };
                    res.render('inviteFriends', _data);
                } else if (jsapiSignature.code == 500) {
                    //系统异常
                    //res.render('exception', {msg: '系统异常'});
                    _data ={taskId: task.id,
                        taskCode: task.taskCode,
                        content: task.content,
                        appId: "",
                        timestamp: "",
                        nonceStr: "",
                        signature: "",
                        url: "",
                        imgUrl: "",
                        domain: config.domain,
                        port: config.port
                    };
                    res.render('inviteFriends', _data);
                } else {
                    var picUrl = 'http://' + config.domain + ':' + config.port + '/img/mw.png';
                    _data = {taskId: task.id,
                        taskCode: task.taskCode,
                        content: task.content,
                        appId: jsapiSignature.data.appId,
                        timestamp: jsapiSignature.data.timestamp,
                        nonceStr: jsapiSignature.data.noncestr,
                        signature: jsapiSignature.data.signature,
                        url: jsapiSignature.data.url,
                        imgUrl: picUrl,
                        domain: config.domain,
                        port: config.port
                    };
                    res.render('inviteFriends', _data);
                }
            });
        });
    }
});

/**
 * 玩家B去输入邀请码
 * useless
 */
router.all('/game/toReceive', function (req, res, next) {
    var userId = (req.query.userId == undefined ? req.body.userId : req.query.userId);
    var accountId = (req.query.accountId == undefined ? req.body.accountId : req.query.accountId);
    res.render('enterCode', {userId: userId, accountId: accountId});
});

/**
 * 去签到页面
 */
router.all('/game/goSignin', function (req, res, next) {
    var userId = (req.query.userId == undefined ? req.body.userId : req.query.userId);
    //var accountId = (req.query.accountId == undefined ? req.body.accountId : req.query.accountId);
    return taskdao.findByUserIdAndState(userId, 1).then(function (ongoing_tasks) {//进行中的任务
        if (ongoing_tasks.length > 0) {
            var taskId = ongoing_tasks[0].id;
            var unit = ongoing_tasks[0].unit;
            var enterpriseid = ongoing_tasks[0].enterpriseid;
            var accountId = ongoing_tasks[0].accountId;
            res.redirect("/html5/main?userId=" + userId + "&taskId=" + taskId + "&unit=" + "" + "&entry=2" + "&accountId=" + accountId+"&enterpriseid="+enterpriseid);
            return;
        }else{
            res.render('exception', {msg: "出错了，请退出游戏，重新进入"});
        }
    });
});

/**
 *玩家消息列表
 * useless
 */
router.all('/game/message', function (req, res, next) {
    var userId = (req.query.userId == undefined ? req.body.userId : req.query.userId);
    msgdao.findByUserId(userId).then(function (messages) {
        res.render('message', {messages: messages});
    });
});


/**
 * 登陆（查看排行榜使用）
 * useless
 */
router.all('/game/dologin', function (req, res, next) {
    var username = req.body.username;
    var password = req.body.password;
    var md5 = new Md5();
    password = md5.encrypt(password);
    userdao.login(username, password).then(function (uLogins) {
        if (uLogins.length > 0) {
            req.session.username = username;
            res.json({statusCode: 200, message: 'loginsuccess'});
        } else {
            res.json({statusCode: 300, message: '用户名或密码错误'});
        }
    }).catch(function (err) {
        log4j.dateFilelogger.error("登录异常" + err);
        res.json({statusCode: 300, message: '服务忙，请稍后重试'});
    });
});

/**
 * 登陆查看排行榜
 * useless
 */
router.all('/game/login', function (req, res, next) {
    res.render('login');
});

/**
 * 排行榜
 * useless
 */
router.all('/game/ranklist', function (req, res, next) {
    if (req.session.username == undefined) {
        res.render('login');
        return;
    }
    res.render('ranklist');
});

module.exports = router;
