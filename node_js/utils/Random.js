function Random(){
    var chars = ['0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'];
    this.chars=chars;
}

Random.prototype.getRandom = function(n){
    var chars = this.chars;
    var result = "";
    for(var i = 0; i < n ; i ++) {
        var id = Math.ceil(Math.random()*35);
        result += chars[id];
    }
    return result;
};

module.exports =Random;