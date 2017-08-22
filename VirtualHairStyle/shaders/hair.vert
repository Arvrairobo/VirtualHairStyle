#version 140
//#extension GL_ARB_explicit_attrib_location : require

attribute vec3 coord3d;
attribute vec2 vertexUV;

varying vec2 uv;
uniform mat4 mvp;

void main() {
	gl_Position = mvp*vec4(coord3d, 1.0f);
	uv = vertexUV;
}