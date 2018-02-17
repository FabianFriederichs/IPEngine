#version 330 core
//output
layout (location = 0) out vec4 color;

uniform sampler2D u_input;

uniform bool u_horizontal;
uniform float u_kernel[3] = float[] (0.38774, 0.24477, 0.06136);

in vec2 texCoords;

void main()
{             
    vec2 tex_offset = 1.0 / textureSize(u_input, 0); // gets size of single texel
    vec4 result = texture(u_input, texCoords) * u_kernel[0]; // current fragment's contribution
    if(u_horizontal)
    {
        for(int i = 1; i < 3; ++i)
        {
            result += texture(u_input, texCoords + vec2(tex_offset.x * i, 0.0)) * u_kernel[i];
            result += texture(u_input, texCoords - vec2(tex_offset.x * i, 0.0)) * u_kernel[i];
        }
    }
    else
    {
        for(int i = 1; i < 3; ++i)
        {
            result += texture(u_input, texCoords + vec2(0.0, tex_offset.y * i)) * u_kernel[i];
            result += texture(u_input, texCoords - vec2(0.0, tex_offset.y * i)) * u_kernel[i];
        }
    }
    color = result;
}