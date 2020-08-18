#version 330 core
layout(location = 0) in vec3 Vertex;

uniform mat4 depthMVP;
uniform mat4 Model;


void main(){

  gl_Position = depthMVP * Model * vec4(Vertex, 1);
}
