#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	vec3 color;
};

uniform Material material;

float near = 0.1; 
float far  = 10.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main() 
{
	float depth = LinearizeDepth(gl_FragCoord.z)/far;
	FragColor = vec4(material.color, 1.0);
}