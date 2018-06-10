#version 330 core
//output
layout (location = 0) out vec3 color;

in vec3 worldPos;
uniform sampler2D u_enver;
const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 sampleErMap(vec3 v)
{
    vec3 nv = normalize(v);
    vec2 uv = vec2(atan(nv.z, nv.x), asin(nv.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

vec3 fetchEnvColor(vec3 dir)
{   
    return texture(u_enver, sampleErMap(dir)).rgb;    
}

void main()
{		
    //color = 0.5 * vec3(worldPos.xy, -worldPos.z) + 0.5;
    color = fetchEnvColor(worldPos);
}