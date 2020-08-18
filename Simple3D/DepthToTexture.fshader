#version 330 core
layout(location = 0) out float FragOut;


void main(){
  FragOut = gl_FragCoord.z;

}
