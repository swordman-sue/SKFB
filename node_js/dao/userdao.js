var when=require('when');
var dao = require('../mysql/dao');
var tokendao = require('../dao/tokendao');

exports.test = function (role_id,name,age) {
    var sql = 'INSERT INTO student(role_id,name,age) VALUES(?,?,?)';
    var args = [role_id,name,age];
	return dao.exeSql(sql, args);
};

exports.test_get_user_data = function (role_id) {
    var sql = 'SELECT * FROM student WHERE role_id=?';
    var args = [role_id];
    return dao.exeSql(sql, args);
};

exports.add = function (openId,userName,tel,unit,dept,firstId,secondId,thirdId,enterpriseid,departmentid,accountId,access_token,expires,refresh_token) {
    var sql = 'INSERT INTO t_users(openId,userName,tel,unit,dept,firstId,secondId,thirdId,enterpriseid,departmentid,accountId,access_token) VALUES(?,?,?,?,?,?,?,?,?,?,?,?)';
    var args = [openId,userName,tel,unit,dept,firstId,secondId,thirdId,enterpriseid,departmentid,accountId,access_token];
    return tokendao.update(access_token,expires,refresh_token).then(function(){
        return dao.exeSql(sql, args);
    });

};

exports.update = function (tel,unit,dept,firstId,secondId,thirdId,enterpriseid,departmentid,accountId,access_token,userId,expires,refresh_token) {
    var sql = 'UPDATE t_users SET tel=?,unit=?,dept=?,firstId=?,secondId=?,thirdId=?,enterpriseid=?,departmentid=?,accountId=?,access_token=? WHERE id=?';
    var args = [tel,unit,dept,firstId,secondId,thirdId,enterpriseid,departmentid,accountId,access_token,userId];
    return tokendao.update(access_token,expires,refresh_token).then(function(){
        return dao.exeSql(sql, args);
    });
};

exports.findByOpenId = function (openId) {
    var sql = 'SELECT * FROM t_users WHERE openId=?';
    var args = [openId];
    return dao.exeSql(sql, args);
};

exports.findByUserId = function (userId) {
    var sql = 'SELECT * FROM t_users WHERE id=?';
    var args = [userId];
    return dao.exeSql(sql, args);
};

exports.findOpenIdByUserId = function (userId) {
    var sql = 'SELECT openId FROM t_users WHERE id=?';
    var args = [userId];
    return dao.exeSql(sql, args);
};

exports.finduserNameAndSignNumer = function (userId) {
    var sql = 'SELECT userName,temp.number signNum FROM t_users u LEFT JOIN (SELECT COUNT(id) number,userId FROM u_sign_in GROUP BY id HAVING userId=?) temp ON u.id=temp.userId WHERE u.id = ?';
    var args = [userId,userId];
    return dao.exeSql(sql, args);
};

exports.findUserSignInCount = function (userId) {
    var sql = 'SELECT COUNT(id) cnt FROM u_sign_in WHERE userId=?';
    var args = [userId];
    return dao.exeSql(sql, args);
};


exports.findByTaskId = function (taskId) {
    var sql = 'SELECT u.*,t.taskCode FROM t_tasks t INNER JOIN t_users u ON t.userId=u.id WHERE t.id=?';
    var args = [taskId];
    return dao.exeSql(sql, args);
};

exports.findByTaskCode = function (taskCode) {
    var sql = 'SELECT u.*,t.id taskId,t.state,t.isCompleted FROM t_tasks t INNER JOIN t_users u ON t.userId=u.id WHERE t.taskCode=?';
    var args = [taskCode];
    return dao.exeSql(sql, args);
};

exports.login = function (username,password) {
    var sql = 'SELECT * FROM t_login WHERE username = ? AND PASSWORD = ?';
    var args = [username,password];
    return dao.exeSql(sql, args);
};

// 更新创建角色数量




