#version 330 core

//output
layout (location = 0) out vec4 color;

in vec3 cmCoords;

uniform samplerCube u_skybox;

void main()
{
    color = vec4(texture(u_skybox, cmCoords).rgb, 1.0f);   
}