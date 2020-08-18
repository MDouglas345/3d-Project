#version 330 core

in vec3 fColor;

out vec3 Frag;




void main(){
  /*vec3 newColor = normalize(fColor);

  if (newColor.y >= 0 && newColor.y < 0.1 ){
    Frag = vec3(0.2 * newColor.y, 0.2 * newColor.y, 1 * newColor.y);
  }
  if (newColor.y >= 0.1 && newColor.y < 0.12 ){
    Frag = vec3(0.8 * newColor.y, 0.8 * newColor.y, 0.8 * newColor.y);
  }
  if (newColor.y >= 0.12 && newColor.y < 0.5 ){
    Frag = vec3(0.2 * newColor.y, 0.4 * newColor.y, 0.2 * newColor.y);
  }
  if (newColor.y >= 0.5 && newColor.y < 0.7 ){
    Frag = vec3(0.5 * newColor.y, 0.2 * newColor.y, 0.2 * newColor.y);
  }
  if (newColor.y > 0.7 && newColor.y < 1.01 ){
    Frag = vec3(1 * newColor.y, 1 * newColor.y, 1 * newColor.y);
  }*/

  float yFactor = fColor.y / 100;
  if (yFactor >= 0 && yFactor < 0.3 ){
    Frag = vec3(0.2 * yFactor, 0.2 * yFactor, 1 * yFactor);
  }
  if (yFactor >= 0.3 && yFactor < 0.32 ){
    Frag = vec3(0.8 * yFactor, 0.8 * yFactor, 0.8 * yFactor);
  }
  if (yFactor >= 0.32 && yFactor < 0.6 ){
    Frag = vec3(0.2 * yFactor, 0.4 * yFactor, 0.2 * yFactor);
  }
  if (yFactor >= 0.6 && yFactor < 0.8 ){
    Frag = vec3(0.5 * yFactor, 0.2 * yFactor, 0.2 * yFactor);
  }
  if (yFactor > 0.8 && yFactor < 1.01 ){
    Frag = vec3(1 * yFactor, 1 * yFactor, 1 * yFactor);
  }
//Frag = vec3(0.2 , 0.6 * fColor.y, 0.5 * fColor.y);
}
