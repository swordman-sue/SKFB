var crypto=require('crypto');
var $=require('underscore');
var DEFAULTS = {
    encoding: {
        input: 'utf8',
        output: 'hex'
    },
    key:'szfore',
    algorithms: ['bf', 'blowfish', 'rc4']//加密算法（第三个参数）crypto.getCiphers()
};

function MixCrypto(options) {
//    if (typeof options == 'string')
//        options = { key: options };

    options = $.extend({}, DEFAULTS, options);
    this.key = options.key;
    this.inputEncoding = options.encoding.input;
    this.outputEncoding = options.encoding.output;
    this.algorithms = options.algorithms;
}

MixCrypto.prototype.encrypt = function (plaintext) {
    return $.reduce(this.algorithms, function (memo, a) {
        var cipher = crypto.createCipher(a, this.key);
        return cipher.update(memo, this.inputEncoding, this.outputEncoding)
            + cipher.final(this.outputEncoding)
    }, plaintext, this);
};

MixCrypto.prototype.decrypt = function (crypted) {
    try {
        return $.reduceRight(this.algorithms, function (memo, a) {
            var decipher = crypto.createDecipher(a, this.key);
            return decipher.update(memo, this.outputEncoding, this.inputEncoding)
                + decipher.final(this.inputEncoding);
        }, crypted, this);
    } catch (e) {
        return;
    }
};

module.exports =MixCrypto;