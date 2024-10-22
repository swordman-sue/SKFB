var when=require('when');
var dao = require('../mysql/dao');


exports.findAll = function(){
    var sql="SELECT * FROM t_type";
    var args=[];
    return dao.exeSql(sql,args);
};

exports.findById = function(id){
    var sql="SELECT * FROM t_type WHERE id=?";
    var args=[id];
    return dao.exeSql(sql,args);
};

exports.findByName = function(name){
    var sql="SELECT * FROM t_type WHERE name=?";
    var args=[name];
    return dao.exeSql(sql,args);
};

exports.add = function(type){
    var sql="INSERT INTO t_type(name,role) VALUES(?,?)";
    var args=[type.name,type.role];
    return dao.exeSql(sql,args);
};



