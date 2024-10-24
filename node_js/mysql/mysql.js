/**
 * Created by Administrator on 14-2-25.
 */
var mq = require("mysql");
var config = require('../utils/config');

var dbconfig={
    user: config.mysql.user,
    password: config.mysql.password,
    database:config.mysql.database,
    port: config.mysql.port,
    host : config.mysql.host
};

//var mc = mq.createConnection({
//    user: config.mysql.user,
//    password: config.mysql.password,
//    database:config.mysql.databas
//    port: config.mysql.port,
//    host : config.mysql.host
//});
//mc.connect();
//module.exports.getConnection=function(){
//    console.log("===========,"+mc.state);
//    if(mc==undefined || mc.state=='protocol_error'){
//        mc = mq.createConnection({
//            user: config.mysql.user,
//            password: config.mysql.password,
//            database:config.mysql.database,
//            port: config.mysql.port,
//            host : config.mysql.host
//        });
//        mc.connect();
//    }
//    return mc;
//};


var connection;

function handleDisconnect() {
    connection = mq.createConnection(dbconfig); // Recreate the connection, since
    // the old one cannot be reused.

    connection.connect(function(err) {              // The server is either down
        if(err) {                                     // or restarting (takes a while sometimes).
            console.log('error when connecting to db:', err);
            setTimeout(handleDisconnect, 2000); // We introduce a delay before attempting to reconnect,
        }                                     // to avoid a hot loop, and to allow our node script to
    });                                     // process asynchronous requests in the meantime.
    // If you're also serving http, display a 503 error.
    connection.on('error', function(err) {
        console.log('db error', err);
        if(err.code === 'PROTOCOL_CONNECTION_LOST') { // Connection to the MySQL server is usually
            handleDisconnect();                         // lost due to either server restart, or a
        } else {                                      // connnection idle timeout (the wait_timeout
            throw err;                                  // server variable configures this)
        }
    });
}

handleDisconnect();

module.exports.getConnection=function(){
    //console.log("connection.state="+connection.state);
    if(connection==undefined || connection.state=='protocol_error'){
        handleDisconnect();
    }
    return connection;
};

