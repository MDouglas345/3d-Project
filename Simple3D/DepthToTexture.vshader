#version 330 core
layout(location = 0) in vec3 Vertex;
layout(location = 1) in vec3 Normal;

uniform mat4 DepthMatrix;
out vec4 VertexOut;
void main(){
  vec3 newVert = Vertex + (Normal * 0.0005f);
  gl_Position = DepthMatrix * vec4(newVert, 1);
  //gl_Position = DepthMatrix * vec4(Vertex, 1);
}
