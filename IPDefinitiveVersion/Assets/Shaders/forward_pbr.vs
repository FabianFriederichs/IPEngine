#version 330 core
  
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

uniform vec2 tc_offset;
uniform vec2 tc_scale;

out struct VertexData
{
	vec3 pos;
	vec2 uv;
	vec3 normal;
	mat3 TBN;
} vertexdat;

void main()
{
	//put everything into view space
	mat4 mv = view_matrix * model_matrix;
	vertexdat.pos = (mv * position).xyz;
	gl_Position = projection_matrix * mv * position;

	vertexdat.uv = uv * tc_scale + tc_offset;

	mat3 nm = transpose(inverse(mat3(mv));
	vec3 N = normalize(nm * normal);
	vec3 T = normalize(nm * tangent);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);
	vertexdat.TBN = mat3(T, B, N);
	vertexdat.normal = N;
}