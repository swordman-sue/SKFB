var when=require('when');
var dao = require('../mysql/dao');
var config = require('../utils/config');

//添加签到记录
exports.signRecod = function (userId,taskId) {
    var sql = 'INSERT INTO u_sign_in(userId,taskId,createTime) VALUES(?,?,?)';
    var args = [userId,taskId,new Date()];
    return dao.exeSql(sql, args);
};

//一次签到更改目标信息
//exports.updateTask = function (taskId) {
//    var sql = 'UPDATE t_tasks SET sign_in_num=sign_in_num+1,lastSigninTime=?,totalSignNum=totalSignNum+1 WHERE id=?';
//    var args = [new Date(),taskId];
//    return dao.exeSql(sql, args);
//};

/**
 * 更新任务签到详细
 * @param taskId
 * @returns {*}
 */
exports.updateTask = function (taskId) {
    var everydayIntegral = config.constant.everydayIntegral;
    var sql = 'UPDATE t_tasks SET sign_in_num=sign_in_num+1,totalSignNum=totalSignNum+1,integral= integral+?,lastSigninTime=? WHERE id=?';
    var args = [everydayIntegral,new Date(),taskId];
    return dao.exeSql(sql, args);
};

//签到第七天
exports.updateTask_complete = function (taskId,continueTimes) {  //任务完成
    var sql = 'UPDATE t_tasks SET state=3,sign_in_num=0,isCompleted=3,continueTimes=?,level=level+1 WHERE id = ?';
    var args = [continueTimes,taskId];
    return dao.exeSql(sql, args);
};

exports.updateTask_complete_1 = function (taskId,continueTimes,extraIntegral) {  //任务完成
    var sql = 'UPDATE t_tasks SET state=3,sign_in_num=0,isCompleted=3,continueTimes=?,integral=integral+?,level=level+1 WHERE id = ?';
    var args = [continueTimes,extraIntegral,taskId];
    return dao.exeSql(sql, args);
};

//今日是否签到
exports.isTodaySign = function (taskId) {
    var sql = 'SELECT * FROM t_tasks t WHERE DATE_FORMAT(t.lastSigninTime,\'%Y-%m-%d\') = CURRENT_DATE() AND t.id=?';
    var args = [taskId];
    return dao.exeSql(sql, args);
};

//完成一个七天花费的天数-1
//exports.completeDays = function (taskId) {
//    var sql = 'SELECT DATEDIFF(CURRENT_DATE(),t.startTime) num FROM t_tasks t WHERE t.id=?';
//    var args = [taskId];
//    return dao.exeSql(sql, args);
//};


////添加积分
//exports.addIntegral = function (userId,unit,taskId,recent_integral,integral) {
//    var sql = 'INSERT INTO t_integral(userId,unit,taskId,recent_integral,integral,level) VALUES(?,?,?,?,?,?)';
//    var args = [userId,unit,taskId,recent_integral,integral,1];
//    return dao.exeSql(sql, args).then(function(){
//        return exports.findTotalIntegralByUserId(userId).then(function(rst){//查询总积分表
//            if(rst.length > 0){
//                return exports.updateTotalIntegralByUserId(userId,integral);//更新总积分表
//            }else{
//                return exports.addTotalIntegralByUserId(userId,integral);   //插入到总积分表
//            }
//        });
//    });
//};
//
////更新积分
//exports.updateIntegral = function (integral,taskId,userId) {
//    var sql = 'UPDATE t_integral SET last_integral=integral,recent_integral=?,integral=integral+?,level=level+1 WHERE taskId=?';
//    var args = [integral,integral,taskId];
//    return dao.exeSql(sql, args).then(function(){
//        return exports.findTotalIntegralByUserId(userId).then(function(rst){//查询总积分表
//            if(rst.length > 0){
//                return exports.updateTotalIntegralByUserId(userId,integral);
//            }else{
//                return exports.addTotalIntegralByUserId(userId,integral);
//            }
//        });
//    });
//};

/**
 *
 * @param userId
 * @param taskId
 * @param integral
 * @param type      1表示需要更新积分表的等级
 * @param unit
 */
exports.addOrUpdateIntegral = function(userId,taskId,integral,type,unit){
    return exports.findIntegralByTaskId(taskId).then(function(taskIntegralList)
    {
        if(taskIntegralList.length <= 0)
        {
            //添加任务积分start
            return exports.addTaskIntegral(userId,taskId,integral,unit).then(function()
            {
                //更新总积分start
                return exports.findTotalIntegralByUserId(userId).then(function(userTotalIntegralList)
                {
                    if(userTotalIntegralList.length <= 0 ){
                        return exports.addTotalIntegralByUserId(userId,integral);
                    }else{
                        return exports.updateTotalIntegralByUserId(userId,integral);
                    }
                });
                //更新总积分end
            });
            //添加任务积分end
        }else
        {
            //更新任务积分start
            return exports.updateTaskIntegral(integral,taskId,userId,type).then(function()
            {
                //更新总积分start
                return exports.findTotalIntegralByUserId(userId).then(function(userTotalIntegralList)
                {
                    if(userTotalIntegralList.length <= 0 ){
                        return exports.addTotalIntegralByUserId(userId,integral);
                    }else{
                        return exports.updateTotalIntegralByUserId(userId,integral);
                    }
                });
                //更新总积分end
            });
            //更新任务积分end
        }
    });
}

//查询积分
exports.findIntegralByTaskId = function (taskId) {
    var sql = 'SELECT * FROM t_integral WHERE taskId=?';
    var args = [taskId];
    return dao.exeSql(sql, args);
};

/**
 *
 * @param userId
 * @param taskId
 * @param integral 获取到的新积分
 * @param unit
 * @returns {*}
 */
exports.addTaskIntegral = function (userId,taskId,integral,unit) {
    var sql = 'INSERT INTO t_integral(userId,unit,taskId,integral) VALUES(?,?,?,?)';
    var args = [userId,unit,taskId,integral];
    return dao.exeSql(sql, args);
};

/**
 *
 * @param integral  获取到的新积分
 * @param taskId
 * @param userId
 * @param type  //是否需要更新等级
 * @returns {*}
 */
exports.updateTaskIntegral = function (integral,taskId,userId,type) {
    var sql;
    if(type == 1){  //
       sql = 'UPDATE t_integral SET last_integral=integral,level=level+1,integral=integral+? WHERE taskId=?';
    }else{
       sql = 'UPDATE t_integral SET last_integral=integral,integral=integral+? WHERE taskId=?';
    }
    var args = [integral,taskId];
    return dao.exeSql(sql, args);
};

exports.findTotalIntegralByUserId = function (userId) {
    var sql = 'SELECT * FROM t_total_integral WHERE userId=?';
    var args = [userId];
    return dao.exeSql(sql, args);
};

exports.addTotalIntegralByUserId = function (userId,integral) {
    var sql = 'INSERT INTO t_total_integral(userId,integral) VALUES(?,?)';
    var args = [userId,integral];
    return dao.exeSql(sql, args);
};

exports.updateTotalIntegralByUserId = function (userId,integral) {
    var sql = 'UPDATE t_total_integral SET integral=integral+? WHERE userId=?';
    var args = [integral,userId];
    return dao.exeSql(sql, args);
};













