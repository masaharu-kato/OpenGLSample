#version 150 core
precision mediump float;
out vec4 fragment;
uniform float t; // time
uniform vec2  r; // resolution

void main(void){
	float a = gl_FragCoord.y / 512.0;
	float b = gl_FragCoord.x / 512.0;
	fragment = vec4(a,b,1.0,1.0);
}