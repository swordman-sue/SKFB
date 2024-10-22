var when = require('when');
var https = require('https');
var http = require('http');
var URL = require('url');

var log4j = require('../utils/log4j');

exports.httpGet = function (url, resultType) {
    var defered = when.defer();
    http.get(url, function (res) {
        var list = [];
        var len = 0;
        res.on('data', function (data) {
            list.push(data);
            len += data.length;
        });
        res.on('end', function () {
            var _data = Buffer.concat(list, len).toString("utf-8");
            if (resultType == 'html') {
                defered.resolve(_data);
            } else {
                //log4j.dateFilelogger.info(_data);
                if(_data==""){
                    defered.reject("鉴权失败");
                }else{
                    defered.resolve(JSON.parse(_data));
                }
            }
        });
    }).on('error', function (e) {
        defered.reject(e);
    });

    return defered.promise;
};

exports.httpsGet = function (url, resultType) {
    var defered = when.defer();
    https.get(url, function (res) {
        var list = [];
        var len = 0;
        res.on('data', function (data) {
            list.push(data);
            len += data.length;
        });
        res.on('end', function () {
            var _data = Buffer.concat(list, len).toString("utf-8");
            if (resultType == 'html') {
                defered.resolve(_data);
            } else {
                defered.resolve(JSON.parse(_data));
            }
        });
    }).on('error', function (e) {
        defered.reject(e);
    });

    return defered.promise;
};

exports.httpPost = function (url, data, datatype) {
    if (datatype == undefined) {
        datatype = "json";
    }
    var defered = when.defer();
    if ('json' == datatype) {
        data = JSON.stringify(data);
    }
    var length = utf8_strlen2(data);
    var p = URL.parse(url);
    var options = {
        host: p.hostname,
        port: p.port || 80,
        path: p.path,
        method: 'POST',
        headers: {
            'Content-Type': 'application/' + datatype,
            'Content-Length': length
        }
    };
    var req = http.request(options, function (res) {
        var list = [];
        var len = 0;
        res.setEncoding('utf8');
        res.on('data', function (data) {
            list.push(data);
            len += data.length;
        });
        res.on('end', function () {
            var _data = Buffer.concat(list, len).toString("utf-8");
            if (contains(res.headers['content-type'], 'json', true)) {
                var d = JSON.parse(_data);
                log4j.dateFilelogger.info(d);
                defered.resolve(d);
            } else {
                log4j.dateFilelogger.info(d);
                defered.resolve(_data);
            }
        });
    });
    req.write(data);
    req.end();
    req.on('error', function (e) {
        defered.reject(e);
    });
    return defered.promise;
};

function utf8_strlen2(str) {
    var cnt = 0;
    for (var i = 0; i < str.length; i++) {
        var value = str.charCodeAt(i);
        if (value < 0x080) {
            cnt += 1;
        }
        else if (value < 0x0800) {
            cnt += 2;
        }
        else {
            cnt += 3;
        }
    }
    return cnt;
}

//string:原始字符串
//substr:子字符串
//isIgnoreCase:忽略大小写
function contains(string, substr, isIgnoreCase) {
    if (isIgnoreCase) {
        string = string.toLowerCase();
        substr = substr.toLowerCase();
    }
    var startChar = substr.substring(0, 1);
    var strLen = substr.length;
    for (var j = 0; j < string.length - strLen + 1; j++) {
        if (string.charAt(j) == startChar)//如果匹配起始字符,开始查找
        {
            if (string.substring(j, j + strLen) == substr)//如果从j开始的字符与str匹配，那ok
            {
                return true;
            }
        }
    }
    return false;
}