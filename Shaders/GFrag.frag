#version 330 core

struct Material
{
	sampler2D shadowMap;
	sampler2D diffuseTex;
    sampler2D displacementTex;
	sampler2D normalTex;
	vec2 textureScale;
};

uniform Material material;

layout (location = 0) out vec3 gViewPos;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out float gDisplacement;
layout (location = 4) out vec3 gTangent;
layout (location = 5) out vec3 gBitangent;
layout (location = 6) out vec3 gTexNormal;
//layout (location = 7) out mat4 gModelMatrix;

in vec3 ModelNormal;
in vec3 ViewNormal;
in vec2 TexCoords;
in vec3 ViewPos;
in vec3 ModelPos;
in vec3 Tangent;
in vec3 Bitangent;


void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gViewPos = ViewPos;

    /*gModelMatrix.r = vec4(inverseModel[0][0], inverseModel[1][0], inverseModel[2][0], inverseModel[3][0]);
    gModelMatrix.g = vec4(inverseModel[0][1], inverseModel[1][1], inverseModel[2][1], inverseModel[3][1]);
    gModelMatrix.b = vec4(inverseModel[0][2], inverseModel[1][2], inverseModel[2][2], inverseModel[3][2]);
    gModelMatrix.a = vec4(inverseModel[0][3], inverseModel[1][3], inverseModel[2][3], inverseModel[3][3]);*/

    gNormal = normalize(ModelNormal); 

    gTexNormal = texture(material.normalTex, TexCoords * material.textureScale).rgb;
    // and the diffuse per-fragment color
    gAlbedoSpec.rgb = texture(material.diffuseTex, TexCoords * material.textureScale).rgb;
    // store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = 1.0;
    // store displacement texture in gdispalcement
    gDisplacement = texture(material.displacementTex, TexCoords * material.textureScale).r;
    // store tangent vector
    gTangent = Tangent;
    //gTangent = vec3(TexCoords,0.0);
    gBitangent = Bitangent;
}  

