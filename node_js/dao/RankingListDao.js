//排行榜
var when=require('when');
var dao = require('../mysql/dao');


/**
 * 全网排名
 * @returns {*}
 */
exports.findRankList = function(pageNum,pageSize){
    var sql='SELECT temp.userId,temp.userName,temp.unit,t.integral FROM ' +
        '( SELECT u.userName,u.id userId,d.title unit FROM t_users u INNER JOIN dept d ON u.enterpriseid = d.id ) temp ' +
        'LEFT JOIN t_total_integral t ON t.userId = temp.userId ORDER BY t.integral DESC ,temp.userId ASC LIMIT ?,?';
    var args=[pageNum,pageSize];
    return dao.exeSql(sql,args);
};

/**
 * 一级排名（全网排名）
 * @param enterpriseid
 * @returns {*}
 */
exports.findFirstRankList = function(enterpriseid){
    var sql='SELECT temp.userId,temp.userName,temp.unit,t.integral FROM ( SELECT u.userName,u.id userId,d.title unit FROM t_users u INNER JOIN dept d ON u.enterpriseid = d.id WHERE u.firstId=? ) temp LEFT JOIN t_total_integral t ON t.userId = temp.userId ORDER BY t.integral DESC ,temp.userId ASC LIMIT 0,50';
    var args=[enterpriseid];
    return dao.exeSql(sql,args);
};

/**
 * 二级排名
 * @param enterpriseid
 * @returns {*}
 */
exports.findSecondRankList = function(enterpriseid,pageNum,pageSize){
    var sql='SELECT temp.userId,temp.userName,temp.unit,t.integral FROM ' +
        '( SELECT u.userName,u.id userId,d.title unit FROM t_users u INNER JOIN dept d ON u.enterpriseid = d.id WHERE u.secondId=? ) temp ' +
        'LEFT JOIN t_total_integral t ON t.userId = temp.userId ORDER BY t.integral DESC ,temp.userId ASC LIMIT ?,?';
    var args=[enterpriseid,pageNum,pageSize];
    return dao.exeSql(sql,args);
};

/**
 * 三级排名
 * @param enterpriseid
 * @returns {*}
 */
exports.findThirdRankList = function(enterpriseid,pageNum,pageSize){
    var sql='SELECT temp.userId,temp.userName,temp.unit,t.integral FROM ' +
        '( SELECT u.userName,u.id userId,d.title unit FROM t_users u INNER JOIN dept d ON u.enterpriseid = d.id WHERE u.thirdId=? ) temp ' +
        'LEFT JOIN t_total_integral t ON t.userId = temp.userId ORDER BY t.integral DESC,temp.userId ASC LIMIT ?,?';
    var args=[enterpriseid,pageNum,pageSize];
    return dao.exeSql(sql,args);
};