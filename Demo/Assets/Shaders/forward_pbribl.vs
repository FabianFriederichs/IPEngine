#version 330 core
  
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;

uniform mat4 u_model_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_projection_matrix;


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
}