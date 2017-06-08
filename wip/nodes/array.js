"use strict";

Array.prototype.contains = function(value){
	var n = this.length
	for (var i = 0; i < n; i++){
		if (this[i] == value) return true
	}
	return false
}

Array.prototype.last = function(){
	return this[this.length - 1]
}

Array.prototype.remove = function(value){
	for (var i = 0; i < this.length; i++){
		if (this[i] === value){
			this[i] = this[this.length - 1]
			this.pop()
			i--
		}
	}
}

Array.prototype.swap = function(i, j){
	var temp = this[i]
	this[i] = this[j]
	this[j] = temp
}
