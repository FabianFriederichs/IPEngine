#version 330 core
//output
layout (location = 0) out vec4 color;

uniform sampler2D u_input;

uniform bool horizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

in vec2 texCoords;

void main()
{             
    vec2 tex_offset = 1.0 / textureSize(u_input, 0); // gets size of single texel
    vec3 result = texture(u_input, texCoords).rgb * weight[0]; // current fragment's contribution
    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(u_input, texCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(u_input, texCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(u_input, texCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(u_input, texCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    FragColor = vec4(result, 1.0);
}