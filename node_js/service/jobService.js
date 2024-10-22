var when = require('when');
var log4j = require('../utils/log4j');
var config = require('../utils/config');
var authUtils = require('../utils/AuthUtils');
var dao = require('../mysql/dao');
var taskdao = require('../dao/taskdao');
var msgdao = require('../dao/msgdao');
var userdao = require('../dao/userdao');
var tokendao = require('../dao/tokendao');


var failNoticeId;
var day = 0;
/**
 * 检查昨天没有签到的任务
 */
exports.failNotice = function () {
    var currentDate = new Date();
    //if (day != currentDate.getDate()) {
     if(true){
        var h = currentDate.getHours();
        var minutes = currentDate.getMinutes();
        if (h >= 0 && h < 1 && ((minutes % 20 == 1))) {
            log4j.dateFilelogger.error("检查失败的任务.." + h+":"+minutes);
            day = currentDate.getDate();
            failTask().then(function (failtasks) {
                for (var i = 0; i < failtasks.length; i++) {
                    var t = failtasks[i];
                    a(t.joinTaskId, t.id);
                }
            }, function (err) {
                log4j.dateFilelogger.error("检查失败的任务异常.." + err);
            });
        }
    }
    clearTimeout(failNoticeId);
    failNoticeId = setTimeout(exports.failNotice, 1000 * 60);
};

var failSendId;
var failSendFlag = 0;
/**
 * 推送给用户失败的任务通知(每天早上9点)
 */
exports.failSend = function () {
    if (failSendFlag != new Date().getDate())
    {
        var h = new Date().getHours();
        if (h >= 9 && h < 10)
        {
            failSendFlag = new Date().getDate();
            unSendMsg().then(function(msgs){
                for(var i = 0;i<msgs.length;i++){
                    var msgBean = msgs[i];
                    console.log("推送给："+msgBean.userId);
                    sendFailMsg(msgBean);
                }
            },function (err) {
                log4j.dateFilelogger.error("检查未推送的消息异常" + err);
            });
        }
    }
    clearTimeout(failSendId);
    failSendId = setTimeout(exports.failSend, 1000 * 60);
};


var signNoticeId;
var signFlag = 0;
/**
 * 检查今天还没有签到的任务（每天晚上9点）
 */
exports.signNotice = function () {
    if (signFlag != new Date().getDate())
    {
        var h = new Date().getHours();
        if (h >= 21 && h < 22)
        {
            signFlag = new Date().getDate();

            signInTask().then(function (unSignInTaskList) {
                    for(var i = 0;i<unSignInTaskList.length;i++){
                        var task = unSignInTaskList[i];
                        var msg = '你今天还没有签到哦，如果任务中断你和你的搭档都将无法得到丰厚的积分奖励，不要让你和你搭档的努力白费哦。';
                        //console.log(task.userId + '未签到');
                        sendMsg(task.userId,msg,5);
                    }
                },function (err) {
                    log4j.dateFilelogger.error("检查失败的任务异常.." + err);
                }
            );
        }
    }
    clearTimeout(signNoticeId);
    signNoticeId = setTimeout(exports.signNotice, 1000 * 60);
};


var handleNewTaskWithoutFriendId;
var handleNewTaskWithoutFriendFlag = 0;
/**
 * 处理无效的新建任务
 */
exports.handleNewTaskWithoutFrien = function () {
    if (handleNewTaskWithoutFriendFlag != new Date().getDate())
    {
        var h = new Date().getHours();
        if (h >= 2 && h < 24)
        {
            handleNewTaskWithoutFriendFlag = new Date().getDate();

            scanNewTaskWithoutFriend().then(function (tasks) {
                    for(var i = 0;i<tasks.length;i++){
                        var taskId = tasks[i].id;
                        deleteByTaskId(taskId);
                        log4j.dateFilelogger.info("删除无效任务：taskCode="+tasks[i].taskCode +",userId="+tasks[i].userId +",content="+tasks[i].content+",taskId="+taskId);
                    }
                },function (err) {
                    log4j.dateFilelogger.error("检查失败的任务异常.." + err);
                }
            );
        }
    }
    clearTimeout(handleNewTaskWithoutFriendId);
    handleNewTaskWithoutFriendId = setTimeout(exports.handleNewTaskWithoutFrien, 1000 * 60);
};

var handleResatrtTaskWithoutFriendId;
var handleResatrtTaskWithoutFriendFlag = 0;
/**
 * 处理无效的重启任务
 */
exports.handleResatrtTaskWithoutFrien = function () {
    if (handleResatrtTaskWithoutFriendFlag != new Date().getDate())
    {
        var h = new Date().getHours();
        if (h >= 1 && h < 24)
        {
            handleResatrtTaskWithoutFriendFlag = new Date().getDate();

            scanRestartTaskWithoutFriend().then(function (tasks) {
                    for(var i = 0;i<tasks.length;i++){
                        var taskId = tasks[i].id;
                        updaeByTaskId(taskId);
                    }
                },function (err) {
                    log4j.dateFilelogger.error("检查无效重启任务异常.." + err);
                }
            );
        }
    }
    clearTimeout(handleResatrtTaskWithoutFriendId);
    handleResatrtTaskWithoutFriendId = setTimeout(exports.handleResatrtTaskWithoutFrien, 1000 * 60);
};


/**
 * 失败的任务
 * @returns {*}
 */
function failTask() {
    var sql = 'SELECT t.* FROM t_tasks t WHERE DATEDIFF(CURRENT_DATE(),t.lastSigninTime) > 1 AND t.state=1';
    var args = [];
    return dao.exeSql(sql, args);
};

/**
 * 待推送的消息
 * @returns {*}
 */
function unSendMsg() {
    var sql = 'SELECT * FROM u_msg WHERE sended = 1 AND type =2';
    var args = [];
    return dao.exeSql(sql, args);
};

function sendFailMsg(msgBean){
    msgdao.sendMsgTo(msgBean.userId,msgBean.content,msgBean.type).then(function(){
        msgdao.updateMsg(msgBean.id);
    });
}

function deleteByTaskId(taskId){
    taskdao.deleteByTaskId(taskId).then(function(){},function(err){
        log4j.dateFilelogger.error("删除无效新建任务失败taskId="+taskId+";" + err);
    });
}

function updaeByTaskId(taskId){
    taskdao.updateState(3,taskId).then(function(){
    }).catch(function(err){
        log4j.dateFilelogger.error("重置无效重启任务失败taskId="+taskId+";" + err);
    });
}

function a(joinTaskId,taskId){
    taskdao.findById2(joinTaskId).then(function (tasks) {
        if(tasks.length > 0){
            var task = tasks[0];
            var days = task.days;// 间隔的天数
            var userIdA = task.userIdA;
            var taskIdA = task.taskIdA;
            var userIdB = task.userIdB;
            var taskIdB = task.taskIdB;
            var content = task.content;
            //console.log(userIdA+"/"+taskIdA+","+userIdB+"/"+taskIdB+"."+days);
            if (days > 1) {//玩家B也没签到
                taskdao.updateStateAndCompleted(2, taskIdA).then(function () {
                    var pause_msg = config.msg.pause_msg;
                    pause_msg = pause_msg.replace('xxx',content);
                    //console.log("----------------1");
                    msgdao.addMsg(userIdA, pause_msg, config.msg.pauseMsgFlag).then(function () {
                    });
                });
            } else {
                taskdao.updateStateAndCompletedForTwo(2, taskId, joinTaskId).then(function () {
                    // console.log("----------------2");
                    var pause_msg = config.msg.pause_msg;
                    pause_msg = pause_msg.replace('xxx',content);
                    msgdao.addMsg(userIdA,pause_msg, config.msg.pauseMsgFlag).then(function () {
                        msgdao.addMsg(userIdB, pause_msg, config.msg.pauseMsgFlag).then(function(){
                        });
                    });
                });
            }
        }else{
            console.log("----------------3");
        }
    });
}

/**
 * 未签到的任务
 * @returns {*}
 */
function signInTask() {
    var sql = 'SELECT t.id taskId,t.userId,t.lastSigninTime,t.state,t.content  FROM t_tasks t WHERE DATEDIFF(CURRENT_DATE(),t.lastSigninTime) != 0 AND t.state=1';
    var args = [];
    return dao.exeSql(sql, args);
};

function sendMsg(userId,msg,type){
    msgdao.add(userId,msg,5).then(function(){
    });
}

/**
 * 扫描无效的新建任务
 * --新建第二天没有邀请到搭档--
 * @returns {*}
 */
function scanNewTaskWithoutFriend(){
    var sql = 'SELECT * FROM t_tasks t WHERE t.state = 0 AND t.joinTaskId = 0 AND t.isCompleted = 1 AND DATEDIFF(CURRENT_DATE(),t.createTime) >= 1';
    var args = [];
    return dao.exeSql(sql, args);
}

/**
 * 扫描无效的重启任务
 * --重启第二天没有邀请到搭档--
 * @returns {*}
 */
function scanRestartTaskWithoutFriend(){
    var sql = 'SELECT * FROM t_tasks t WHERE t.state = 0 AND t.joinTaskId != 0 AND t.isCompleted != 1 AND DATEDIFF(CURRENT_DATE(),t.restartTime) >= 1';
    var args = [];
    return dao.exeSql(sql, args);
}


