var routes = require('./routes/index');
var api = require('./routes/api');
var game = require('./routes/game');

exports.use = function (app) {
    app.use('/', routes);
    app.use('/users', api);
    app.use('/game', game);
}
