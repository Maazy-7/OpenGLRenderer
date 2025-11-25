#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform samplerCube cubeDepthMap;
uniform sampler2D ssao;


struct Light 
{
	vec3 position;
	vec3 color;
	float linear;
	float quadratic;

};

#define N_LIGHTS 4
uniform Light lights[N_LIGHTS];

uniform vec3 viewPos;
uniform mat4 viewMat;
uniform bool ssaoOn;

float calculateShadowCubeMap(vec3 fragPos, vec3 lightPos) 
{
	fragPos = vec3(inverse(viewMat) * vec4(fragPos,1.0) );
    vec3 fragToLight = fragPos - lightPos;//vector between frag and light pos
    float closestDepth = texture(cubeDepthMap, fragToLight).r;//sampling from cube map using fragToLight direction vector
	closestDepth *= 25.0;//setting range of the depth from [0,1] to [near, plane]
    float currentDepth = length(fragToLight);//linear depth of the current fragment's depth
    float bias = 0.05;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;//determining whether the fragment is in shadow or not
    return shadow;
}

void main() 
{
	vec3 FragPos = texture(gPosition, TexCoords).rgb;
	vec3 Normal = texture(gNormal, TexCoords).rgb;
	vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
	float Specular = texture(gAlbedoSpec, TexCoords).a;
	float ambientOcclusion = texture(ssao, TexCoords).r;

	if (length(FragPos) < 0.0001) //early out if fragment is not part of geometry but the background
	{
		FragColor = vec4(0.2,0.3,0.3,1.0);//background color, should make this a uniform
		return;
	}

	if (ssaoOn) { ambientOcclusion = 1.0; }
	vec3 lighting = vec3(0.8,0.2,0.2) * 0.3 * ambientOcclusion;
	vec3 viewDir = normalize(viewPos - FragPos);

	for (int i = 0; i < N_LIGHTS; i++) 
	{
		Light light = lights[i];
		light.position = vec3(viewMat * vec4(light.position,1.0));//setting light position to view space
		//diffuse
		vec3 lightDir = normalize(light.position - FragPos);
		vec3 diffuse = max(dot(Normal, lightDir), 0.0) * vec3(0.8,0.2,0.2) * light.color;
		//specular
		vec3 halfwayDir = normalize(lightDir + viewDir);
		vec3 specular = pow(max(dot(Normal, halfwayDir),0),32) * 1.0 * light.color;
		//attenuation
		float Distance = length(light.position - FragPos);
		float attenuation = 1.0 / (1.0 + light.linear * Distance + light.quadratic * Distance * Distance);
		diffuse *= attenuation;
		specular *= attenuation;
		float n = max(0,1-2*i);
		float shadow = n*calculateShadowCubeMap(FragPos, lights[i].position);
		lighting += (1.0-shadow)*(diffuse + specular);
	}

	FragColor = vec4(lighting, 1.0);
	
}