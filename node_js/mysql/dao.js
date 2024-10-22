var when=require('when');
var mysql=require('./mysql');
exports.exeSql=function(sql,params){
    var defered=when.defer();
    mysql.getConnection().query(sql,params,function(err,result){
        if (err) {
            defered.reject(sql+","+params+";"+err);
        } else {
            defered.resolve(result);
        }
    });
    return defered.promise;
};

exports.startTransaction=function(success,error){
    var conn=mysql.getConnection();
    mysql.getConnection().beginTransaction(function(err){
        if (err) {
            error(err);
        }else{
            success(conn);
        }
    });
};

exports.patchTransaction = function (sqlArray) {
    var defered = when.defer();
    var conn = mysql.getConnection();
    conn.beginTransaction(function(err){
        if (err) {
            defered.reject(err);
        }else{
            var i = 0;
            abc(sqlArray, i, conn, function (rst) {
                defered.resolve(rst);
            }, function (err) {
                defered.reject(err);
            });
        }
    });
    return defered.promise;
};

function abc(sqlArray,i,conn,success,error,results){
    if (sqlArray.length <= i) {
        conn.commit(function(err) {
            if (err) {
                conn.rollback(function() {
                    error(err);
                });
            }else{
				//console.log('results:',results);
                success(results);
            }
        });
        return;
    }
    if(results==undefined){
        results=[];
    }
	//console.log('sqlArray[i].sql:',sqlArray[i].sql);
	//console.log('sqlArray[i].params:',sqlArray[i].params);
    conn.query(sqlArray[i].sql,sqlArray[i].params,function(err,result){
        if (err) {
            conn.rollback(function() {
                error(err);
            });
        } else {
			//console.log('result[%d]',i);
			//console.log('result:',result);
            results.push(result);
            i=i+1;
            abc(sqlArray,i,conn,success,error,results);
        }
    });
}