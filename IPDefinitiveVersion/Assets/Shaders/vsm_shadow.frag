#version 330 core

out vec2 shadowMap;

void main()
{
    float depth = gl_FragCoord.z;
    depth = depth * 0.5 + 0.5;
	shadowMap = vec2(depth, depth * depth);   
}