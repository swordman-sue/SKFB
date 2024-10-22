var when=require('when');
var dao = require('../mysql/dao');


/**
 * 添加日志
 * @param desc
 * @param params
 * @returns {*}
 */
exports.add = function(desc,params){
    var sql='INSERT INTO auth_log(des,params,times) VALUES (?,?,?)';
    var args=[desc,params,new Date()];
    return dao.exeSql(sql,args);
}