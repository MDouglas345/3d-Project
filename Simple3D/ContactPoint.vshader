#version 330 core

layout(location = 0) in vec3 Vertex;

in mat4 MVP;
in vec3 Color;

out vec3 fColor;

void main(){
  gl_Position = MVP * vec4(Vertex, 1);
  fColor = Color;

}
