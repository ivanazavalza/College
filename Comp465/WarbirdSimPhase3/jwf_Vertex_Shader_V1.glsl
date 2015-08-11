/* 
jwf_Vertex_Shader_V1.glsl

Very simple vertex shader which takes in from the VAO associated
buffer a vectors position, color, and normal. 

This is my first attempt at making a Vertex Shader. The idea of this vertex
shader is to set the gl_position, and prepare other variables which will be
used for light calculations on the individual fragments. This vertex shader
assumes that the values associated with the VAO buffer include position, normal,
and color. 

Jared Fowler
November 11, 2014
*/

# version 330 core


/*Input VAO Values*/
in vec4 vPosition;
in vec4 vColor;
in vec3 vNormal;

/*Output to Frag Shader*/
out vec4 fColor;
out vec3 fPosition;
out vec3 fNormal;

/*Uniform Constants*/
uniform mat4 ModelViewProjection;
uniform mat4 ModelViewMatrix;


void main()
{
    //gl_position must be set! This represents the vertex's position on the screen
    gl_Position = ModelViewProjection * vPosition;

    //Pass various information into the fragment shader
    fColor    = vColor;
    fNormal   = vec3(normalize(ModelViewMatrix * vec4(vNormal, 0)));
    fPosition = vec3(ModelViewMatrix * vPosition);

}