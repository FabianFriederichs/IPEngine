#version 330 core
  
layout (location = 0) in vec3 position;
//layout (location = 1) in vec2 uv;
//layout (location = 2) in vec3 normal;
//layout (location = 3) in vec3 tangent;

uniform mat4 u_model_matrix;
uniform mat4 u_light_view_matrix;
uniform mat4 u_light_projection_matrix;

void main()
{
    gl_Position = u_light_projection_matrix * u_light_view_matrix * u_model_matrix * vec3(position, 1.0);
}