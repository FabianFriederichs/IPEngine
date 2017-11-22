#version 330 core
  
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out struct VertexData
{
	vec3 pos;
	vec2 uv;
	vec3 normal;
	vec3 tangent;
} vertexdat;

void main()
{
	vec4 worldpos = model * vec4(position.x, position.y, position.z, 1.0);
    gl_Position = projection * view * worldpos;
	vertexdat.pos = worldpos.xyz;
	vertexdat.uv = uv;
	vertexdat.normal = mat3(transpose(inverse(model))) * normal; //transform normal to world space
	vertexdat.pos = vec3(model * vec4(position, 1.0f));
	vertexdat.tangent = vec3(model * vec4(tangent, 1.0f));
}