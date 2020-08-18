#version 330 core
out vec3 PixColor;

in vec2 fTexCoords;
in vec2 Test;

uniform sampler2D text;

void main(){
  //float depthValue = texture(depthMap, fTexCoords).r;
  float depthValue = texture(text, fTexCoords).r;
  float new = (depthValue - 1) * 80000;

  vec3 result = vec3(depthValue,depthValue, depthValue);


  //PixColor = vec4(vec3(depthValue), 1.0);
  //PixColor = vec3(depthValue, depthValue, depthValue) ;
  //PixColor = vec3(new,new,new);
 PixColor = result;



  //PixColor = Test;
}
