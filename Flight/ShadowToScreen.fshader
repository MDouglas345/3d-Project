#version 330 core
uniform sampler2D ShadowMap;

in vec3 fNormal;
in vec3 fLightColor;
in vec3 fLightPos;
in vec3 fLightDir;
in vec3 fMaterialColor;
in vec3 fCameraDir;
in vec4 fDepthPosition;

out vec3 FragColor;

float CalcShadow(){

  float V = 1.0;
  float bias = 0.0005;
  //if (fDepthPosition.x > 1.0 || fDepthPosition.x < 0.0 || fDepthPosition.y > 1.0 || fDepthPosition.y < 0.0 || fDepthPosition.w < 0.0) {
	//	return 1.0; // do not add shadow/ignore
	//}

    if (texture(ShadowMap, fDepthPosition.xy ).r < fDepthPosition.z - bias){
      V = 0.4;
    }
   return V;
}

float CalcShadow2(){
  float V = 1.0;
  float bias = 0.0005;

  if (texture(ShadowMap, fDepthPosition.xy + vec2(-0.15, 0.05)).r < fDepthPosition.z - bias){
    V -= 0.2;
  }
  if (texture(ShadowMap, fDepthPosition.xy + vec2(0.05, 0.05) ).r < fDepthPosition.z - bias){
    V -= 0.2;
  }
  if (texture(ShadowMap, fDepthPosition.xy + vec2(-0.15, -0.15) ).r < fDepthPosition.z - bias){
    V -= 0.2;
  }
  if (texture(ShadowMap, fDepthPosition.xy + vec2(0.05, -0.15)).r < fDepthPosition.z - bias){
    V -= 0.2;
  }

  return V;
}

void main(){

  //Ambient
  vec3 AmbientLight = vec3(0.05,0.05,0.05) * fLightColor ;

  //Diffuse
  float Diff = clamp(dot(fLightDir, fNormal),0, 1);
  vec3 DiffuseLight = Diff * fLightColor ;

  //Specular
  vec3 R = reflect(fLightDir, fNormal);
  float Spec = clamp(dot(R,fCameraDir), 0, 1);
  vec3 SpecularLight = 0.9 * Spec * fLightColor ;

  float Shadow = CalcShadow();

  FragColor = (AmbientLight + Shadow *(DiffuseLight + SpecularLight)) * fMaterialColor;



}
