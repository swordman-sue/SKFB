var when=require('when');
var config = require('../utils/config');
var dateformat = require('../utils/dateformat');
var httputils = require('../utils/httputils');
var Md5 = require('../utils/Md5');
var md5=new Md5();

var log4j = require('../utils/log4j');


/**
 * 获取鉴权凭证access_token
 * @param mCode
 * @returns {*|Promise}
 */
exports.getAccessToken = function (mCode) {
    var grant_type,client_id,sign,timestamp,scope,redirect_uri,code;

    grant_type=config.auth.grant_type;
    client_id=config.auth.client_id;
    redirect_uri=config.auth.redirect_uri;
    code=mCode;
    timestamp=""+dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    scope="1";
    sign=exports.getSign(timestamp);
    var url=config.auth.url_netAccesstoken+'&grant_type='+grant_type
        +'&client_id='+client_id
        +'&sign='+sign
        +'&timestamp='+timestamp
        +'&scope='+scope
        +'&code='+code
        +'&redirect_uri='+redirect_uri;

    return httputils.httpGet(url,'json').then(function(result){
        return result;
    });
}


exports.getAccessToken_ = function (url) {
    return httputils.httpGet(url,'json').then(function(result){
        return result;
    });
}



/**
 * 鉴权
 * @param mCode
 * @returns {*|Promise}
 */
exports.getUser = function (mCode) {
    var grant_type,client_id,sign,timestamp,scope,redirect_uri,code;

    grant_type=config.auth.grant_type;
    client_id=config.auth.client_id;
    redirect_uri=config.auth.redirect_uri;
    code=mCode;
    timestamp=""+dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    sign=exports.getSign(timestamp);
    scope="1";

    var url_Token=config.auth.url_netAccesstoken+'&grant_type='+grant_type
        +'&client_id='+client_id
        +'&sign='+sign
        +'&timestamp='+timestamp
        +'&scope='+scope
        +'&code='+code
        +'&redirect_uri='+redirect_uri;
    //console.log(url_Token);
    return exports.getAccessToken_(url_Token).then(function(accessToke){
        if(accessToke.code == config.auth.uselessCode){
            return accessToke;
        }
        var url_getUser=config.auth.url_netAuthedUser+'&sign='+sign
            +'&timestamp='+timestamp
            +'&redirect_uri='+redirect_uri
            +'&access_token='+accessToke.access_token
            +'&id='+accessToke.uid;
        return httputils.httpGet(url_getUser,'json').then(function(result){
            //console.log(result);
            result.access_token = accessToke.access_token;
            result.expires = accessToke.expires;
            result.refresh_token =accessToke.refresh_token;
            result.uid = accessToke.uid;
            return result;
        });
    });
}

//签名
exports.getSign = function (timestamp) {
    var plainText = "client_id=" + config.auth.client_id + "client_key=" + config.auth.client_key + "timestamp=" + timestamp;
    var sign = md5.encrypt(plainText);
    return sign;
}

/**
 * 获取jssdk调用凭证
 * @param mAccountId
 * @param mUrl
 * @returns {*|Promise}
 */
exports.getJssdkTicket = function (mAccountId,mUrl) {
    console.log("======="+accountId);
    var accountId,url,timestamp,client_id,grant_type,sign;

    accountId=mAccountId;
    url=mUrl;
    timestamp=""+dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    client_id=config.auth.client_id;
    grant_type=config.auth.jssdkTicket_grant_type;
    sign=exports.getSign(timestamp);

    var url_getJssdkTicket=config.auth.url_netJssdkTicket+'?accountId='+accountId
        +'&timestamp='+timestamp
        +'&client_id='+client_id
        +'&grant_type='+grant_type
        +'&sign='+sign
        +'&url='+url;
    console.log(url_getJssdkTicket);
    return httputils.httpGet(url_getJssdkTicket,'json').then(function(ticket){
        return ticket;
    });
}

/**
 * 获取南网单位组织架构
 * @returns {*|Promise}
 */
exports.getDepartMentInfo = function(access_token){
    var timestamp=""+dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    var sign=exports.getSign(timestamp);
    var url_netDept = config.auth.url_netDept
        +'&grant_type='+config.auth.grant_type
        +'&client_id='+config.auth.client_id
        +'&sign='+sign
        +'&timestamp='+timestamp
        +'&access_token='+access_token
        +'&redirect_uri='+config.auth.redirect_uri;
    return httputils.httpGet(url_netDept,'json').then(function(depts){
        return depts;
    });
}

/**
 * 发送模板消息
 * @param access_token
 * @param uid
 * @param data
 */
exports.sendTemplateMsg = function(access_token,uid,data){
    var timestamp=""+dateformat(new Date(),"yyyy-MM-dd hh:mm:ss");
    var sign=exports.getSign(timestamp);
    var url_send_templatemsg=config.auth.url_send_templatemsg+'?access_token='+access_token+'&timestamp='+timestamp+'&sign='+sign;

    console.log(url_send_templatemsg);
    var postData = {
        'scene_no': config.auth.scene_no,
        'url': config.auth.redirect_uri,
        "is_oauth": true,
        'domain_id': config.auth.domain_id,
        'send_object': [
            {
                'pid': uid,
                'type': 'PERSONAL',
                'scope': 'ONLY'
            }
        ],
        "data": data
    };
    //console.log(postData);
    httputils.httpPost(url_send_templatemsg,postData,'json').then(function(result){
        if(result.mid != undefined){
            //发送成功
        }else{
            log4j.dateFilelogger.error("模板消息发送失败:" + result.message);
            log4j.dateFilelogger.error(postData);
        }

    }).catch(function(err){
        log4j.dateFilelogger.error("模板消息发送失败" + err);
        log4j.dateFilelogger.error(postData);
    });

}




