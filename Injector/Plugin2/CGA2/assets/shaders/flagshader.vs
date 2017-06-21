#version 330 core
//Vertex Attribute
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

//world, view und projection matrix
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;

//Daten fuer den Fragment Shader
out struct VertexData
{
	vec2 uv;
	vec3 normal;
} vertexdat;

void main()
{
	//gl_Position berechnen
	gl_Position = proj_mat * view_mat * world_mat * vec4(position, 1.0);

	//uv Koordinate setzen
	vertexdat.uv = uv;
	//Normale in Viewspace transformieren
	vertexdat.normal = normalize((transpose(inverse(view_mat * world_mat)) * vec4(normal, 0.0)).xyz);
	
}