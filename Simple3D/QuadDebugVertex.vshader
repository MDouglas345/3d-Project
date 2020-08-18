#version 330 core
layout(location = 0) in vec3 Vertex;
layout(location = 1) in vec2 TexCoords;

out vec2 fTexCoords;
out vec2 Test;

void main(){
  fTexCoords = TexCoords;
  Test = vec4(Vertex, 1).xy;
  gl_Position = vec4(Vertex, 1);
}
