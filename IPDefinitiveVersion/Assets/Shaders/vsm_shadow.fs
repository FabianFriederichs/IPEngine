#version 330 core

layout (location = 0) out vec2 shadowMap;

in vec4 fragposLightSpace;

void main()
{
    float depth = fragposLightSpace.z / fragposLightSpace.w;
    depth = depth * 0.5 + 0.5;
    float depth2 = depth * depth;
    float dx = dFdx(depth);
	float dy = dFdy(depth);
	depth2 += 0.25*(dx*dx+dy*dy);
	shadowMap = vec2(depth, depth2);   
}