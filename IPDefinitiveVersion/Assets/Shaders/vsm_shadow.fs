#version 330 core

layout (location = 0) out vec4 shadowMap;

in vec4 fragposLightSpace;

uniform float u_shadowWarpFactor;

void main()
{
    float depth = fragposLightSpace.z / fragposLightSpace.w;
    depth = depth * 0.5 + 0.5;

    float dx = dFdx(depth);
	float dy = dFdy(depth);

    float depth2 = (depth * depth) + 0.25*(dx*dx+dy*dy);
    
    float w1depth = exp(u_shadowWarpFactor * depth);
    float w1depth2 = w1depth * w1depth;//exp(u_shadowWarpFactor * depth2);//w1depth * w1depth;// + 0.25*(dx*dx+dy*dy);    
    float w2depth = -exp(-u_shadowWarpFactor * depth);
    float w2depth2 = w2depth * w2depth;//-exp(-u_shadowWarpFactor * depth2);//w2depth * w2depth;// + 0.25*(dx*dx+dy*dy);    
	
	shadowMap = vec4(w1depth, w1depth2, w2depth, w2depth2);
}