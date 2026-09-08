#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 ourColor; // output a color to the fragment shader
out vec2 TexCoord;

void main(){
  gl_Position = projection * view * model * vec4(aPos , 1.0);
  ourColor = aColor; // set ourColor to the input color we got from the vertex data
  TexCoord = aTexCoord;
}               
