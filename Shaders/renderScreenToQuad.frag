#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D gViewPos;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gDisplacement;
uniform sampler2D gTangent;
uniform sampler2D gBitangent;
uniform sampler2D gTexNormal;
uniform sampler2D ssao;

const float ParallaxSteps = 100.0; // number of steps for parallax mapping
const float ParallaxHeightScale = 0.15; // height scale for parallax mapping

struct PointLight
{
	vec3  Position;
	float Intensity;
	vec3  Color;
	float Constant;
	float Linear;
	float Quadratic;
};

uniform PointLight lights[10];
uniform int NR_LIGHTS = 3;
uniform vec3 cameraPos;

uniform mat4 invView;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    // number of depth layers
    const float minLayers = 30;
    const float maxLayers = 60;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    
    float numSteps = min(100, ParallaxSteps);
    // depth of current layer
    float currentLayerDepth = 0.0;
    // calculate the size of each layer
    float layerDepth = 1.0 / numSteps;

    // get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(gDisplacement, currentTexCoords).r;
    
    // the amount to shift the texture coordinates per layer 
    vec2 deltaTexCoords = viewDir.xy * ParallaxHeightScale / (viewDir.z * numSteps);
  

    for (float i = 0.0f; i < numSteps; i++)
    {
        if (currentLayerDepth < currentDepthMapValue)
        {
            // shift texture coordinates along direction of P
            currentTexCoords -= deltaTexCoords;
            // get depthmap value at current texture coordinates
            currentDepthMapValue = texture(gDisplacement, currentTexCoords).r;  
            // get depth of next layer
            currentLayerDepth += layerDepth;  
        }
        else
        {
            break;
        }
    }
    
    return currentTexCoords;

    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(gDisplacement, prevTexCoords).r - currentLayerDepth + layerDepth;
    
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;  
}

/*vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    // number of depth layers
    const float minLayers = 8;
    const float maxLayers = 32;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    //numLayers = 32;
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = normalize(viewDir.xy / viewDir.z) * ParallaxHeightScale; 
    vec2 deltaTexCoords = P / numLayers;
  
    // get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(gDisplacement, currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(gDisplacement, currentTexCoords).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }
    
    return currentTexCoords;

    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(gDisplacement, prevTexCoords).r - currentLayerDepth + layerDepth;
    
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;  
}*/

/*vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    // number of depth layers
    const float minLayers = 30;
    const float maxLayers = 60;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  

    float numSteps = min(100, ParallaxSteps);
    float height = 1.0;
    float stepNum = 1.0 / numSteps;

    vec2 offset = texCoords.xy;
    float HeightMap = texture(gDisplacement, offset).r;

    vec2 delta = viewDir.xy * ParallaxHeightScale / (viewDir.z * numSteps);

    for (float i = 0.0f; i < numSteps; i++)
    {
        if (HeightMap < height)
        {
            height -= stepNum;
            offset += delta;
            HeightMap = texture(gDisplacement, offset).r;
        }
        else
        {
            break;
        }
    }

    return offset;
    
}*/

void main()
{
    
    // Calculate TBN matrix
    vec3 T = vec3(texture(gTangent, TexCoord).xyz);
    vec3 B = vec3(texture(gBitangent, TexCoord).xyz);
    vec3 N = vec3(texture(gNormal, TexCoord).xyz);
    mat3 TBN = transpose(mat3(T, B, N));
    
    float D = dot(T,N);


    vec3 fragPos = texture(gViewPos, TexCoord).rgb;
    vec4 worldPos = invView * vec4(fragPos, 1.0);
    vec3 worldPos3 = worldPos.xyz / worldPos.w;

    vec3 objectView3 =  cameraPos - worldPos3 ;

    

    vec3 TangentFragPos = TBN * worldPos3;
    // Transform the camera position to tangent space
    vec3 TangentViewPos = TBN * cameraPos;

    // Calculate the view direction vector in tangent space
    vec3 viewDir = normalize(TBN * objectView3);




    // parallax mapping 
    vec2 P = ParallaxMapping(TexCoord, viewDir);
    if(P.x > 1.0 || P.y > 1.0 || P.x < 0.0 || P.y < 0.0)
        discard;


    // obtain normal from normal map
    vec3 Normal = texture(gTexNormal, P).rgb;
    Normal = normalize(Normal * 2.0 - 1.0);  
     // get diffuse color 
    vec3 Diffuse = texture(gAlbedoSpec, P).rgb;
    // get ambient occlusion
    float AmbientOcclusion = texture(ssao, P).r;


    // apply lighting 
    vec3 Lighting = vec3(0.0);
    for (int i = 0; i < NR_LIGHTS; ++i) 
    {
        vec3 TangentLightPos = TBN * lights[i].Position;
        vec3 lightDir = normalize(TangentLightPos - TangentFragPos);

        // diffuse
        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color * lights[i].Intensity;

        // specular
        vec3 reflectDir = reflect(-lightDir, Normal);
        vec3 halfwayDir = normalize( lightDir+ viewDir );
        float spec = pow(max(dot(reflectDir, halfwayDir), 0.0), 64); // adjusted exponent
        vec3 specular = lights[i].Color * spec * lights[i].Intensity;

        // attenuation
        float distance = length(TangentLightPos - TangentFragPos);
        float attenuation = 1.0 / (lights[i].Constant + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
        diffuse *= attenuation;
        specular *= attenuation;

        Lighting += diffuse + specular;
    }

    // apply ambient occlusion to the new lighting
    vec3 Ambient = vec3(0.5 * Diffuse); //* AmbientOcclusion;
    Lighting += Ambient;

    // output the final color

    vec3 lDiffuse = texture(gAlbedoSpec, TexCoord).rgb;
    FragColor = vec4(Lighting, 1.0);
    //FragColor = vec4(1.0,1.0,1.0, 1.0) * AmbientOcclusion;
}



