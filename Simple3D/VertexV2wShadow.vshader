#version 330 core
layout(location = 0) in vec3  Vertex;
layout(location = 1) in vec3 Normal;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform vec3 ObjColor;
uniform vec3 LightPos_WorldSpace;
uniform vec3 LightColor;
uniform float LightPower;
uniform vec3 CameraPosition;
uniform mat4 DepthMatrix;

out vec3 fLightDirection;
out vec3 fMaterialColor;
out vec3 fLightColor;
out vec3 fNormal;
out vec4 fDepthPosition;


out vec3 fCameraDirection;


void main(){
  gl_Position = MVP * vec4(Vertex, 1);
  fDepthPosition = DepthMatrix * M * vec4(Vertex, 1);

  vec3 m_VertexPosition = (M * vec4(Vertex, 1)).xyz;
  vec3 m_NormalPosition = (M * vec4(Normal, 1)).xyz;

  fLightDirection = normalize(LightPos_WorldSpace - m_VertexPosition);
  fNormal = normalize(m_NormalPosition);
  fMaterialColor = ObjColor;
  fLightColor = LightColor;

  fCameraDirection = normalize(CameraPosition - m_VertexPosition);










}
