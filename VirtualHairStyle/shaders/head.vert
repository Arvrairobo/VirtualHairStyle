#version 140
attribute vec3 coord3d;
out vec3 vcolor;

uniform mat4 mvp;

void main() {
  vcolor = coord3d;

  gl_Position = mvp*vec4(coord3d, 1.0f);
}