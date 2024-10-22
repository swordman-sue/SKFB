var express = require('express');
var router = express.Router();

var log4j = require('../utils/log4j');
var httputils = require('../utils/httputils');
var authUtils = require('../utils/AuthUtils');

var userdao = require('../dao/userdao');
var taskdao = require('../dao/taskdao');
var syncDeptDao = require('../dao/SyncDeptDao');
var taskservice = require('../service/taskservice');
var authlogDao = require('../dao/AuthlogDao');
var config = require('../utils/config');



//点击菜单
router.all('/entrance', function (req, res, next) {
    var code = (req.query.code == undefined ? req.body.code : req.query.code);
    if (code == undefined) {
        res.render('exception', {msg: "未获取到授权码code"});
        return;
    }
    authUtils.getUser(code).then(function (user) {
        if(user.code == config.auth.uselessCode){   //无效的code
            res.render('exception', {msg: user.message});
            return;
        }if(user.code == config.auth.uselessTokenCode){ //无效的token！
            res.render('exception', {msg: user.message});
            return;
        }
        var openId = user.id;           //userId
        var userName = user.realname;   //姓名
        var tel = user.mobile;          //手机
        var unit = user.enterprisename; //企业名称
        var dept = user.departmentname; //部门名称
        var accountId = user.accountId;   //公众号id
        var enterpriseid = user.enterpriseid;//企业id
        var departmentid = user.departmentid;//部门id

        var access_token = user.access_token;
        var expires = user.expires;
        var refresh_token = user.refresh_token;
        //console.log(accountId + "/"+user.access_token);
        config.token.access_token = access_token;
        config.token.expires = expires;
        config.token.refresh_token = refresh_token;

        if (accountId == null) {
            accountId = 'gh_null';
        }
        if (openId == null || userName == null || enterpriseid == null) {
            console.log("未获取到用户信息");
            log4j.dateFilelogger.info("======="+code);
            log4j.dateFilelogger.info(user);
            res.render('exception', {msg: "未获取到用户信息"});
            return;
        }
        // var baseUrl = '/html5src';
        var baseUrl = '/html5';
        return userdao.findByOpenId(openId).then(function (users) {
            if (users.length <= 0) {
                //throw new Error('未找到用户');
                return syncDeptDao.findPathId(enterpriseid).then(function (depts) {
                    if (depts.length > 0) {
                        var dept0 = depts[0].pathById;
                        var ids = dept0.split(".");
                        var firstId = ids[0];
                        var secondId = ids[1];
                        var thirdId = ids[2];
                        if (secondId == undefined) {
                            secondId = firstId;
                            thirdId = firstId;
                        } else if (thirdId == undefined) {
                            thirdId = secondId;
                        }
                        return userdao.add(openId, userName, tel, unit, dept, firstId,
                            secondId, thirdId, enterpriseid, departmentid, accountId,access_token,expires,refresh_token).then(function (rst) {
                            var userId = rst.insertId;
                            res.redirect(baseUrl + "/main?userId=" + userId + "&entry=1" + "&accountId=" + accountId+"&enterpriseid="+enterpriseid);
                        });
                    } else {
                        //这里做同步
                        var des = '添加用户信息失败，用户单位信息不存在,需要同步数据';
                        var params = 'openId='+openId +',' + 'userName='+userName +',' + 'tel='+tel +',' + 'unit='+unit +',' + 'dept='+dept +','
                            + 'firstId='+0 +',' + 'secondId='+0 +',' + 'thirdId='+0 +',' + 'enterpriseid='+enterpriseid +','
                            + 'departmentid='+departmentid +',' + 'accountId='+accountId;
                        return authlogDao.add(des,params).then(function(){
                            res.render('exception', {msg: "failed to obtain user enterprise information.."});
                        });
                    }
                });
            }

            var userId = users[0].id;
            return syncDeptDao.findPathId(enterpriseid).then(function (depts)
            {
                if (depts.length > 0) {
                    var dept0 = depts[0].pathById;
                    var ids = dept0.split(".");
                    var firstId = ids[0];
                    var secondId = ids[1];
                    var thirdId = ids[2];
                    if (secondId == undefined) {
                        secondId = firstId;
                        thirdId = firstId;
                    } else if (thirdId == undefined) {
                        thirdId = secondId;
                    }
                    return userdao.update(tel, unit, dept, firstId, secondId, thirdId, enterpriseid, departmentid, accountId,access_token,userId,expires,refresh_token).then(function (rst)
                    {
                        /*start 状态判断 start*/
                        return taskdao.findByUserIdAndState(userId, 0).then(function (unstart_tasks)
                        {   //未开始的任务
                            if (unstart_tasks.length > 0) {
                                var taskId = unstart_tasks[0].id;
                                //res.send("去邀请好友页面");//entry=3
                                res.redirect("/game/inviteFriends?taskId=" + taskId + "&accountId=" + accountId);
                                return;
                            }
                            return taskdao.findByUserIdAndState(userId, 1).then(function (ongoing_tasks) {//进行中的任务
                                if (ongoing_tasks.length > 0) {
                                    var taskId = ongoing_tasks[0].id;
                                    var unit = ongoing_tasks[0].unit;
                                    //res.send("去签到页面");  //entry=2
                                    res.redirect(baseUrl + "/main?userId=" + userId + "&taskId=" + taskId + "&unit=" + "" + "&entry=2" + "&accountId=" + accountId+"&enterpriseid="+enterpriseid);
                                    return;
                                }
                                return taskdao.findByUserIdAndState(userId, 2).then(function (stop_tasks) {//被暂停的任务
                                    if (stop_tasks.length > 0) {
                                        var taskId = stop_tasks[0].id;
                                        //res.send("去选择页面（选择重启或新建）");//entry=4
                                        taskdao.findSignNum(taskId).then(function(result){
                                            if(result.length < 0){
                                                var signNum = 0;
                                                res.redirect(baseUrl + "/main?userId=" + userId + "&taskId=" + taskId + "&entry=4" + "&accountId=" + accountId+"&enterpriseid="+enterpriseid+"&signNum="+signNum);
                                                return;
                                            }else{
                                                var task = result[0];
                                                var sign_in_num = task.sign_in_num;
                                                var sign_in_num2 = task.sign_in_num2;
                                                var signNum = Math.min(sign_in_num, sign_in_num2);
                                                res.redirect(baseUrl + "/main?userId=" + userId + "&taskId=" + taskId + "&entry=4" + "&accountId=" + accountId+"&enterpriseid="+enterpriseid+"&signNum="+signNum);
                                                return;
                                            }
                                        });
                                        return;
                                    }
                                    //res.send("去新建页面");  //entry=1
                                    res.redirect(baseUrl + "/main?userId=" + userId + "&entry=1" + "&accountId=" + accountId+"&enterpriseid="+enterpriseid);
                                    return;
                                });
                            });
                        });
                        /*end 状态判断 end*/
                    });
                } else {
                    //这里做同步
                    var des = '更新用户信息失败，用户单位信息不存在,需要同步数据';
                    var params = 'openId='+openId +',' + 'userName='+userName +',' + 'tel='+tel +',' + 'unit='+unit +',' + 'dept='+dept +','
                        + 'firstId='+0 +',' + 'secondId='+0 +',' + 'thirdId='+0 +',' + 'enterpriseid='+enterpriseid +','
                        + 'departmentid='+departmentid +',' + 'accountId='+accountId;
                    return authlogDao.add(des,params).then(function(){
                        res.render('exception', {msg: "failed to obtain user enterprise information.."});
                    });
                }
            });
        })
    }).catch(function (err) {
        log4j.dateFilelogger.error("failed to obtain user information.." + err);
        res.render('exception', {msg: "failed to obtain user information.." + err});
    });
});


//接受邀请
router.all('/befrends', function (req, res) {
    var taskCode = (req.query.taskCode == undefined ? req.body.taskCode : req.query.taskCode);
    var userId = (req.query.userId == undefined ? req.body.userId : req.query.userId);
    taskCode = trim(taskCode);
    return userdao.findByUserId(userId).then(function (users) {
        return userdao.findByTaskCode(taskCode).then(function (self_users) {
            if (self_users.length <= 0) {
                res.json({statusCode: 300, message: '验证码不存在'});
                log4j.dateFilelogger.error("验证码不存在/game/befrends?userId="+userId+"&taskCode="+taskCode);
            } else {
                var self = self_users[0];
                if (self.id == userId) {
                    res.json({statusCode: 300, message: '自己不能成为自己的搭档'});
                } else if (self.state != 0) {
                    res.json({statusCode: 300, message: '您的好友目标或正在进行'});
                }
                else {
                    taskservice.receiveInvite(self.taskId,self.isCompleted, users[0].openId, res);//配对
                }
            }
        });
    }).catch(function (err) {
        log4j.dateFilelogger.error("failed to obtain user information.." + err);
        res.json({statusCode: 300, message: '服务忙，请稍后重试'});
    });
});

function trim(s){
    return s.replace(/(^\s*)|(\s*$)/g, "");
}

module.exports = router;

