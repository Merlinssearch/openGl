#version 330 core
layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec3 aColor; // the color variable has attribute position 1
  
uniform float time; 
out vec3 ourColor; // output a color to the fragment shader

void main()
{   
    vec4 transform = vec4(cos(time) * aPos.x + sin(time) * aPos.z, aPos.y , -sin(time) * aPos.x + cos(time) * aPos.z , 1.0) ;
    gl_Position = transform;
    ourColor = aColor; // set ourColor to the input color we got from the vertex data
}               
