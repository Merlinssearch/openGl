#version 330 core
layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec3 aColor; // the color variable has attribute position 1

uniform float time;
// uniform vec4 timeColor;
out vec3 ourColor; // output a color to the fragment shader

void main()
{   
    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor;
    // if (gl_VertexID == 0 ) {
    //   ourColor = aColor * vec3((sin(time*2)+1)/2,(-sin(time*2)+1)/2,sin(time)*0);
    // } else if (gl_VertexID == 1) {
    //   ourColor = aColor * vec3((sin(time*2)+1)/2,(-sin(time*2)+1)/2,sin(time)*0);
    // }else if (gl_VertexID == 2) {
    //   ourColor = aColor * vec3((sin(time*2)+1)/2,(-sin(time*2)+1)/2,sin(time)*0);
    // }
}          
