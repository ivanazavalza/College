/* 
jwf_Vertex_Shader_V1.glsl

Very simple vertex shader which takes in from the VAO associated
buffer a vectors position, color, and normal. 

This is my first attempt at making a Vertex Shader. This is built
off an original skeleton "simpleVertex.glsl" by Mike Barnes.

Jared Fowler
November 11, 2014
*/

# version 330 core

in vec4 vPosition;
in vec4 vColor;
in vec3 vNormal;

out vec4 fColor;
out vec3 fPosition;
out vec3 fNormal;

uniform mat4 ModelViewProjection;
uniform mat4 ModelViewMatrix;


void main()
{
    /*Handle vertex position in this code. Color and lights will be handled in the fragment shader.*/

    //gl_position must be set! This represents the vertex's position on the screen
    gl_Position = ModelViewProjection * vPosition;

    //Pass various information into the fragment shader
    fColor    = vColor;
    fNormal   = (normalize(ModelViewMatrix * vec4(vNormal, 0))).xyz;
    fPosition = (ModelViewMatrix * vPosition).xyz;

}