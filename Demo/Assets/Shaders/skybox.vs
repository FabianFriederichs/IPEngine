#version 330 core
  
layout (location = 0) in vec3 position;

out vec3 cmCoords;

uniform mat4 u_view_matrix;
uniform mat4 u_projection_matrix;

void main()
{
	cmCoords = position;
	gl_Position = u_projection_matrix * u_view_matrix * vec4(position, 1.0);
}