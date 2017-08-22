#version 140
varying vec2 uv;
uniform sampler2D myTextureSampler;

void main() {
	gl_FragColor = texture(myTextureSampler, uv).rgba;
}