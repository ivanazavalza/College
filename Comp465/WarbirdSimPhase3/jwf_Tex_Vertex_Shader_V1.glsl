/* 
Very Simple Vertex Shader made to handle a texture.

Jared Fowler
Nov 21, 2014
*/

# version 330 core

in vec4 vPosition;
in vec2 vTexCoord;
out vec2 vs_texCoord;  
uniform mat4 ViewProjection;

void main() {
    //Set the position of the vertex
    gl_Position = ViewProjection * vPosition;
    //Pass on through the tex coord
    vs_texCoord = vTexCoord;
}