var crypto = require('crypto');

function Md5(){
}

Md5.prototype.encrypt = function(key){
    var md5 = crypto.createHash('md5');
    md5.update(key);
    key = md5.digest('hex');
    return key;
};

module.exports =Md5;


