var when=require('when');
var dao = require('../mysql/dao');
var config = require('../utils/config');



//
exports.add = function(access_token,expires,refresh_token){
    var sql='INSERT INTO t_token(access_token,expires,refresh_token) VALUES (?,?,?);';
    var args=[access_token,expires,refresh_token];
    return dao.exeSql(sql,args);
}

//
exports.update = function(access_token,expires,refresh_token){
    var sql='UPDATE t_token SET access_token=?,expires=?,refresh_token=? where id =1';
    var args=[access_token,expires,refresh_token];
    return dao.exeSql(sql,args);
}

exports.findToken = function(){
    var sql='SELECT * FROM t_token';
    var args=[];
    return dao.exeSql(sql,args);
}

/**
 * 获取token
 * @returns {*|Promise}
 */
exports.getCacheToken = function(){
    var token = config.token.access_token;
    if(token == ""){
        return exports.findToken().then(function(tokens){
            var access_token = tokens[0].access_token;
            return access_token;
        });
    }else{
        var defered=when.defer();
        defered.resolve(config.token.access_token);
        return defered.promise;
    }
}

