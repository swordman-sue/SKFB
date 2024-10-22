
/*function Car(model, year, miles) {
    var self = this;
    var model,year,miles;
    self.model = model;
    self.year = year;
    self.miles=miles;
}*/

var Car = function(model, year, miles){
    var self = this;
    var model,year,miles;
    self.model = model;
    self.year = year;
    self.miles=miles;
}

Car.prototype.toString = function(){
    return this.model+","+this.year+","+this.miles;
};

Car.prototype.getAll = function(){
    return '原型方法';
};

//var car=new Car();

module.exports =Car;

