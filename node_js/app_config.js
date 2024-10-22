var path = require('path');
var favicon = require('serve-favicon');
var logger = require('morgan');
var cookieParser = require('cookie-parser');
var bodyParser = require('body-parser');

exports.use = function (app,express,session) {
    // view engine setup
    app.set('views', path.join(__dirname, 'views'));
    app.set('view engine', 'ejs');

    // uncomment after placing your favicon in /public
    //app.use(favicon(__dirname + '/public/favicon.ico'));
    app.use(logger('dev'));
    app.use(bodyParser.json());
    app.use(bodyParser.urlencoded({ extended: false }));
    app.use(cookieParser());
    app.use(express.static(path.join(__dirname, 'public')));

    var opions={
        name:'connect.sid',
        secret: 'keyboard cat',
        resave: false,
        saveUninitialized: true,
        //cookie: { secure: true }
        cookie: { maxAge: 60 * 1000 * 60 *2 }
    };
    app.use(session(opions));
}