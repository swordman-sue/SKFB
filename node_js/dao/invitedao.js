var when=require('when');
var dao = require('../mysql/dao');

//员工接收邀请创建新的任务--》返回新建任务id
exports.addTask_B = function (userId, type, content, cardId, joinTaskId,taskCode) {
    var sql = 'INSERT INTO t_tasks(userId,type,content,createTime,startTime,lastSigninTime,cardId,state,joinTaskId,taskCode) VALUES(?,?,?,?,?,?,?,?,?,?)';
    var d = new Date();
    d.setDate(d.getDate() - 1);
    var args = [userId, type, content, new Date(),new Date(),d, cardId, 1, joinTaskId,taskCode];
    return dao.exeSql(sql, args);
};

//员工接收邀请后更新好友的任务状态以及关联的信息
exports.updateTask = function (joinTaskId,taskId,sign_in_num,isCompleted) {

    if(isCompleted == 2){   //未完成不更新startTime
        var sql = 'UPDATE t_tasks SET state=?,lastSigninTime=?,sign_in_num=?,joinTaskId=? WHERE id=?';
        var d = new Date();
        d.setDate(d.getDate() - 1);
        var args = [1, d,sign_in_num, joinTaskId,taskId];
        return dao.exeSql(sql, args);
    }else{
        var sql = 'UPDATE t_tasks SET state=?,startTime=?,lastSigninTime=?,sign_in_num=?,joinTaskId=? WHERE id=?';
        var d = new Date();
        d.setDate(d.getDate() - 1);
        var args = [1, new Date(),d,sign_in_num, joinTaskId,taskId];
        return dao.exeSql(sql, args);
    }

};

exports.updateTask_002 = function (joinTaskId,type,taskId,sign_in_num,isCompleted) {
    if(isCompleted == 2){
        var sql = 'UPDATE t_tasks SET state=?,lastSigninTime=?,sign_in_num=?,joinTaskId=?,type=? WHERE id=?';
        var d = new Date();
        d.setDate(d.getDate() - 1);
        var args = [1,d,sign_in_num, joinTaskId,type,taskId];
        return dao.exeSql(sql, args);
    }else{
        var sql = 'UPDATE t_tasks SET state=?,startTime=?,lastSigninTime=?,sign_in_num=?,joinTaskId=?,type=? WHERE id=?';
        var d = new Date();
        d.setDate(d.getDate() - 1);
        var args = [1, new Date(),d,sign_in_num, joinTaskId,type,taskId];
        return dao.exeSql(sql, args);
    }

};


//邀请的搭档是否和上一次是同一个人
exports.isFriends = function (taskId_A,taskId_B) {
    var sql = 'SELECT t.id,t.joinTaskId,t.sign_in_num,t1.id id2,t1.joinTaskId joinTaskId2,t1.sign_in_num sign_in_num2  FROM t_tasks t ' +
        'INNER JOIN  t_tasks t1 ON t.joinTaskId=t1.id AND t1.joinTaskId=t.id WHERE t.id=? AND t1.id=?;';
    var args = [taskId_A,taskId_B];
    return dao.exeSql(sql, args);
};