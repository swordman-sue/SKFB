var when = require('when');
var log4j = require('../utils/log4j');
var config = require('../utils/config');
var Random = require('../utils/Random');

var taskdao = require('../dao/taskdao');
var userdao = require('../dao/userdao');
var invitedao = require('../dao/invitedao');
var signdao = require('../dao/signdao');
var typedao = require('../dao/typedao');
var msgdao = require('../dao/msgdao');

var base64 = require('../utils/base64.js').Base64;

var MixCrypto = require('../utils/MixCrypto');
var mixCrypto = new MixCrypto({});

var dao = require('../mysql/dao');

//新建
exports.newTaskService = function (userId, type, content, cardId, res) {
    taskdao.findOngoingTaskByUserId(userId).then(function (result2) {
        if (result2.length > 0) {
            res.json({statusCode: 300, message: '有进行中的目标等着你去做，新建失败'});
        } else {
            //新建
            return taskdao.findByContent_UserId(userId, content).then(function (result) {
                if (result.length > 0) {
                    res.json({statusCode: 300, message: '相同的目标有一个就够了，新建失败'});
                } else {
                    return typedao.findByName(type).then(function (result4) {
                        if (result4.length > 0) {
                            var random = new Random();
                            var taskCode = random.getRandom(8);
                            return taskdao.addTask(userId, type, content, cardId, taskCode).then(function (result3) {
                                var taskId = result3.insertId;
                                res.json({statusCode: 200, taskId: taskId });
                                //推送
                            });
                        } else {
                            var typeObj = {name: type, role: 1};
                            return typedao.add(typeObj).then(function () {
                                var random = new Random();
                                var taskCode = random.getRandom(8);
                                return taskdao.addTask(userId, type, content, cardId, taskCode).then(function (result3) {
                                    var taskId = result3.insertId;
                                    res.json({statusCode: 200, taskId: taskId });
                                    //推送
                                });
                            });
                        }
                    });
                }
            });
        }
    }).catch(function (err) {
        log4j.dateFilelogger.error("新建任务异常>" + err);
        res.json({statusCode: 300, message: err});
    });
};

//接受邀请
exports.receiveInvite = function (taskId,isCompleted, openId, res) {
    return taskdao.findById(taskId).then(function (result) {
        if (result.length > 0) {
            var task = result[0];
            if (task.state == 1) {
                //员工A已经有搭档了1
                //res.send('员工A已经有搭档了1');
                res.json({statusCode: 300, message: '你的好友已经有搭档了'});
            } else {
                return taskdao.findOngoingTaskByOpenId(openId).then(function (result1) {
                    if (result1.length > 0) {
                        //员工B有进行中的任务2
                        //res.send('员工B有进行中的任务2');
                        res.json({statusCode: 300, message: '你有正在进行的任务'});
                    } else {
                        return taskdao.findByContent_OpenId(openId, task.content).then(function (result2) {
                            if (result2.length > 0) {
                                //员工B有这个任务（更新+更新）
                                var task1 = result2[0];
                                var taskId_B = task1.id;  //员工B的目标id
                                var isCompleted_B = task1.isCompleted;
                                return invitedao.isFriends(taskId, taskId_B).then(function (result3) {
                                    if (result3.length <= 0) {
                                        return  invitedao.updateTask(taskId_B, taskId, 0,isCompleted).then(function () {
                                            return invitedao.updateTask_002(taskId, task.type, taskId_B, 0,isCompleted_B).then(function () {//还要更改type
                                                //结对子成功3
                                                var msg_A = '你的好友' + task1.userName + '已经接受了你的邀请，你们已经成为好习惯搭档。在接下来7天，你们需要互相鼓励、互相帮助，完成共同的目标。共同坚持7天后，你们都将获得丰厚的积分奖励。';
                                                var msg_B = '恭喜你成为了' + task.userName + '的好习惯搭档，在接下来7天，你们需要互相鼓励、互相帮助，完成共同的目标。共同坚持7天后，你们都将获得丰厚的积分奖励。';
                                                return msgdao.add(task.userId, msg_A, config.msg.befriendsFlag).then(function () {
                                                    return msgdao.add(task1.userId, msg_B, config.msg.befriendsFlag).then(function () {
                                                        //res.send('结对子成功3');
                                                        res.json({statusCode: 200, message: '恭喜你，成功成为你好友的搭档'});
                                                    });
                                                });
                                            });
                                        });
                                    } else {
                                        //继承签到（继承较小的签到天数）
                                        var t_t1 = result3[0];
                                        var sign_in_num = t_t1.sign_in_num;
                                        var sign_in_num2 = t_t1.sign_in_num2;
                                        var my_sign_in_num = Math.min(sign_in_num, sign_in_num2);
                                        return  invitedao.updateTask(taskId_B, taskId, my_sign_in_num,isCompleted).then(function () {
                                            return invitedao.updateTask(taskId, taskId_B, my_sign_in_num,isCompleted_B).then(function () {
                                                //结对子成功3
                                                var msg_A = '你的好友' + task1.userName + '已经接受了你的邀请，你们已经成为好习惯搭档。在接下来7天，你们需要帮助、鼓励对方完成自己的目标。共同坚持7天后，你们都将获得丰厚的积分奖励。';
                                                var msg_B = '恭喜你成为了' + task.userName + '的好习惯搭档，在接下来7天，你们需要帮助、鼓励对方完成自己的目标。共同坚持7天后，你们都将获得丰厚的积分奖励。';
                                                return msgdao.add(task.userId, msg_A, config.msg.befriendsFlag).then(function () {
                                                    return msgdao.add(task1.userId, msg_B, config.msg.befriendsFlag).then(function () {
                                                        //res.send('结对子成功3_');
                                                        res.json({statusCode: 200, message: '恭喜你，成功成为你好友的搭档'});
                                                    });
                                                });
                                                //res.send('结对子成功3_');
                                            });
                                        });
                                    }
                                });
                            } else {
                                //员工B没有这个任务（新建+更新）
                                return userdao.findByOpenId(openId).then(function (result_users) {
                                    var userId = result_users[0].id;
                                    var userName = result_users[0].userName;
                                    var type = task.type;
                                    var content = task.content;
                                    var cardId = task.cardId;
                                    var joinTaskId = taskId;
                                    var random = new Random();
                                    var taskCode = random.getRandom(8);
                                    return invitedao.addTask_B(userId, type, content, cardId, joinTaskId, taskCode).then(function (result3) {
                                        var taskId_B = result3.insertId;
                                        return invitedao.updateTask(taskId_B, taskId, 0,isCompleted).then(function () {
                                            //结对子成功4
                                            var msg_A = '你的好友' + userName + '已经接受了你的邀请，你们已经成为好习惯搭档。在接下来的7天，你们需要互相鼓励、互相帮助，完成共同的目标。共同坚持7天后，你们都将获得丰厚的积分奖励。';
                                            var msg_B = '恭喜你成为了' + task.userName + '的好习惯搭档，在接下来的7天，你们需要互相鼓励、互相帮助，完成共同的目标。共同坚持7天后，你们都将获得丰厚的积分奖励。';
                                            return msgdao.add(task.userId, msg_A, config.msg.befriendsFlag).then(function () {
                                                return msgdao.add(userId, msg_B, config.msg.befriendsFlag).then(function () {
                                                    //res.send('结对子成功4');
                                                    res.json({statusCode: 200, message: '恭喜你，成功成为你好友的搭档'});
                                                });
                                            });
                                        });
                                    });
                                });
                            }
                        });
                    }
                });
            }
        } else {
        }
    }).catch(function (err) {
        log4j.dateFilelogger.error("员工B接受员工A邀请异常>" + err);
        res.json({statusCode: 300, message: err});
    });
};

//每日签到
exports.everyDaySignIn = function (userId, unit, taskId, res) {
    signdao.isTodaySign(taskId).then(function (rst) {
        if (rst.length > 0) {
            res.json({statusCode: 300, message: "你今天已经签到了"});
        } else {
            return signdao.signRecod(userId, taskId).then(function () //添加签到记录
            {
                return signdao.updateTask(taskId).then(function ()  //更新积分
                {
                    return taskdao.findByTaskId(taskId).then(function (result) {
                        var result_task = result[0];

                        if (result_task.sign_in_num >= 7)//签到第7天
                        {
                            if (result_task.sign_in_num2 >= 7) { //搭档也签到了7天
                                //签到完成更新任务状态
                                var taskids = [];
                                taskids.push(taskId);
                                taskids.push(result_task.id2);

                                //计算额外奖励
                                if(result_task.isCompleted == 2)
                                {   //没有奖励
                                    return signdao.updateTask_complete(taskId,0).then(function(){
                                        var type = 1;
                                        return signdao.addOrUpdateIntegral(userId,taskId,config.constant.everydayIntegral,type,unit).then(function()
                                        {
                                            //res.json({statusCode: 200, message: "任务完成"});
                                            exports.op(result_task,res);
                                        });
                                    });
                                }else{
                                    var times = result_task.continueTimes+1;
                                    var tempTimes = times;
                                    if(times > 3){
                                        times = 1;
                                        tempTimes = 1;
                                    }
                                    var extraIntegral = times * 140;
                                    return signdao.updateTask_complete_1(taskId,tempTimes,extraIntegral).then(function(){
                                        ////当日加额外奖励
                                        var everydayIntegral = config.constant.everydayIntegral + extraIntegral;
                                        var type = 1;
                                        return signdao.addOrUpdateIntegral(userId,taskId,everydayIntegral,type,unit).then(function()
                                        {
                                            exports.op(result_task,res);
                                        });
                                    });
                                }
                            } else {
                                //搭档还没完成
                                var type = 0;
                                return signdao.addOrUpdateIntegral(userId,taskId,config.constant.everydayIntegral,type,unit).then(function()
                                {
                                    res.json({statusCode: 200, message: "搭档还没完成!"});
                                    msgdao.sendMsgTo(result_task.userId2,'你的搭档已经完成7天签到了哦，你也不要忘记，继续坚持你们的目标，然后回到游戏中签到吧！',5);
                                });
                            }
                        } else
                        {
                            var type = 0;
                            return signdao.addOrUpdateIntegral(userId,taskId,config.constant.everydayIntegral,type,unit).then(function()
                            {
                                res.json({statusCode: 200, message: "签到成功!"});
                            });
                        }
                    });
                });
            });
        }
    }).catch(function (err) {
        log4j.dateFilelogger.error("员工每日签到异常>" + err);
        res.json({statusCode: 300, message: err});
    });
};

exports.op = function(result_task,res){
    var userId2 = result_task.userId2;
    var taskId2 = result_task.id2;

    if(result_task.isCompleted2 == 2){//没有奖励
        return signdao.updateTask_complete(taskId2,0).then(function(){
            return signdao.addOrUpdateIntegral(userId2,taskId2,0,1,"").then(function()//更新等级
            {
                return msgdao.add(userId2, config.msg.completed_msg_interrupted, config.msg.signSuccessFlag).then(function(){
                    return msgdao.add(result_task.userId, config.msg.completed_msg_interrupted, config.msg.signSuccessFlag).then(function(){
                        res.json({statusCode: 200, message: "任务完成"});
                    });
                });

            });
        });
    }else{
        var times = result_task.continueTimes2 + 1;
        var tempTimes = times;
        if(times > 3){
            times = 1;
            tempTimes = 1;
        }
        var extraIntegral = times * 140;
        //更新积分和次数
        return signdao.updateTask_complete_1(taskId2,tempTimes,extraIntegral).then(function(){
            return signdao.addOrUpdateIntegral(userId2,taskId2,extraIntegral,1,"").then(function()
            {
                if(result_task.isCompleted == 2){
                    return msgdao.add(userId2, config.msg.completed_msg_interrupted, config.msg.signSuccessFlag).then(function(){
                        return msgdao.add(result_task.userId, config.msg.completed_msg_interrupted, config.msg.signSuccessFlag).then(function(){
                            res.json({statusCode: 200, message: "任务完成"});
                        });
                    });
                }else{
                    return msgdao.add(userId2, config.msg.completed_msg_nointerrupt, config.msg.signSuccessFlag).then(function(){
                        return msgdao.add(result_task.userId, config.msg.completed_msg_nointerrupt, config.msg.signSuccessFlag).then(function(){
                            res.json({statusCode: 200, message: "任务完成"});
                        });
                    });
                }
            });
        });
    }
}

//历史目标
exports.historyTasks = function (userId, res) {
    userdao.findByUserId(userId).then(function (users) {
        var userName = users[0].userName;
        var signNum = 0;
        return userdao.findUserSignInCount(userId).then(function(signInNums){
            if(signInNums.length > 0){
                signNum = signInNums[0].cnt;
            }
            return taskdao.findUserTasks_002(userId).then(function (result) {
                var total_integral = 0;
                if (result.length > 0) {
                    for (var i = 0; i < result.length; i++) {
                        total_integral = total_integral + result[i].integral;
                        var time = result[i].createTime;
                        if (time != null) {
                            result[i].year = time.getFullYear();
                            result[i].month = time.getMonth() + 1;
                            result[i].date = time.getDate();
                        }
                        if (result[i].integral == null) {
                            result[i].integral = 0;
                        }
                    }
                    res.json({statusCode: 200, userName: userName, total_integral: total_integral,signNum : signNum, tasks: result});
                } else {
                    res.json({statusCode: 200, userName: userName, total_integral: total_integral,signNum : signNum, tasks: result});
                }
            })
        });
    }).catch(function (err) {
        log4j.dateFilelogger.error("查询员工历史目标异常>"+err+",users/api/historyTasks?userId="+userId);
        res.json({statusCode: 300, message: err});
    });
};

//用户重启任务
exports.taskRestart = function (userId, taskId, res) {

    taskdao.findOngoingTaskByUserId(userId).then(function (result) {
        if (result.length > 0) {
            res.json({statusCode: 300, message: '你有任务或正在进行，重启失败'});
        } else {
            return taskdao.restart(taskId).then(function ()
            {
                return taskdao.findTaskAndUser(taskId).then(function(selfTaskList)
                {

                    var selfMsg = config.msg.restart_msg_self;
                    var friendMsg = config.msg.restart_msg_friend;

                    if(selfTaskList.length <= 0){
                        res.json({statusCode: 200, message: '重启成功'});
                        return;
                    }
                    var selfTask = selfTaskList[0];
                    var userName = selfTask.userName;
                    var content = selfTask.content;
                    return taskdao.findTaskAndUser(selfTask.joinTaskId).then(function(friendTaskList){
                        if(friendTaskList.length <= 0){
                            res.json({statusCode: 200, message: '重启成功'});
                            return;
                        }
                        var friendTask = friendTaskList[0];
                        var friendId = friendTask.userId;
                        var friendName = friendTask.userName;
                        selfMsg = selfMsg.replace('xxx',content);
                        selfMsg = selfMsg.replace('yyy',friendName);

                        return msgdao.add(userId,selfMsg,config.msg.restartSuccessFlag).then(function(){
                            taskdao.findOngoingTaskByUserId(friendId).then(function(friendOnGoingTaskList){
                                if(friendOnGoingTaskList.length > 0){
                                    res.json({statusCode: 200, message: '重启成功'});
                                }else{
                                    friendMsg = friendMsg.replace('xxx',content);
                                    friendMsg = friendMsg.replace('yyy',userName);
                                    return msgdao.add(friendId,friendMsg,config.msg.restartSuccessFlag).then(function(){
                                        res.json({statusCode: 200, message: '重启成功'});
                                    });
                                }
                            });
                        });
                    });
                });

            });
        }
    }).catch(function (err) {
        log4j.dateFilelogger.error("用户重启任务>" + err);
        res.json({statusCode: 300, message: err});
    });
};

/**
 * 再来一次
 */
exports.playAgain = function(userId,taskId,res){
    var sql = 'SELECT t1.state,t1.content,t1.sign_in_num,t2.id taskId2,t2.userId userId2,t2.sign_in_num sign_in_num2,t2.state state2 FROM t_tasks t1  INNER JOIN t_tasks t2 ON t1.joinTaskId = t2.id WHERE t1.id = ?';
    var args = [taskId];
    dao.exeSql(sql,args).then(function(result){
        if(result.length < 0){
            res.json({statusCode: 300, message: '任务重启失败，请稍后重试'});
            return;
        }
        var task = result[0];
        var content = task.content;
        var state = task.state;
        var sign_in_num = task.sign_in_num;
        var userId2 = task.userId2;
        var taskId2 = task.taskId2;
        var sign_in_num2 = task.sign_in_num2;

        return taskdao.findOngoingTaskByUserId(userId2).then(function(friendTasks){
            if(friendTasks.length > 0){
                res.json({statusCode: 300, message: '你的好友有任务正在进行，重启失败' });
            }else{
                var d = new Date();
                if(state == 2){
                    d.setDate(d.getDate() - 1);
                }
                var signNum = Math.min(sign_in_num, sign_in_num2);
                var sql2 = 'UPDATE t_tasks SET state = 1 ,lastSigninTime = ?,sign_in_num = ? WHERE id IN(?,?)';
                var args2 = [d,signNum,taskId,taskId2];
                return dao.exeSql(sql2,args2).then(function(){
                    res.json({statusCode: 200, message: '重启成功'});
                    if(state == 2){
                        msgdao.add(userId2,config.msg.playAgainMsg,config.msg.restartSuccessFlag).catch(function(err){
                        });
                    }else{
                        userdao.findByUserId(userId).then(function(users){
                            var userName = users[0].userName;
                            var msg = config.msg.playAgainMsg_for;
                            msg = msg.replace('xxx',content);
                            msg = msg.replace('yyy',userName);
                            msgdao.add(userId2,msg,config.msg.restartSuccessFlag)
                        }).catch(function(err){
                        });
                    }
                });
            }
        });
    }).catch(function(err){
        log4j.dateFilelogger.error("任务重启失败（再来一次）" + err+",users/api/playAgain?userId="+userId+"&taskId="+taskId);
        res.json({statusCode: 300, message: '任务重启失败，请稍后重试'});
    });

}

exports.myTask = function (userId, taskId, res) {
    taskdao.findUserSignInCase(taskId).then(function (result) {
        if (result.length > 0) {
            return taskdao.findIntegralByTaskId(taskId).then(function (rst) {
                var total_integral = 0;
                var integral = 0;
                var level = 0;
                if (rst.length > 0) {
                    integral = rst[0].integral;
                    level = rst[0].level;
                }
                return taskdao.findTotalIntegralByUserId(userId).then(function (rst) {
                    if(rst.length > 0){
                        total_integral = rst[0].integral;
                    }
                    result[0].total_integral = total_integral;
                    result[0].integral = integral;
                    result[0].level = level;
                    var signDetail = result[0];
                    res.json({statusCode: 200, tasks: signDetail});
                });
            });
        } else {
            log4j.dateFilelogger.error("查询员工签到详情异常"+",users/api/mytask?userId="+userId+"&taskId="+taskId);
            res.json({statusCode: 300, message: '数据库异常'});
        }
    }).catch(function (err) {
        log4j.dateFilelogger.error("查询员工签到详情异常>" + err+",users/api/mytask?userId="+userId+"&taskId="+taskId);
        res.json({statusCode: 300, message: '数据库异常'});
    });
}

//任务分享(员工签到详情)
exports.userSignInCase = function (userId, taskId,accountId, res) {

    taskdao.findUserSignInCase(taskId).then(function (result) {
        if (result.length > 0) {
            return taskdao.findIntegralByTaskId(taskId).then(function (rst) {
                var total_integral = 0;
                var integral = 0;
                if (rst.length > 0) {
                    integral = rst[0].integral;
                }
                return taskdao.findTotalIntegralByUserId(userId).then(function (rst) {
                    if (rst.length > 0) {
                        total_integral = rst[0].integral;
                    }
                    result[0].total_integral = total_integral;
                    result[0].integral = integral;
                    var signDetail = result[0];
                    var id, userId, content, sign_in_num, cardId, sign_in_num2, total_integral1, integral1,totalSignNum;
                    id = signDetail.id;
                    userId = signDetail.userId;
                    content = signDetail.content;
                    sign_in_num = signDetail.sign_in_num;
                    cardId = signDetail.cardId;
                    sign_in_num2 = signDetail.sign_in_num2;
                    total_integral1 = signDetail.total_integral;
                    integral1 = signDetail.integral;
                    totalSignNum = signDetail.totalSignNum;
                    //var baseContent=base64.encode(content);
                    var baseContent=mixCrypto.encrypt(content);
                    var params=baseContent+'|'+sign_in_num+'|'+cardId+'|'+sign_in_num2+'|'+total_integral1+'|'+integral1+'|'+accountId+'|'+totalSignNum;
                    res.redirect(config.auth.registeredUriForJssdk + '/game/shareSignIncase?params=' + params);
                });
            });
        } else {
            log4j.dateFilelogger.error("(分享任务)查询员工签到详情异常taskId="+taskId+",userId="+userId);
            res.render('exception',{msg: "failed to obtain task detail.."});
        }
    }).catch(function (err) {
        log4j.dateFilelogger.error("查询员工签到详情异常>" + err);
        res.render('exception',{msg: "failed to obtain task detail.."});
    });
};


