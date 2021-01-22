#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
  
out vec2 TexCoord;

uniform mat4 view;

void main()
{
	vec4 position = view*vec4(aPos, 1.0);
    gl_Position = vec4(position.xy, position.z-0.001, position.w);
	TexCoord = aTexCoord;
}