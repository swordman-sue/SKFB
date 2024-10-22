var when=require('when');
var dao = require('../mysql/dao');

/**
 * 查询已同步的单位
 * @returns {*}
 */
exports.findAllDept = function(){
    var sql='SELECT id FROM dept';
    var args=[];
    return dao.exeSql(sql,args);
}

/**
 * 新增一个单位
 * @param id
 * @param title
 * @param shortName
 * @param pid
 * @param path
 * @param remark
 * @param isenterprise
 * @param pathById
 * @returns {*}
 */
exports.addDept = function(id,title,shortName,pid,path,remark,isenterprise,pathById){
    var sql='INSERT INTO dept(id,title,shortName,pid,path,remark,isenterprise,pathById) VALUES (?,?,?,?,?,?,?,?)';
    var args=[id,title,shortName,pid,path,remark,isenterprise,pathById];
    return dao.exeSql(sql,args);
}

/**
 * 查询单位结构
 * @param enterpriseid
 * @returns {*}
 */
exports.findPathId = function(enterpriseid){
    var sql='SELECT pathById FROM dept WHERE id = ?';
    var args=[enterpriseid];
    return dao.exeSql(sql,args);
}

/**
 * 查询单位信息
 * @param enterpriseid
 * @returns {*}
 */
exports.findBy = function(enterpriseid){
    var sql='SELECT * FROM dept WHERE id = ?';
    var args=[enterpriseid];
    return dao.exeSql(sql,args);
}






