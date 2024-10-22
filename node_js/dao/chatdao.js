var when=require('when');
var dao = require('../mysql/dao');

//查询聊天记录(今天和前三天的记录)
exports.findByTaskId = function(taskId){
    var sql='SELECT temp.*,u.userName name FROM ' +
        '(SELECT tc.* FROM t_chat tc INNER JOIN t_tasks tt ON tc.type=tt.type WHERE tt.id=? AND DATEDIFF(CURRENT_DATE(),tc.createTime)<=3 ) temp ' +
        'INNER JOIN t_users u ON temp.userId=u.id ORDER BY temp.id ASC';
    var args=[taskId];
    return dao.exeSql(sql,args);
};

/**
 * 最新五十条记录
 * @param taskId
 * @returns {*}
 * @private
 */
exports.findByTaskId_ = function(taskId){
    var sql='SELECT temp.*,u.userName name FROM ' +
        '(SELECT tc.* FROM t_chat tc INNER JOIN t_tasks tt ON tc.type=tt.type WHERE tt.id=? LIMIT 0,100 ) temp ' +
        'INNER JOIN t_users u ON temp.userId=u.id ORDER BY temp.id ASC';
    var args=[taskId];
    return dao.exeSql(sql,args);
};

//员工聊天
exports.add = function(userId,taskId,type,content){
    var sql='INSERT INTO t_chat(type,userId,taskId,content,createTime) VALUES(?,?,?,?,?)';
    var args=[type,userId,taskId,content,new Date()];
    return dao.exeSql(sql,args);
}

