#version 330 core
#define MAX_TEXTURES 8

#define MAX_SHININESS 100.0f

//output
layout (location = 0) out vec4 color;

//Materials

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
    float alpha;
    sampler2D mtex[MAX_TEXTURES]; // 0: diffuse // 1: spec // 2: gloss // 3: normal // 4: height
    int texcount;
}; 

uniform Material material;

//input uniforms
uniform vec3 camerapos;

//input interface blocks DO IT!

in struct VertexData
{
	vec3 pos;
	vec2 uv;
	vec3 normal;
	vec3 tangent;
}   vertexdat;

//light calculating functions

void main()
{
	int texturecount = material.texcount;

	vec3 outcol = texture(material.mtex[0], vertexdat.uv).xyz;


	//color = vec4(outcol, 1.0f);
	//return;



    color = vec4(outcol, 1.0f);
    
    
}