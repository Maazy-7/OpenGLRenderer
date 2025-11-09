#version 330 core

out float FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec2 screenDimensions;
#define kernelSize 64
uniform vec3 samples[kernelSize];

float radius = 0.5;
float bias = 0.025;

vec2 noiseScale = vec2(screenDimensions.x/4.0, screenDimensions.y/4.0);

layout (std140) uniform Matrices 
{
	uniform mat4 view;
	uniform mat4 projection;
};


void main() 
{
	vec3 fragPos = texture(gPosition, TexCoords).xyz;
	vec3 normal = texture(gNormal, TexCoords).rgb;
	vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);

	//TBN matrix to go from tangent space to view space
	vec3 tangent = normalize(randomVec - normal * dot(normal, randomVec));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);
	
	//iterate through sample kernel to compute the occlusion factor
	float occlusion = 0.0;
	for (int i = 0; i < kernelSize; i++) 
	{
		vec3 samplePos = TBN * samples[i];//sample vector in view space
		samplePos = fragPos + samplePos * radius;//sample vector in view space relative to fragment position
		
		//project sample pos to get position on screen using the position texture
		vec4 offset = vec4(samplePos, 1.0);
		offset = projection * offset;
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5 + 0.5;

		//sample depth
		float sampleDepth = texture(gPosition, offset.xy).z;

		//range check
		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
		//occlusion accumulation calculation
		occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
	}
	occlusion = 1.0 - (occlusion / kernelSize);
	FragColor = occlusion;
}