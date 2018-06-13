#version 450

#extension GL_ARB_separate_shader_objects : enable

layout (set = 0, binding = 1) uniform samplerCube hdrProbe;

layout (push_constant) uniform pushConstants
{
	uint materialId;
} pcs;

layout (location = 0) in vec3 inUVW;

layout (location = 0) out vec4 outGbuffer1;
layout (location = 1) out vec4 outGbuffer2;
layout (location = 2) out vec4 outGbuffer3;


float packRGBA(vec4 color)
{
	uvec4 rgba = uvec4(color * 255.0 + 0.49);
	return float((rgba.r << 24) | (rgba.g << 16) | (rgba.b << 8) | rgba.a);
}


void main() 
{
	vec3 hdrColor = textureLod(hdrProbe, inUVW, 0).rgb;
	outGbuffer1 = vec4(hdrColor, 0.0);
	outGbuffer2 = vec4(0.0);
	outGbuffer3 = vec4(0.0, 0.0, 0.0, float(pcs.materialId) / 255.0);
}