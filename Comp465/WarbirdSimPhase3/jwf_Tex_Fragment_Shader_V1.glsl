/* 
Very Simple Fragment Shader made to handle a texture.

Jared Fowler
Nov 21, 2014
*/

# version 330 core

in vec2 vs_texCoord;
out vec4 fragColor;
uniform sampler2D Texture;

void main() {
    fragColor = texture(Texture, vs_texCoord);
  }