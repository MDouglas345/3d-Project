#version 330 core
layout(location = 0) in vec3 Verticies;
layout(location = 1) in vec3 Normals;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform vec3 ObjColor;
uniform vec3 LightPos_WorldSpace;
uniform vec3 LightColor;
uniform float LightPower;

out vec3 fMaterialColor;
out vec3 fLightPos_CameraSpace;
out vec3 fLightDir_CameraSpace;
out vec3 fEyeDir_CameraSpace;
out vec3 fVertexPosition_CameraSpace;
out vec3 fNormalPosition_CameraSpace;
out vec3 fLightColor;
out float fLightPower;


void main(){
gl_Position = MVP * vec4(Verticies, 1);

fVertexPosition_CameraSpace = (M * V * vec4(Verticies, 1)).xyz;
fEyeDir_CameraSpace = vec3(0,0,0) - fVertexPosition_CameraSpace;

fLightPos_CameraSpace = (V * vec4(LightPos_WorldSpace, 1)).xyz;

fNormalPosition_CameraSpace = (M * V * vec4(Normals, 0)).xyz;

fLightDir_CameraSpace = fLightPos_CameraSpace + fEyeDir_CameraSpace;

fMaterialColor = ObjColor;

fLightColor = LightColor;

fLightPower = LightPower;

}
