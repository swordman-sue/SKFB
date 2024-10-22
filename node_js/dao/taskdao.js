var when=require('when');
var dao = require('../mysql/dao');

/**
 * 1、以下为员工新建目标
 */

//查看员工有无该目标
exports.findByContent_UserId = function (userId,content) {
    var sql = 'SELECT * FROM t_tasks WHERE userId=? AND content=?';
    var args = [userId,content];
    return dao.exeSql(sql, args);
};
//查看员工有无该目标
exports.findByContent_OpenId = function (openId,content) {
    var sql = 'SELECT t.*,u.userName FROM t_tasks t INNER JOIN t_users u ON t.userId=u.id WHERE u.openId=? AND t.content=?';
    var args = [openId,content];
    return dao.exeSql(sql, args);
};

//查看有无进行中的目标
exports.findOngoingTaskByUserId = function (userId) {
    var sql = 'SELECT * FROM t_tasks WHERE userId=? AND (state=? or state=?)';
    var args = [userId,1,0];
    return dao.exeSql(sql, args);
};
//查看有无进行中的目标
exports.findOngoingTaskByOpenId = function (openId) {
    var sql = 'SELECT * FROM t_users u INNER JOIN t_tasks t ON u.id=t.userId WHERE u.openId=? AND (t.state=? or state=?)';
    var args = [openId,1,0];
    return dao.exeSql(sql, args);
};

//根据状态查询用户的任务
exports.findByUserIdAndState = function (userId,state) {
    var sql = 'SELECT u.userName,u.unit,u.enterpriseid,u.accountId,t.* FROM t_tasks t INNER JOIN t_users u ON t.userId=u.id WHERE t.userId=? AND t.state=? ';
    if(state == 2){
        sql=sql+'AND DATEDIFF(CURRENT_DATE(),t.lastSigninTime) <= 2';
    }
    var args = [userId,state];
    return dao.exeSql(sql, args);
};



//新建目标
exports.addTask = function (userId, type, content, cardId,taskCode) {
    var sql = 'INSERT INTO t_tasks(userId,type,content,createTime,cardId,taskCode) VALUES(?,?,?,?,?,?)';
    var args = [userId, type, content, new Date(), cardId,taskCode];
    return dao.exeSql(sql, args);
};

/**
 * 2、以下员工接收邀请
 */
exports.findSimpleById = function (taskId) {
    var sql = 'SELECT * FROM t_tasks WHERE id = ?';
    var args = [taskId];
    return dao.exeSql(sql, args);
};

exports.findById = function (taskId) {
    var sql = 'SELECT t.*,DATEDIFF(CURRENT_DATE(),t.lastSigninTime) days,u.userName FROM t_tasks t INNER JOIN t_users u ON t.userId=u.id WHERE t.id = ?';
    var args = [taskId];
    return dao.exeSql(sql, args);
};

exports.findById2 = function (taskId) {
    var sql = 'SELECT t.id taskIdB,t.content,t.joinTaskId taskIdA,t.userId userIdB,t2.userId userIdA,DATEDIFF(CURRENT_DATE(),t.lastSigninTime) days FROM t_tasks t INNER JOIN t_tasks t2 ON  t2.id=t.joinTaskId WHERE t.id = ?';
    var args = [taskId];
    return dao.exeSql(sql, args);
};

exports.findByTaskId = function (taskId) {
    var sql = 'SELECT t.id,t.userId,t.sign_in_num,t.isCompleted,t.continueTimes,t1.id id2,t1.userId userId2,t1.sign_in_num sign_in_num2,t1.isCompleted isCompleted2,t.continueTimes continueTimes2 FROM t_tasks t INNER JOIN t_tasks t1 ON t.joinTaskId=t1.id WHERE t.id=?';
    var args = [taskId];
    return dao.exeSql(sql, args);
};


exports.findUserByTaskId = function (taskId) {
    var sql = 'SELECT t.*,u.openId FROM t_tasks t INNER JOIN t_users u ON t.userId=u.id WHERE t.id=?';
    var args = [taskId];
    return dao.exeSql(sql, args);
};



//查看用户的历史目标
exports.findUserTasks = function(userId){
  var sql='SELECT u.userName,temp.* FROM t_users u  ' +
      'INNER JOIN ' +
      '(SELECT t_t.userId,t_t.createTime,t_t.type,t_t.content,t_t.state,t_i.integral ' +
      'FROM t_tasks t_t INNER JOIN t_integral t_i ' +
      'ON t_t.id=t_i.taskId WHERE t_t.userId=?) temp ' +
      'ON temp.userId=u.id';
    var args=[userId];
    return dao.exeSql(sql,args);
};

exports.findUserTasks_002 = function(userId){
    var sql='SELECT t_t.id ,t_t.userId,t_t.createTime,t_t.type,t_t.content,t_t.state,t_t.totalSignNum,t_i.integral FROM t_tasks t_t LEFT JOIN t_integral t_i ON t_t.id=t_i.taskId WHERE t_t.userId=?';
    var args=[userId];
    return dao.exeSql(sql,args);
};

//任务签到分享
exports.findUserSignInCase = function(taskId){
    var sql='SELECT t1.id,t1.userId,t1.type,t1.content,t1.sign_in_num,t1.cardId,t1.totalSignNum,t2.sign_in_num sign_in_num2 FROM t_tasks t1 INNER JOIN t_tasks t2 ON t1.joinTaskId=t2.id WHERE t1.id=?';
    var args=[taskId];
    return dao.exeSql(sql,args);
};

//获取本次任务的积分
exports.findIntegralByTaskId = function(taskId){
    var sql='SELECT ti.userId,ti.integral,ti.level FROM t_integral ti WHERE taskId=?';
    var args=[taskId];
    return dao.exeSql(sql,args);
};

//获取所有任务的积分
exports.findAllIntegralByUserId = function(userId){
    var sql='SELECT ti.taskId,ti.integral,ti.level FROM t_integral ti WHERE userId=?';
    var args=[userId];
    return dao.exeSql(sql,args);
};

exports.findTotalIntegralByUserId = function(userId){
    var sql='SELECT * FROM t_total_integral WHERE userId = ?';
    var args=[userId];
    return dao.exeSql(sql,args);
};


exports.updateState = function(state,taskId){
    var sql='UPDATE t_tasks SET state = ? WHERE id=?';
    var args=[state,taskId];
    return dao.exeSql(sql,args);
};

exports.restart = function(taskId){
    var sql='UPDATE t_tasks SET state = ?,restartTime = ? WHERE id=?';
    var args=[0,new Date(),taskId];
    return dao.exeSql(sql,args);
};

//exports.updateStateForTwo = function(state,taskId,joinTaskId){
//    var sql='UPDATE t_tasks SET state = ? WHERE id in(?,?)';
//    var args=[state,taskId,joinTaskId];
//    return dao.exeSql(sql,args);
//};

exports.updateStateAndCompleted = function(state,taskId){
    var sql='UPDATE t_tasks SET state = ?,isCompleted = 2,continueTimes = 0 WHERE id=?';
    var args=[state,taskId];
    return dao.exeSql(sql,args);
};

exports.updateStateAndCompletedForTwo = function(state,taskId,joinTaskId){
    var sql='UPDATE t_tasks SET state = ?,isCompleted = 2,continueTimes = 0 WHERE id in(?,?)';
    var args=[state,taskId,joinTaskId];
    return dao.exeSql(sql,args);
};


exports.findTaskAndUser = function(taskId){
    var sql='SELECT t.userId userId,u.userName,t.id taskId,t.content,t.joinTaskId FROM t_tasks t INNER JOIN t_users u ON t.userId = u.id WHERE t.id = ? ';
    var args=[taskId];
    return dao.exeSql(sql,args);
};

exports.deleteByTaskId = function(taskId){
    var sql='DELETE FROM t_tasks WHERE id = ?';
    var args=[taskId];
    return dao.exeSql(sql,args);
};

exports.findSignNum = function(taskId){
    var sql = 'SELECT t1.state,t1.content,t1.sign_in_num,t2.id taskId2,t2.userId userId2,t2.sign_in_num sign_in_num2,t2.state state2 FROM t_tasks t1  INNER JOIN t_tasks t2 ON t1.joinTaskId = t2.id WHERE t1.id = ?';
    var args = [taskId];
    return dao.exeSql(sql,args);
}
























