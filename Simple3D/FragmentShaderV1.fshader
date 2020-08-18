#version 330 core
in vec3 fMaterialColor;
in vec3 fLightPos_CameraSpace;
in vec3 fLightDir_CameraSpace;
in vec3 fEyeDir_CameraSpace;
in vec3 fVertexPosition_CameraSpace;
in vec3 fNormalPosition_CameraSpace;
in vec3 fLightColor;
in float fLightPower;

out vec3 PixColor;

void main(){
vec3 n = normalize(fNormalPosition_CameraSpace);
vec3 l = normalize(fLightDir_CameraSpace);
vec3 e = normalize(fEyeDir_CameraSpace);
vec3 r = reflect(-l, n);

float Distance = length(fLightDir_CameraSpace);

float Theta = clamp(dot(n,l),0,1);
float cosAlpha = clamp(dot(e,r),0,1);

vec3 MaterialAmbientColor = vec3(0.1, 0.1, 0.1) * fMaterialColor;

PixColor = MaterialAmbientColor + fMaterialColor * fLightColor * fLightPower * Theta / (Distance * Distance);
//PixColor = fMaterialColor;
}