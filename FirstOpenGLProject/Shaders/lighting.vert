#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 FragPosLightSpace;
out vec3 TangentViewPos;
out vec3 TangentLightPos;
out vec3 TangentFragPos;

layout (std140) uniform Matrices
{
	uniform mat4 view;
	uniform mat4 projection;
};

uniform mat4 model;
uniform mat4 lightSpaceMatrix;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	FragPos = vec3(model * vec4(aPos,1.0));
	TexCoords = aTexCoords;
	
	Normal = mat3(transpose(inverse(model)))*aNormal;
	mat3 normalMatrix = transpose(inverse(mat3(model)));

	vec3 T = normalize(normalMatrix * aTangent);
	vec3 N = normalize(normalMatrix * aNormal);

	//Gram Schmidt process to re-orthogonalize the Tanget vector so that its orthogonal to the surface normal
	T = normalize(T - dot(N,T) * N);
	//N and T can be crossed to get the bit Tangent vector
	vec3 B = cross(N,T);

	mat3 TBN = transpose(mat3(T, B, N));
	//moving lighting variables to tangent space
	TangentViewPos = TBN * viewPos;
	TangentLightPos = TBN * lightPos;
	TangentFragPos = TBN * FragPos;//FragPos will be interpolated still so its calculation still works
	
	FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0); //for directional light shadow map

	gl_Position = projection * view * vec4(FragPos, 1.0);
}