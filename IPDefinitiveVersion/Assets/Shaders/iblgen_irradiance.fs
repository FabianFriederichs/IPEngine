#version 330 core
//output
layout (location = 0) out vec3 color;

uniform samplerCube u_envcube;
uniform sampler2D u_enver;
uniform int u_envmap_type;
uniform float u_sample_delta;

in vec3 worldPos;

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
    if(u_envmap_type == 0)
    {
        return texture(u_envcube, dir).rgb;
    }
    else
    {
        return texture(u_enver, sampleErMap(dir)).rgb;
    }
}

const float PI = 3.14159265359;

void main()
{		
    // the sample direction equals the hemisphere's orientation 
    vec3 normal = normalize(worldPos);
  
    vec3 irradiance = vec3(0.0);

    vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(up, normal);
    up         = cross(normal, right);

    float sampleDelta = u_sample_delta;
    float nrSamples = 0.0; 
    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            // spherical to cartesian (in tangent space)
            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal; 

            irradiance += fetchEnvColor(sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(nrSamples));
  
    color = irradiance;
}