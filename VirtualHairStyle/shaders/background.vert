#version 140
attribute vec2 coord2d;
attribute vec2 texcoord;
varying vec2 f_texcoord;

void main() {
  gl_Position = vec4(coord2d, 1.0, 1.0);
  f_texcoord = texcoord;
}