#version 330 core
uniform sampler2D ShadowMap;

in vec3 fMaterialColor;
in vec3 fLightColor;
in vec3 fLightDirection;
in vec3 fNormal;
in vec4 fDepthPosition;

in vec3 fCameraDirection;

out vec3 PixColor;

float ShadowCalc(){
  vec3 pos = fDepthPosition.xyz * 0.5 + 0.5;
  float depth = texture(ShadowMap, pos.xy).r;
  return depth < pos.z ? 0.0 : 1.0;
}

void main(){

vec3 AmbientColor = vec3(0.1,0.1,0.1) * fLightColor;

float Diffuse = clamp(dot(fNormal, fLightDirection), 0, 1);
vec3 DiffuseColor = Diffuse * fLightColor * 0.5;

vec3 R = reflect(-fLightDirection, fNormal);
float Spec = clamp(dot(R, fCameraDirection), 0, 1);
vec3 SpecularLight = 0.8 * Spec * fLightColor;

float Shadow = ShadowCalc();

  PixColor = (AmbientColor + Shadow * (DiffuseColor + SpecularLight)) * fMaterialColor;

}
