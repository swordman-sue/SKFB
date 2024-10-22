var express = require('express');
var router = express.Router();

var when=require('when');
var log4j = require('../utils/log4j');

var taskservice=require('../service/taskservice');
var chatservice=require('../service/chatservice');
var ranklistservice=require('../service/ranklistservice');

var typedao=require('../dao/typedao');
var rankingListDao=require('../dao/RankingListDao');
var msgdao = require('../dao/msgdao');
var userdao=require('../dao/userdao');


/**
 * 新建任务
 */
router.all('/api/newTask', function(req, res, next) {
    var userId, type, content, cardId;
    userId=(req.query.userId == undefined ? req.body.userId : req.query.userId);
    type=(req.query.type == undefined ? req.body.type : req.query.type);
    content=(req.query.content == undefined ? req.body.content : req.query.content);
    //cardId=(req.query.cardId == undefined ? req.body.cardId : req.query.cardId);
    cardId = 0;

    taskservice.newTaskService(userId, type, content, cardId,res);
});

/**
 * 任务详情
 */
router.all('/api/mytask', function(req, res, next) {
    var userId,taskId;
    userId=(req.query.userId == undefined ? req.body.userId : req.query.userId);
    taskId=(req.query.taskId == undefined ? req.body.taskId : req.query.taskId);

    taskservice.myTask(userId,taskId,res);
});

/**
 * 每日签到
 * http://192.168.1.184:90/users/api/everyDaySignIn?userId=1&taskId=10&unit=南方电网，广东电网，东莞供电局
 */
router.all('/api/everyDaySignIn', function(req, res, next) {
    var userId,unit,taskId;
    userId=(req.query.userId == undefined ? req.body.userId : req.query.userId);
    unit=(req.query.unit == undefined ? req.body.unit : req.query.unit);
    taskId=(req.query.taskId == undefined ? req.body.taskId : req.query.taskId);

    taskservice.everyDaySignIn(userId,"",taskId,res);
});

/**
 * 历史任务
 */
router.all('/api/historyTasks', function(req, res, next) {
    var userId;
    userId=(req.query.userId == undefined ? req.body.userId : req.query.userId);

    taskservice.historyTasks(userId,res);
});

/**
 * 重启任务
 */
router.all('/api/taskRestart', function(req, res, next) {
    var userId,taskId;
    userId=(req.query.userId == undefined ? req.body.userId : req.query.userId);
    taskId=(req.query.taskId == undefined ? req.body.taskId : req.query.taskId);

    console.log(userId+","+taskId);
    taskservice.taskRestart(userId,taskId,res);
});

/**
 * 再来一次
 */
router.all('/api/playAgain', function(req, res, next) {
    var userId,taskId;
    userId=(req.query.userId == undefined ? req.body.userId : req.query.userId);
    taskId=(req.query.taskId == undefined ? req.body.taskId : req.query.taskId);
    console.log(userId+","+taskId);
    taskservice.playAgain(userId,taskId,res);
});

/**
 *  获取用户消息
 */
router.all('/api/getMsg', function(req, res, next) {
    var userId = (req.query.userId == undefined ? req.body.userId : req.query.userId);
    return msgdao.readMsg(userId).then(function(){
        return msgdao.findByUserId(userId).then(function (messages) {
            res.json({statusCode: 200,result: messages});
        });
    }).catch(function(err){
        log4j.dateFilelogger.error("查询新消息>"+err);
        res.json({statusCode: 300, message: err});
    });
});

/**
 * 是否有新消息
 */
router.all('/api/isHaveNewMsg', function(req, res, next) {
    var userId=(req.query.userId == undefined ? req.body.userId : req.query.userId);
    msgdao.findUnReadMsg(userId).then(function(cnts){
        var cnt = cnts[0].cnt;
        if(cnt > 0){
            res.json({statusCode: 200,flag:true});
        }else{
            res.json({statusCode: 200,flag:false});
        }
    }).catch(function(err){
        log4j.dateFilelogger.error("查询是否有新消息>"+err);
        res.json({statusCode: 300, message: err});
    });
});

/**
 * 玩家提交聊天
 */
router.all('/api/chat', function(req, res, next) {
    var userId,taskId,content;
    userId=(req.query.userId == undefined ? req.body.userId : req.query.userId);
    taskId=(req.query.taskId == undefined ? req.body.taskId : req.query.taskId);
    content=(req.query.content == undefined ? req.body.content : req.query.content);

    var filter = ['习近平', '彭丽媛', '李克强', '张德江', '俞正声', '刘云山', '王岐山', '张高丽', '共产党', '中国共产党',
        '他妈的', '她妈的', '妈的', 'TMD', 'tmd', '民主', '人权', '公民政权', '选票', '公民选举', '天斌人权', '民主政体',
        '寡头政体', '一党独政', '一党遮天', '一党', '政治体制', '政党体制', '山寨', '水货', '盗版', '刻录', '刘晓波', '李红智', '梁光烈', '和平奖', '淫', '鸡巴',
        '1989六四', '性爱', '成人BT', '淫色', '龟头', '插', '穴', '精液', '兽奸', '阴茎', '肉洞', '做爱', '车震', '强奸', '玩穴', '肛交',
        'government', '政府', '巨乳', '阴蒂', '淫女', '处女', '阴毛', '阴屄', '贺龙', '我叼', '操逼', 'bitch', '女优', '阴茎', '插', '騷穴', '嫩穴', '嫩逼',
        '嫩B', '嫩b', '小穴', '錢國梁', '温总理', '淫荡', '麻黃素', '下体', '轮奸', 'B样', '骚', '贱人', 'fuck', '成人电影', '烂b',
        '烂B', '烂逼', '六四', '高潮', '吃大便', '拉屎', '肉唇', '销魂洞', '机八', '淫乱', '父女', '母子', '支那', '打炮', '抽插',
        '日穿', '三陪女', '吃鸡巴', '胯下呻吟', '呻吟', '乡巴佬', '轮干', '嫩肉', '穴口', '激情色', '黑手党', '温比', '淫B', 'Ａ片', '色情', '3p'
    ];
    for (var i = 0; i < filter.length; i++) {
        if (content.indexOf(filter[i]) != -1) {
            content = content.replace(filter[i], '***');
        }
    }

    chatservice.chat(userId,taskId,content,res);
});

/**
 * 群聊天记录
 */
router.all('/api/findchat', function(req, res, next) {
    var taskId;
    taskId=(req.query.taskId == undefined ? req.body.taskId : req.query.taskId);

    chatservice.findchat(taskId,res);
});

/**
 * 查找群组
 */
router.all('/api/findTypes', function(req, res, next) {
    typedao.findAll().then(function(types){
        res.json({'statusCode': 200,'types':types});
    }).catch(function(err){
        log4j.dateFilelogger.error("新建任务异常>"+err);
        res.json({statusCode: 300, message: err});
    });
});



/**
 * 组织战力排名
 * useless
 */
router.all('/api/unitintegral_ranklist', function(req, res, next) {
    ranklistservice.unitintegral_ranklist(res);
});

/**
 * 组织参与度排行榜
 * useless
 */
router.all('/api/unitjoin_ranklist', function(req, res, next) {
    ranklistservice.unitjoin_ranklist(res);
});

/**
 * 励志达人榜
 * useless
 */
router.all('/api/taskintegral_ranklist', function(req, res, next) {
    ranklistservice.taskintegral_ranklist(res);
});

/**
 * 宠物达人榜
 * useless
 */
router.all('/api/persontasks_ranklist', function(req, res, next) {
    ranklistservice.persontasks_ranklist(res);
});

/**
 * 全网排名
 */
router.all('/api/getFirstRankList', function(req, res, next) {
    var enterpriseid;
    enterpriseid=(req.query.enterpriseid == undefined ? req.body.enterpriseid : req.query.enterpriseid);
    rankingListDao.findRankList(0,500).then(function(result){
        res.json({result:result});
    });
});

/**
 * 二级排名
 */
router.all('/api/getSecondRankList', function(req, res, next) {
    var userId,enterpriseid;
    //enterpriseid=(req.query.enterpriseid == undefined ? req.body.enterpriseid : req.query.enterpriseid);
    userId = (req.query.userId == undefined ? req.body.userId : req.query.userId);
    userdao.findByUserId(userId).then(function(users){
        enterpriseid = users[0].secondId;
        rankingListDao.findSecondRankList(enterpriseid,0,500).then(function(result){
            res.json({result:result});
        });
    });

});

/**
 * 三级排名
 */
router.all('/api/getThirdRankList', function(req, res, next) {
    var enterpriseid;
    enterpriseid=(req.query.enterpriseid == undefined ? req.body.enterpriseid : req.query.enterpriseid);
    rankingListDao.findThirdRankList(enterpriseid,0,500).then(function(result){
        res.json({result:result});
    });
});

router.all('/api/getRankList', function(req, res, next) {
    var enterpriseid,type,pageNum;
    pageNum=(req.query.pageNum == undefined ? req.body.pageNum : req.query.pageNum);
    type=(req.query.type == undefined ? req.body.type : req.query.type);
    enterpriseid=(req.query.enterpriseid == undefined ? req.body.enterpriseid : req.query.enterpriseid);
    pageNum = Number(pageNum);
    var pageSize = 50;
    var isLast =false;
    console.log(pageNum+","+type+","+enterpriseid);
    if(type == 1){
        return rankingListDao.findRankList(pageNum * pageSize,pageSize).then(function(result){
            if(result.length < 50){
                isLast = true;
            }
            res.json({pageNum : pageNum+1,pageSize : pageSize,isLast : isLast,result : result});
        });
    }else if(type == 2){
        return rankingListDao.findSecondRankList(enterpriseid,pageNum * pageSize,pageSize).then(function(result){
            if(result.length < 50){
                isLast = true;
            }
            res.json({pageNum : pageNum+1,pageSize : pageSize,isLast : isLast,result : result});
        });
    }else{
        return rankingListDao.findThirdRankList(enterpriseid,pageNum * pageSize,pageSize).then(function(result){
            if(result.length < 50){
                isLast = true;
            }
            res.json({pageNum : pageNum+1,pageSize : pageSize,isLast : isLast,result : result});
        });
    }
});


module.exports = router;
