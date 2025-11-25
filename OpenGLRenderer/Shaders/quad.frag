#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform sampler2D blurBuffer;
uniform float exposure;
uniform bool hdr;
uniform bool bloom;
uniform float near_plane;
uniform float far_plane;

//debug
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;


// required when using a perspective projection matrix
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}

void main()
{
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    vec3 blurColor = texture(blurBuffer, TexCoords).rgb;
    vec3 mapped = hdrColor;
    if (hdr) 
    {
        mapped += blurColor;
        //exposure tone mapping
        mapped = vec3(1.0) - exp(-mapped * exposure);
    }

    //gamma correction
    mapped = pow(mapped, vec3(1.0/gamma));
    
    //FragColor = vec4(mapped, 1.0);

    //debug

    FragColor = vec4( vec3(texture(hdrBuffer, TexCoords).rgb) ,1.0);
}