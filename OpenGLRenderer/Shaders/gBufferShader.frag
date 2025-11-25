#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D diffuse;
uniform sampler2D specular;

void main() 
{
	gPosition = FragPos; // fragment position
	gNormal = normalize(Normal); // per fragment normal
	gAlbedoSpec.rgb = texture(diffuse, TexCoords).rgb; // diffuse color of object
	gAlbedoSpec.a = texture(specular, TexCoords).r; // specular color of object
}