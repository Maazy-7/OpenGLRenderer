#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;
in vec3 TangentViewPos;
in vec3 TangentLightPos;
in vec3 TangentFragPos;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float farPlane;
uniform sampler2D depthMap;
uniform sampler2D diffuse;
uniform samplerCube cubeDepthMap;
uniform sampler2D normalMap;
uniform sampler2D heightMap;

float calculateShadow(vec4 fragPosLightSpace, vec3 lightDirection, vec3 fragNormal) 
{
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
    float bias = max(0.05 * (1- dot(fragNormal, lightDirection)), 0.005);
    
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

float calculateShadowCubeMap(vec3 fragPos) 
{
    vec3 fragToLight = fragPos - lightPos;//vector between frag and light pos
    float closestDepth = texture(cubeDepthMap, fragToLight).r;//sampling from cube map using fragToLight direction vector
    closestDepth *= farPlane;//setting range of the depth from [0,1] to [near, plane]
    float currentDepth = length(fragToLight);//linear depth of the current fragment's depth
    float bias = 0.05;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;//determining whether the fragment is in shadow or not
    return shadow;
}

vec2 parallaxMapping(vec2 texCoords, vec3 viewDir) //parallax occlusion mapping method
{
    const float minLayers = 8;
    const float maxLayers = 32;
    //increasing the amount of layers to check based of the angle the surface is being viewed at, the less perpendicular the more layers need to be checked
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0,0.0,1.0), viewDir)));
    float layerDepth = 1.0 / numLayers; //size of each layer
    float currentLayerDepth = 0.0;//this will hold the current depth of the layer during the iterations
    float heightScale = 0.06;
    vec2 P = viewDir.xy / max(viewDir.z, 0.1) * heightScale;//the amount the texture coordinate gets shifted per layer
    vec2 deltaTexCoords = P / numLayers;

    //getting initial values
    vec2 currentTexCoords = texCoords;
    float currentDepthValue = texture(heightMap, currentTexCoords).r;

    while (currentLayerDepth < currentDepthValue) 
    {
        //shifting the texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        
        currentDepthValue = texture(heightMap, currentTexCoords).r;//depth value at current texture coordinates

        currentLayerDepth += layerDepth;//incrementing to the next layer depth
    }

    //texture coordinates right before the collision
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    //get depth values before and after collision to use for the interpolation
    float afterDepth = currentDepthValue - currentLayerDepth;//current layer depth is always less than current depth value and if its not then the while loop exits
    float beforeDepth = texture(heightMap, prevTexCoords).r - currentLayerDepth + layerDepth;//reverting layer depth to what it was before the collision

    //interpolation of the 2 texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

void main() 
{   
    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    vec2 texCoords = TexCoords;

//    texCoords = parallaxMapping(TexCoords, viewDir);
//    if (texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0) 
//    {
//        discard;
//    }

    //normal calculation
    vec3 normal = texture(normalMap, texCoords).rgb;//getting normal from normal map
    normal = normalize(normal * 2.0 - 1.0);//setting range of normal from [0,1] to [-1,1]
    //normal = normalize(Normal); // turns off normal mapping by using surface normal <- NEEDS the transformed normal from vertex shader

    vec3 color = texture(diffuse, texCoords).rgb;

    // ambient
     vec3 ambient = 0.05 * color;;
    
    // diffuse
    vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * color;

    //attenuation
    float distance = length(TangentFragPos - TangentLightPos);
    float attenuation = 1.0 / (1 + 0.09 * distance + 0.32 * distance*distance);

    diffuse *= attenuation;
    specular *= attenuation;
    
    //float shadow = calculateShadow(FragPosLightSpace, lightDir, normal);
    float shadow = calculateShadowCubeMap(FragPos);
    
    vec3 lighting = (ambient + (1.0 - shadow)*(diffuse + specular))*vec3(5.0);
    //gamma correction
    //lighting = pow(lighting, vec3(1.0/2.2)); if hdr is on then gamma correction is done in the hdr shader
    
    FragColor = vec4(lighting,1.0);

    //for the bright color output, only fragments which exceed a certain brightness will be written into the brightness texture
    float brightness = dot(FragColor.rgb, vec3(0.216, 0.7152, 0.0722));

    if (brightness > 1.0) 
    {
        BrightColor = vec4(FragColor.rgb, 1.0);
    }
    else 
    {
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}