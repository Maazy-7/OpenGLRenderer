#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform samplerCube cubeDepthMap;
uniform sampler2D depthMap;
uniform sampler2D ssao;


struct Light //need to make this more compact
{
	vec3 position;
	vec3 color;
	float linear;
	float quadratic;
	int type;
};

#define N_LIGHTS 4
uniform Light lights[N_LIGHTS];
uniform int lightCount = 1;

#define PI_2 3.14159 / 2.0

uniform vec3 viewPos;
uniform mat4 viewMat;
uniform bool ssaoOn;

float calculateShadow(vec4 fragPosLightSpace, vec3 fragNormal) 
{
	//TODO need light space matrix to convert frag pos to light space
	fragPosLightSpace = vec4( inverse(viewMat) * vec4(fragPosLightSpace) );//converting fragPos from view space to world space
    //applying perspective divide to the frag pos in light space
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    //transform projCoords from NDC [-1,1] to depth range [0,1]
    projCoords = projCoords * 0.5 + 0.5;
    //sampling from the depth map using projCoords x and y components to find the fragment's depth from the light's perspective
    float closestDepth = texture(depthMap, projCoords.xy).r;//sampling .r since a depth map texture only has one component
    
    //getting the fragments current depth
    float currentDepth = projCoords.z;
    if (projCoords.z > 1.0) { return 0.0; }//if fragment outside of light's frustum than the light creates no shadow

    //calculate bias to ensure shadow acne doesn't occur
    float bias = 0.05;
    
    //PCF - softening shadow to get rid of the jagged edges on the shadows
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depthMap, 0);
    for (int x = -1; x <= 1; x++) 
    {
        for (int y = -1; y <= 1; y++) 
        {
            float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }

    shadow /= 9.0;
    return shadow;
}

float calculateShadowCubeMap(vec3 fragPos, vec3 lightPos) 
{
	fragPos = vec3(inverse(viewMat) * vec4(fragPos,1.0) );//converting fragpos from view space to world space
    vec3 fragToLight = fragPos - lightPos;//vector between frag and light pos
    float closestDepth = texture(cubeDepthMap, fragToLight).r;//sampling from cube map using fragToLight direction vector
	closestDepth *= 25.0;//setting range of the depth from [0,1] to [near, plane]
    float currentDepth = length(fragToLight);//linear depth of the current fragment's depth
    float bias = 0.05;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;//determining whether the fragment is in shadow or not
    return shadow;
}

vec3 calculateDirLight(Light light, vec3 normal, vec3 viewDir) 
{
	light.position = vec3(viewMat * vec4(light.position,1.0));//setting light position to view space
	vec3 lightDir = vec3(-light.position);
	vec3 diffuse = max(dot(normal, lightDir), 0.0) * vec3(0.8,0.0,0.0) * light.color;
	vec3 halfwayDir = normalize(lightDir + viewDir);
	vec3 specular = pow(max(dot(normal, halfwayDir),0),32) * 1.0 * light.color;
	return vec3(diffuse + specular);
}

vec3 calculatePointLight(Light light, vec3 normal, vec3 viewDir, vec3 fragPos) 
{
	light.position = vec3(viewMat * vec4(light.position,1.0));//setting light position to view space
	//diffuse
	vec3 lightDir = normalize(light.position - fragPos);
	vec3 diffuse = max(dot(normal, lightDir), 0.0) * vec3(0.8,0.0,0.0) * light.color;
	//specular
	vec3 halfwayDir = normalize(lightDir + viewDir);
	vec3 specular = pow(max(dot(normal, halfwayDir),0),32) * 1.0 * light.color;
	//attenuation
	float Distance = length(light.position - fragPos);
	float attenuation = 1.0 / (1.0 + light.linear * Distance + light.quadratic * Distance * Distance);
	diffuse *= attenuation;
	specular *= attenuation;	
	return vec3( diffuse + specular );
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
	vec3 lighting = vec3(0.8,0.0,0.0) * 0.3;// * ambientOcclusion;
	vec3 viewDir = normalize(-FragPos);

	for (int i = 0; i < lightCount || i < N_LIGHTS; i++) 
	{
		Light light = lights[i];
		vec3 point = calculatePointLight(light, Normal, viewDir, FragPos);
		vec3 directional = calculateDirLight(light, Normal, viewDir);
		float pointShadow = calculateShadowCubeMap(FragPos, lights[i].position);
		float directionalShadow = calculateShadow(vec4(FragPos,1.0), Normal);
		//one of these calculations returns 0, dependant on the light's type
		lighting += cos(light.type * PI_2) * (1 - pointShadow) * point;
		lighting += sin(light.type * PI_2) * (1 - directionalShadow) * directional;
	}

	FragColor = vec4(lighting, 1.0);
	
}