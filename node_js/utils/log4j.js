var log4js = require('log4js');
log4js.configure({
    appenders: [
        {
            type: 'console',
            category: "console"
        }, //控制台输出
        {
            type: "dateFile",
            /*filename: 'logs/log.log',*/
            filename: 'D:/app/node_js/logs/log.log',
            pattern: "_yyyy-MM-dd",
            alwaysIncludePattern: true,
            category: 'dateFileLog'
        },//日期文件格式
        {
            type: 'file',
            filename: 'D:/app/node_js/logs/access.log',
            maxLogSize: 1024,
            backups:4,
            category: 'normal'
        }//文件格式
    ],
    replaceConsole: true,   //替换console.log
    levels:{
        dateFileLog: 'INFO'//INFO
    }
});

/*
* appenders中配置了两个输出，一个是控制台输出，一个是文件输出。
*
* level:log4js.levels.INFO, 设置默认日志输出级别是INFO。
*   log4js的输出级别6个: trace, debug, info, warn, error, fatal
    logger.trace(‘Entering cheese testing’);
    logger.debug(‘Got cheese.’);
    logger.info(‘Cheese is Gouda.’);
    logger.warn(‘Cheese is quite smelly.’);
    logger.error(‘Cheese is too ripe!’);
    logger.fatal(‘Cheese was breeding ground for listeria.’);
* */

var dateFileLog = log4js.getLogger('dateFileLog');
exports.dateFilelogger = dateFileLog;

var fileLog = log4js.getLogger('normal');
exports.filelogger = fileLog;

exports.getlogger=function(name){
    var logger = log4js.getLogger(name);
    logger.setLevel('INFO');
    return logger;
}

exports.use = function(app) {
    //页面请求日志,用auto的话,默认级别是WARN
    //app.use(log4js.connectLogger(dateFileLog, {level:'auto', format:':method :url'}));
    app.use(log4js.connectLogger(dateFileLog, {level:'debug', format:':method :url'}));
    //app.use(log4js.connectLogger(this.getlogger('normal'), {level:'info', format:':method :url'}));
}