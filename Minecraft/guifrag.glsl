#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D guitexture;


void main()
{
	FragColor = texture(guitexture, TexCoord);
} 