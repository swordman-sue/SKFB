var when=require('when');
var log4j = require('../utils/log4j');

var ranklistdao=require('../dao/ranklistdao');


//组织战力排名
exports.unitintegral_ranklist = function(res){
    var defered=when.defer();
    return ranklistdao.findAllUnitInteral().then(function(allUnitInteral){
        if(allUnitInteral.length<=0){
            //res.send('没有单位参与..');
            res.json({statusCode:300,msg:'没有单位参与..'});

        }
        //计算
        var count = allUnitInteral.length;
        var k = count;
        function jsUnitFight(){
            var j = count - k;
            while (k-- == 0) {
                res.json({statusCode:200,allUnitInteral:allUnitInteral});
                return;
            }
            ranklistdao.findUnitUsersCount(allUnitInteral[j].unit).then(function(user_counts){
                var user_count=user_counts[0].user_count;
                console.log("--------------------------"+j);
                allUnitInteral[j].user_count=user_count;
                if(user_count==0){
                    allUnitInteral[j].unitfight=0;
                }else{
                    var fight=allUnitInteral[j].unitIntegral/user_count;
                    fight=Math.round(fight*Math.pow(10, 2))/Math.pow(10, 2);
                    allUnitInteral[j].unitfight=fight;
                }
                jsUnitFight();
            });
        }
        jsUnitFight();
        //计算
    });
};

//组织参与度排行榜
exports.unitjoin_ranklist = function(res){
    ranklistdao.findUnitUsersNum().then(function(usersNums){
        if(usersNums.length<=0){
           // res.send('没有单位参与..');
            res.json({statusCode:300,msg:'没有单位参与..'});
            return;
        }
        //计算
        var count = usersNums.length;
        var k = count;
        function jsUnitDegree(){
            var j = count - k;
            while (k-- == 0) {
                //res.send(usersNums);
                res.json({statusCode:200,usersNums:usersNums});
                return;
            }
            ranklistdao.findUnitTotalUserCount(usersNums[j].unit).then(function(unit_user_counts){
                var staffcount=unit_user_counts[0].staffcount;
                console.log("--------------------------"+j);
                usersNums[j].staffcount=staffcount;
                if(staffcount==0){
                    usersNums[j].unitdegree=0;
                }else{
                    var unitdegree=usersNums[j].unit_users_count/staffcount;
                    unitdegree=Math.round(unitdegree*Math.pow(10, 2))/Math.pow(10, 2);
                    usersNums[j].unitdegree=unitdegree;
                }
                jsUnitDegree();
            });
        }
        jsUnitDegree();

    });
};

//励志达人榜
//http://127.0.0.1:90/users/api/taskintegral_ranklist
exports.taskintegral_ranklist = function(res){
    ranklistdao.taskintegral_ranklist().then(function(rst){
        res.json({statusCode: 200, result: rst});
    }).catch(function(err){
        log4j.dateFilelogger.error("励志达人榜异常>"+err);
        res.json({statusCode: 300, message: err});
    });
};

//宠物达人榜
//http://127.0.0.1:90/users/api/persontasks_ranklist
exports.persontasks_ranklist = function(res){
    ranklistdao.persontasks_ranklist().then(function(rst){
        res.json({statusCode: 200, result: rst});
    }).catch(function(err){
        log4j.dateFilelogger.error("宠物达人榜异常>"+err);
        res.json({statusCode: 300, message: err});
    });
};