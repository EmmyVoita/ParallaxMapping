#version 330 core


layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_color;
layout (location = 2) in vec2 vertex_texcoord;
layout (location = 3) in vec3 vertex_normal;


// Out for calc Tangent/Bitangent
out vec3 gs_vertex_object_position;
out vec2 gs_vertex_object_texcoord;
//out vec3 gs_vertex_object_normal;

out vec3 vsViewPos;
out vec3 vsModelPos;
out vec3 vsModelNormal;
out vec3 vsViewNormal;


uniform bool invertedNormals;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    // pass the object space vertex position to the geometry shader (in blender object file they are stored in object space)
    gs_vertex_object_position = vertex_position;

    // pass the texture coordinates to the geometry shader  (in blender object file they are stored in object space)
    gs_vertex_object_texcoord = vertex_texcoord;

    // pass the object space normals to the geometry shader (in blender object file they are stored in object space)
    //gs_vertex_object_normal = (texture(material.normalTex, vertex_texcoord * material.textureScale).rgb * 2.0 - 1);

    // transform the vertex position into clip-space
    gl_Position = projection * view * model * vec4(vertex_position, 1.0);
    
    vsViewPos = vec3(view * model * vec4(vertex_position, 1.0));
    vsModelPos = normalize(vec3(vec4(vertex_position, 1.0)));

    vsModelNormal =  mat3(transpose(inverse(model))) * (invertedNormals ? -vertex_normal : vertex_normal);
    vsViewNormal = mat3(transpose(inverse(view * model))) * (invertedNormals ? -vertex_normal : vertex_normal);  

}
