"use strict";

function clamp(x, a, b){
	if (x < a) return a
	if (x > b) return b
	return x 
}

function rgba(r, g, b, a){
	r = clamp(r*256|0, 0, 255)
	g = clamp(g*256|0, 0, 255)
	b = clamp(b*256|0, 0, 255)
	a = clamp(a, 0, 1)
	return "rgba(" + r + ", " + g + ", " + b + ", " + a + ")"
}
