#version 330 core

//output
layout (location = 0) out vec4 color;

in vec3 cmCoords;

uniform samplerCube u_skybox;
uniform sampler2D u_skyer;
uniform int u_envmap_type;

#define PI 3.14159265

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 sampleErMap(vec3 v)
{
    vec3 nv = normalize(v);
    vec2 uv = vec2(atan(nv.z, nv.x), asin(nv.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
    //this version fixes the rotation problem
    //but is a bit more expensive
    // vec3 vn = normalize(v);
    // mat3 rotCorrection = mat3(
    //     0.0, 0.0, 1.0,
    //     0.0, 1.0, 0.0,
    //     -1.0, 0.0, 0.0
    // );
    // vn = rotCorrection * vn;   
    // float lat = acos(-vn.y) / PI;
    // float lon = clamp((atan(vn.z, vn.x) + PI) / (2.0 * PI), 0.0, 1.0);
    // return vec2(lon, lat);
}

void main()
{
    if(u_envmap_type == 0)
    {
        color = vec4(texture(u_skybox, cmCoords).rgb, 1.0f);
    }
    else if(u_envmap_type == 1)
    {
        color = vec4(texture(u_skyer, sampleErMap(cmCoords)).rgb, 1.0f);
        //color = vec4(sampleErMap(cmCoords).x, sampleErMap(cmCoords).y, 0.0, 1.0f);
        //color = vec4(0.5 * normalize(cmCoords).yyy + 0.5, 1.0f);
    }
    else
        color = vec4(1.0f);   
}