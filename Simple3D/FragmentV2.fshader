#version 330 core

in vec3 fMaterialColor;
in vec3 fLightColor;
in vec3 fLightDirection;
in vec3 fNormal;

in vec3 fCameraDirection;

out vec3 PixColor;

void main(){

vec3 AmbientColor = vec3(0.1,0.1,0.1) * fLightColor;

float Diffuse = clamp(dot(fNormal, fLightDirection), 0, 1);
vec3 DiffuseColor = Diffuse * fLightColor * 0.5;

vec3 R = reflect(-fLightDirection, fNormal);
float Spec = clamp(dot(R, fCameraDirection), 0, 1);
vec3 SpecularLight = 0.8 * Spec * fLightColor;

  PixColor = (AmbientColor + DiffuseColor + SpecularLight) * fMaterialColor;

}
