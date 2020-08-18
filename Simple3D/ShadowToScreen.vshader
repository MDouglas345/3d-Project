#version 330 core
layout(location = 0) in vec3 Vertex;
layout(location = 1) in vec3 Normal;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform mat4 DepthMatrix;
uniform vec3 ObjColor;
uniform vec3 LightPos_WorldSpace;
uniform vec3 LightColor;
uniform vec3 CameraPos_WorldSpace;

out vec3 fNormal;
out vec3 fLightColor;
out vec3 fLightPos;
out vec3 fLightDir;
out vec3 fMaterialColor;
out vec3 fCameraDir;
out vec4 fDepthPosition;


void main(){
  gl_Position = MVP * vec4(Vertex, 1.0f);
  vec3 tNormal = vec3( M * vec4(Normal,0.0f));
  //fNormal = normalize(tNormal);
  fNormal = normalize(tNormal);

//Doesnt seem right...
  fDepthPosition = DepthMatrix * vec4(Vertex, 1);

  vec3 VertexPosition_WorldSpace = (M * vec4(Vertex, 1)).xyz;
  fLightDir = normalize(  LightPos_WorldSpace - VertexPosition_WorldSpace );
  fCameraDir = normalize(CameraPos_WorldSpace - VertexPosition_WorldSpace);

  fMaterialColor = ObjColor;
  fLightColor = LightColor;
  fLightPos = LightPos_WorldSpace;

}
