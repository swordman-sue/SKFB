var when=require('when');
var log4j = require('../utils/log4j');
var taskdao=require('../dao/taskdao');
var chatdao=require('../dao/chatdao');



//员工提交聊天内容
//http://127.0.0.1:90/users/api/chat?userId=1&taskId=3&content=aaa说话
exports.chat = function(userId,taskId,content,res){
    taskdao.findById(taskId).then(function(result){
        if(result.length>0){
            var type=result[0].type;
            return chatdao.add(userId,taskId,type,content).then(function(){
                res.json({statusCode: 200, message: '员工提交聊天成功'});
            });
        }else{
            res.json({statusCode: 300, message: '数据库异常'});
        }
    }).catch(function(err){
        log4j.dateFilelogger.error("员工聊天异常>"+err);
        res.json({statusCode: 300, message: err});
    });
};

//查询员工聊天内容
//http://127.0.0.1:90/users/api/findchat?taskId=3
exports.findchat = function(taskId,res){
    chatdao.findByTaskId_(taskId).then(function(chats){
        res.json({statusCode: 200, chats: chats});
    }).catch(function(err){
        log4j.dateFilelogger.error("查询员工聊天异常>"+err);
        res.json({statusCode: 300, message: err});
    });
};
