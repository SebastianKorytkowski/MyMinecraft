#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in float aOcculsion;
layout (location = 3) in float aLight;
  
out vec2 TexCoord;
out float Occulsion;
out float Light;

uniform mat4 view;

void main()
{
	TexCoord = aTexCoord;
	Occulsion = aOcculsion;
	Light = aLight;
	gl_Position = view*vec4(aPos, 1.0);
}