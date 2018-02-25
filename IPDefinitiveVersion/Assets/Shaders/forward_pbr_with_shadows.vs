#version 330 core
  
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;

#define MAX_DIR_LIGHTS 2

uniform mat4 u_model_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_projection_matrix;
uniform mat4 u_light_matrix[MAX_DIR_LIGHTS];
uniform int u_num_light_matrices;

out struct VertexData
{
	vec3 pos;
	vec4 posLightSpace[MAX_DIR_LIGHTS];
	vec2 uv;
	vec3 normal;
	mat3 TBN;
} vertexdat;

void main()
{
	//put everything into view space
	mat4 mv = u_view_matrix * u_model_matrix;
	vertexdat.pos = (mv * vec4(position, 1.0f)).xyz;
	gl_Position = u_projection_matrix * mv * vec4(position, 1.0f);

	vertexdat.uv = uv;

	mat3 nm = transpose(inverse(mat3(mv)));
	vec3 N = normalize(nm * normal);
	vec3 T = normalize(nm * tangent);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);
	vertexdat.TBN = mat3(T, B, N);
	vertexdat.normal = N;
	for(int i = 0; i < u_num_light_matrices; ++i)
		vertexdat.posLightSpace[i] = u_light_matrix[i] * u_model_matrix * vec4(position, 1.0);
}