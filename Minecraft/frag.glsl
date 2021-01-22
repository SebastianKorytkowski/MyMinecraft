#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;
in float Occulsion;
in float Light;

uniform vec3 normal;
uniform vec3 sunlightdirection;


uniform sampler2D terraintexture;
void main()
{
	vec3 lightDir = normalize(-sunlightdirection);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
	float ambient = 0.1;

	float light = (Light)*diff + (1.0-Light)*Light;

	FragColor = (ambient+Occulsion*light*0.9)*texture(terraintexture, TexCoord);
} 