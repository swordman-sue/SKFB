

//定义一个Hello类（包含属性和方法）
//function Hello() {
//    var name;
//    this.setName = function(thyName) {
//        name = thyName;
//    };
//    this.sayHello = function() {
//        console.log('Hello ' + name);
//        return 'Hello ' + name;
//    };
//};


var Hello = function(){
    var self = this;
    var name;
    self.setName = function(thyName) {
        name = thyName;
    };
    self.sayHello = function() {
        console.log('Hello ' + name);
        return 'Hello ' + name;
    };
};

module.exports = Hello;
