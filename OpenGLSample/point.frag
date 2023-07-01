#version 460 core
precision mediump float;
out vec4 fragment;
uniform float t; // time
in vec4 pos;

void main(void){
	//	fragment = (int(pos.x*10) + int(pos.y*10)) % 2 == 0 ? vec4(0.6,0.6,0.6,1.0) : vec4(0.8,0.8,0.8,1.0);
	fragment = vec4(pos.x, pos.y, 0.0, 1.0);
}