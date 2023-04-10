#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 gs_vertex_object_position[];
in vec2 gs_vertex_object_texcoord[];
//in vec3 gs_vertex_object_normal[];

in vec3 vsViewPos[];
in vec3 vsModelPos[];
in vec3 vsModelNormal[];
in vec3 vsViewNormal[];

out vec3 ModelNormal;
out vec3 ViewNormal;
out vec3 ViewPos;
out vec3 ModelPos;
out vec2 TexCoords;
out vec3 Tangent;
out vec3 Bitangent;

uniform mat4 model;




vec3 calculateSurfaceTangent(vec3 edge1, vec3 edge2, vec2 deltaUV1, vec2 deltaUV2) {
    vec3 tangent;
    float f = deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y;
    float epsilon = 0.0001;
    if (abs(f) < epsilon) {
        tangent = vec3(1.0, 1.0, 1.0); // fallback to a default value
    }
    else {
        f = 1.0 / f;
        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent = normalize(tangent);
    }
    return tangent;
}

/*
vec3 calculateSurfaceBitangent(vec3 normal, vec3 tangent, vec2 uv0, vec2 uv1, vec2 uv2) {
    vec3 bitangent = cross(normal, tangent);
    float handedness = (uv1.x - uv0.x) * (uv2.y - uv0.y) - (uv2.x - uv0.x) * (uv1.y - uv0.y) > 0.0 ? -1.0 : 1.0;
    bitangent = bitangent * handedness;
    
    // Use Gram-Schmidt process to make bitangent orthogonal to both normal and tangent
    //bitangent -= normal * dot(bitangent, normal);
    //bitangent -= tangent * dot(bitangent, tangent);
    //bitangent = normalize(bitangent);

    //return vec3(0,1,0);
    return bitangent;

    
}*/


vec3 calculateSurfaceBitangent(vec3 edge1, vec3 edge2, vec2 deltaUV1, vec2 deltaUV2) {
    vec3 bitangent;
    float f = deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y;
    float epsilon = 0.0001;
    if (abs(f) < epsilon) {
        bitangent = vec3(1.0, 1.0, 1.0); // fallback to a default value
    }
    else {
        f = 1.0 / f;
        bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent = normalize(bitangent);
    }
    return bitangent;
}



void main()
{
    vec3 pos0 = gs_vertex_object_position[0];
    vec3 pos1 = gs_vertex_object_position[1];
    vec3 pos2 = gs_vertex_object_position[2];

    vec2 uv0 = gs_vertex_object_texcoord[0];
    vec2 uv1 = gs_vertex_object_texcoord[1];
    vec2 uv2 = gs_vertex_object_texcoord[2];

    vec3 edge1 = pos1 - pos0;
    vec3 edge2 = pos2 - pos0;

    vec2 deltaUV1 = uv1 - uv0;
    vec2 deltaUV2 = uv2 - uv0;

    
    // Calculate the normal, tangent, and bitangent vectors
    vec3 normal = normalize(vsModelNormal[0] + vsModelNormal[1] + vsModelNormal[2]);
    vec3 tangent = normalize(calculateSurfaceTangent(edge1, edge2, deltaUV1, deltaUV2));
    vec3 bitangent = normalize(calculateSurfaceBitangent(edge1, edge2, deltaUV1, deltaUV2));


    vec3 crossProduct = cross(normal,tangent);
    float dotProduct = dot(crossProduct, bitangent);

    // If the dot product is negative, invert the tangent vector
    if (dotProduct < 0.0)
    {
        tangent = -tangent;
    }

    // Recalculate the bitangent vector using the cross product between the tangent vector and the normal vector
    //bitangent = cross(normal, tangent);
    //vec3 bitangent = normalize(calculateSurfaceBitangent(normal, tangent, uv0, uv1, uv2));



    for (int i = 0; i < 3; i++)
    {
        gl_Position = gl_in[i].gl_Position;
        ViewPos = vsViewPos[i];
        ModelPos = vsModelPos[i];
        TexCoords = gs_vertex_object_texcoord[i];
        ModelNormal = vsModelNormal[i];
        ViewNormal = vsViewNormal[i];
        Tangent = normalize(mat3(model) * tangent);
        Bitangent = normalize(mat3(model) * bitangent);
        EmitVertex();
    }
    
    EndPrimitive();
}




