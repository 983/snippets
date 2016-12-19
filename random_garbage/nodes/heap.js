"use strict";

function MinHeap(isLess, onMove){
	if (isLess === undefined){
		isLess = function(a, b){
			return a < b
		}
	}
	
	if (onMove === undefined){
		onMove = function(){}
	}
	
	this.isLess = isLess
	this.onMove = onMove
	this.values = []
}

MinHeap.prototype.push = function(value){
	var values = this.values
	
	values.push(value)
	this.onMove(values.last(), values.length - 1)
	this.bubbleUp(values.length - 1)
}

MinHeap.prototype.bubbleUp = function(i){
	var values = this.values
	var isLess = this.isLess
	var onMove = this.onMove
	
	while (i != 0){
		var parent = (i - 1) >> 1
		// small values bubble up
		if (isLess(values[i], values[parent])){
			values.swap(i, parent)
			onMove(values[i], i)
			onMove(values[parent], parent)
			i = parent
		}else{
			break
		}
	}
}

MinHeap.prototype.check = function(){
	var values = this.values
	var n = values.length
	var isLess = this.isLess
	
	for (var i = 0; i < n; i++){
		var leftChild = i*2 + 1
		var rightChild = i*2 + 2
		if (leftChild < n && isLess(values[leftChild], values[i])){
			throw new Error("Value is greater than left child")
		}
		if (rightChild < n && isLess(values[rightChild], values[i])){
			throw new Error("Value is greater than right child")
		}
	}
}

MinHeap.prototype.sinkDown = function(i){
	var values = this.values
	var n = values.length
	var isLess = this.isLess
	var onMove = this.onMove
	
	while (1){
		// swap i with minimum
		var leftChild = i*2 + 1
		var rightChild = i*2 + 2
		
		// no children, nothing to swap
		if (leftChild >= n) break
		
		var minimum = rightChild >= n || isLess(values[leftChild], values[rightChild]) ?
			leftChild : rightChild
		
		if (isLess(values[minimum], values[i])){
			values.swap(i, minimum)
			onMove(values[i], i)
			onMove(values[minimum], minimum)
			i = minimum
		}else{
			break
		}
	}
}

MinHeap.prototype.removeAt = function(i){
	var values = this.values;
	var n = values.length
	
	if (i < 0 || i >= n){
		throw new Error("Index out of bounds " + i + " " + n)
	}
	
	var result = values[i]
	
	if (n > 1){
		values[i] = values.pop()
		this.onMove(values[i], i)
		this.sinkDown(i)
	}else{
		values.pop()
	}
	
	return result
}

MinHeap.prototype.pop = function(){
	return this.removeAt(0)
}

MinHeap.prototype.empty = function(){
	return this.values.length == 0
}

function testHeap(n){	
	var a = []
	var b = []
	var h = new MinHeap();

	for (var i = 0; i < n; i++){
		var x = Math.random()*5|0
		a.push(x)
		h.push(x)
		h.check()
	}

	function cmp(a, b){
		if (a < b) return -1
		if (a > b) return +1
		return 0
	}

	a.sort(cmp)

	while (!h.empty()){
		b.push(h.pop())
		h.check()
	}

	for (var i = 0; i < a.length; i++){
		if (a[i] != b[i]){
			throw new Error(a[i], "!=", b[i], "at", i)
		}
	}
}

for (var k = 0; k < 10; k++){
	testHeap(Math.random()*10|0)
}
