//排行榜
var when=require('when');
var dao = require('../mysql/dao');

//单位参与游戏所有员工的总积分
exports.findAllUnitInteral = function(){
    var sql='SELECT unit,SUM(integral) unitIntegral  FROM t_integral GROUP BY unit';
    var args=[];
    return dao.exeSql(sql,args);
};

//某单位参与人数
exports.findUnitUsersCount = function(unit){
    var sql='SELECT COUNT(id) user_count FROM t_users WHERE unit=?';
    var args=[unit];
    return dao.exeSql(sql,args);
};

//某单位完成游戏的人数
exports.findUnitUsersNum = function(){
    var sql='SELECT unit,COUNT(DISTINCT userId) unit_users_count FROM t_integral GROUP BY unit';
    var args=[];
    return dao.exeSql(sql,args);
};

//单位总人数
exports.findUnitTotalUserCount = function(unit){
    var sql='SELECT * FROM t_unit WHERE unit=?';
    var args=[unit];
    return dao.exeSql(sql,args);
};



//励志达人榜(任务积分表)
exports.taskintegral_ranklist = function(){
    var sql='SELECT i.*,u.userName,u.unit,t.type,t.content FROM t_integral i ' +
        'INNER JOIN t_tasks t INNER JOIN t_users u ' +
        'ON i.taskId=t.id AND i.userId=u.id ORDER BY i.integral DESC LIMIT 0,10';
    var args=[];
    return dao.exeSql(sql,args);
};

//宠物达人榜
exports.persontasks_ranklist = function(){
    var sql='SELECT u.userName,u.unit,temp.* FROM t_users u INNER JOIN ' +
        '(SELECT userid,COUNT(userId) card_count FROM t_integral GROUP BY userId ORDER BY card_count DESC LIMIT 0,10) temp ' +
        'ON u.id=temp.userId';
    var args=[];
    return dao.exeSql(sql,args);
};


