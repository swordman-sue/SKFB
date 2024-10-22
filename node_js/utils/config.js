/**
 * 系统配置文件
 */
var config={
	recharge_callback_url:'http://127.0.0.1:40006/recharge_callback_common',
    port: 90,
    mysql: {
        user: "root",
        password: "debug",
        database: 'skfb',
        port: 3306,
        host: '127.0.0.1'
    },
};
module.exports = config;