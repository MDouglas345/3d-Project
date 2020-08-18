#version 330 core

layout(location = 0) in vec3 Vertex;

uniform mat4 MVP;
out vec3 fColor;

void main(){
  fColor = Vertex;
  gl_Position = MVP * vec4(Vertex, 1);

}
