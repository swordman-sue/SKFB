var when=require('when');
var dao = require('../mysql/dao');
var config = require('../utils/config');
var authUtils = require('../utils/AuthUtils');
var userDao = require('../dao/userdao');
var tokenDao = require('../dao/tokendao');


exports.add = function(userId,content,type){
    var sql='INSERT INTO u_msg(userId,content,createTime,type) VALUES(?,?,?,?)';
    var args=[userId,content,new Date(),type];
    return dao.exeSql(sql,args).then(function(){
        exports.sendMsgTo(userId,content,type);
    });
};

exports.addMsg = function(userId,content,type){
    var sql='INSERT INTO u_msg(userId,content,createTime,type,sended) VALUES(?,?,?,?,?)';
    var args=[userId,content,new Date(),type,1];
    return dao.exeSql(sql,args);
};

exports.findByUserId = function(userId){
    var sql='SELECT u.*,DATE_FORMAT(u.createTime,\'%Y-%m-%d %T\') time1  FROM u_msg u WHERE userId = ? ORDER BY id DESC LIMIT 0,50';
    var args=[userId];
    return dao.exeSql(sql,args);
};

exports.findUnReadMsg = function(userId){
    var sql='SELECT COUNT(*) cnt FROM u_msg WHERE flag = 0 AND userId=? ';
    var args=[userId];
    return dao.exeSql(sql,args);
};

exports.readMsg = function(userId){
    var sql='UPDATE u_msg SET flag=1 WHERE userId=? ';
    var args=[userId];
    return dao.exeSql(sql,args);
};

exports.updateMsg = function(msgId){
    var sql='UPDATE u_msg SET sended = 0 WHERE id =?';
    var args=[msgId];
    return dao.exeSql(sql,args);
};



/**
 * 发送模板消息
 * @param userId 好习惯用户id
 * @returns {*}
 */
exports.sendMsgTo = function(userId,msg,type){
    return userDao.findOpenIdByUserId(userId).then(function(users){
        var openId = users[0].openId;
        tokenDao.getCacheToken().then(function(token){
            exports.getData(msg,type).then(function(data){
                authUtils.sendTemplateMsg(token,openId,data);
            });
        });
    });
};

exports.getData = function(msg,type){
    var defered=when.defer();
    msg = msg + '\n\n';
    var data =
        "{\"first\":{\"color\":\"#173177\",\"value\":"+"\""+msg+"\""+"}," +
        "\"keyword1\":{\"color\":\"#173177\",\"value\":\"好习惯系统通知\"}," +
        "\"keyword2\":{\"color\":\"#173177\",\"value\":"+"\""+getTypeMsg(type)+"\""+"}," +
        "\"remark\":{\"color\":\"#173177\",\"value\":"+"\""+getStringDate()+"\""+"}}";
    defered.resolve(data);
    return defered.promise;
};

function getTypeMsg(type){
    var typeMsg;
    if(type == 5){
        typeMsg =config.msg.signinType;
    }else if(type == 1){
        typeMsg =config.msg.signSuccessType;
    }else if(type == 2){
        typeMsg =config.msg.pauseType;
    }else if(type == 3){
        typeMsg =config.msg.befriendsType;
    }else if(type == 4){
        typeMsg =config.msg.restartSuccessType;
    }else{
        typeMsg = '一般通知';
    }
    return typeMsg;
}

function getStringDate(){
    var date = new Date();
    var time = "\n\n时间："+date.getFullYear()+"年"+(date.getMonth()+1)+"月"+date.getDate()+"日"
    return time;
}

