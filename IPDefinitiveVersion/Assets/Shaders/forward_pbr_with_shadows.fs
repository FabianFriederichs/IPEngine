#version 330
#define MAX_TEXTURES 4
//more light sources with uniform buffer or shader storage buffer thingy later
#define MAX_DIR_LIGHTS 2
#define MAX_POINT_LIGHTS 8
#define MAX_SPOT_LIGHTS 8

//output
layout (location = 0) out vec4 color;

//input
in struct VertexData
{
	vec3 pos;
    vec4 posLightSpace[MAX_DIR_LIGHTS];
	vec2 uv;
	vec3 normal;
    mat3 TBN;
}   vertexdat;

//Types
struct Tex2D {
    sampler2D tex;
    vec2 scale;
    vec2 offset;
};

struct Material {
    //sampler2D mtex[MAX_TEXTURES]; //0: albedo; 1: MRAR: r=metallic, g=roughness, b=ao, a=insulator reflectance at normal incidence; 2: normal; 3: emissive
    Tex2D albedo;
    Tex2D mrar;
    Tex2D normal;
    Tex2D emissive;
    //int texcount;
}; 

struct DirLight {
    vec3 color;
    vec3 direction;
    bool enableShadows;
    sampler2D shadowMap;
    float shadowVarianceBias;
    float lightBleedReduction;
    float shadowWarpFactor;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float max_range;
};

struct SpotLight {
    vec3 position;
    vec3 color;
    vec3 direction;
    float max_range;
    float outer_cone_angle;
    float inner_cone_angle;
};

//constants
const float PI = 3.14159265359;

//Uniforms
// material uniforms
uniform Material u_material;
uniform vec3 u_fallbackColor;
// global uniforms
//  matrices for transforming stuff into view space
uniform mat4 u_model_matrix;
uniform mat4 u_view_matrix;
//light uniforms
uniform DirLight u_directionalLights[MAX_DIR_LIGHTS];
uniform int u_dirLightCount;
uniform PointLight u_pointLights[MAX_POINT_LIGHTS];
uniform int u_pointLightCount;
uniform SpotLight u_spotLights[MAX_SPOT_LIGHTS];
uniform int u_spotLightCount;
uniform vec3 u_ambientLight;
uniform float u_toneMappingExposure;

//setup helpers
vec3 normalFromNormalMap(vec3 texData, mat3 TBN)
{
    vec3 n = texData * 2.0f - 1.0f;
    n = mix(vertexdat.normal, normalize(TBN * n), float(length(texData) > 1.e-6f));
    return n;
}

float lstep(float _min, float _max, float v)
{
    return clamp((v - _min) / (_max - _min), 0.0, 1.0);
}

float reduceLightBleeding(float p_max, float amount)
{
    // Remove the [0, Amount] tail and linearly rescale (Amount, 1].
    return lstep(amount, 1.0, p_max);
}

//vsm shadow sampling
float calcShadowFactor(int i)
{
    vec3 fragDepth = vertexdat.posLightSpace[i].xyz / vertexdat.posLightSpace[i].w;
    fragDepth = fragDepth * 0.5 + 0.5;
    vec4 ssamp = texture(u_directionalLights[i].shadowMap, fragDepth.xy);
    float lightDepth1M1 = ssamp.r;
    float lightDepth1M2 = ssamp.g;
    float lightDepth2M1 = ssamp.b;
    float lightDepth2M2 = ssamp.a;

    float t1 = exp(u_directionalLights[i].shadowWarpFactor * fragDepth.z);
    float t2 = -exp(-u_directionalLights[i].shadowWarpFactor * fragDepth.z);
    
    float u = lightDepth1M1;
    float o2 = lightDepth1M2 - (lightDepth1M1 * lightDepth1M1);//, 0.001);
    o2 = max(o2, u_directionalLights[i].shadowVarianceBias);
    

    float res1 = o2 / (o2 + (t1 - u) * (t1 - u));

    u = lightDepth2M1;
    o2 = lightDepth2M2 - (lightDepth2M1 * lightDepth2M1);//, 0.001);
    o2 = max(o2, u_directionalLights[i].shadowVarianceBias);

    float res2 = o2 / (o2 + (t2 - u) * (t2 - u));

    float res = min(max(res1, float(t1 <= lightDepth1M1)), max(res2, float(t2 <= lightDepth2M1)));

    res = reduceLightBleeding(res, u_directionalLights[i].lightBleedReduction);
    return res;
}

//light radiance calculation
vec3 calcDirLightRadiance(int i)
{
    //incorrect, but we assume for now that dirlights are not attenuated    
    return u_directionalLights[i].color * (u_directionalLights[i].enableShadows ? calcShadowFactor(i) : 1.0f);
}

vec3 calcPointLightRadiance(int i, vec3 fPos)
{
    float d = length(u_pointLights[i].position - fPos);
    float att = 1.0f / (1.0f + (d * d)); //probably needs fix
    return u_pointLights[i].color * att;
}

vec3 calcSpotLightRadiance(int i, vec3 fPos)
{
    vec3 dv = u_spotLights[i].position - fPos;
    float d = length(dv);
    float att = 1.0f / (1.0f + (d * d)); //probably needs fix
    //spot light cone stuff
    float cost = dot(normalize(u_spotLights[i].direction), normalize(-dv));
    float cosi = cos(u_spotLights[i].inner_cone_angle);
    float coso = cos(u_spotLights[i].outer_cone_angle);
    att *= clamp((cost - coso) / (cosi - coso), 0.0f, 1.0f);
    return u_spotLights[i].color * att;
}

//pbr helpers (PBR Stuff as explained here: https://learnopengl.com/PBR/Lighting)

//normal distribution function

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

//geometry function / microscopic self shadowing

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0; //needs to be touched when doing ibl!

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

//fresnel function
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

//cook-torrance brdf
vec3 cookTorranceBRDF(vec3 n, vec3 v, vec3 l, float roughness, vec3 reflectance, float metalness, vec3 albedo)
{
    vec3 h = normalize(v + l);

    float D = DistributionGGX(n, h, roughness);  //normal distribution 
    float G   = GeometrySmith(n, v, l, roughness);      
    vec3 F = fresnelSchlick(max(dot(h, v), 0.0), reflectance);

    vec3 nominator    = D * G * F; 
    float denominator = 4 * max(dot(n, v), 0.0) * max(dot(n, l), 0.0) + 0.001; // 0.001 to prevent divide by zero.
    vec3 specular = nominator / denominator;
    
    // kS is equal to Fresnel
    vec3 kS = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - kS;
    // multiply kD by the inverse metalness such that only non-metals 
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metalness;	  

    // scale light by NdotL
    float NdotL = max(dot(n, l), 0.0);        

    // add to outgoing radiance Lo
    return (kD * albedo / PI + specular) * NdotL; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
}


void main()
{
    // calcShadowFactor();
    // return;
    //sample material textures
    vec4 ts_albedo = texture(u_material.albedo.tex, vertexdat.uv * u_material.albedo.scale + u_material.albedo.offset);
    vec4 ts_mrar = texture(u_material.mrar.tex, vertexdat.uv * u_material.mrar.scale + u_material.mrar.offset);
    vec3 ts_normal = texture(u_material.normal.tex, vertexdat.uv * u_material.normal.scale + u_material.normal.offset).rgb;
    vec4 ts_emissive = texture(u_material.emissive.tex, vertexdat.uv * u_material.emissive.scale + u_material.emissive.offset);

    //split material data
    vec3 mt_albedo = pow(ts_albedo.rgb, vec3(2.2));;
    float mt_alpha = ts_albedo.a;
    float mt_metalness = ts_mrar.r;
    float mt_roughness = ts_mrar.g;
    float mt_ao = ts_mrar.b;
    vec3 mt_insulatorReflectanceNI = ts_mrar.aaa;
    vec3 mt_normal = normalFromNormalMap(ts_normal, vertexdat.TBN);
    vec3 mt_emissive = ts_emissive.rgb;

	//prepare vectors
    vec3 N = mt_normal * (float(gl_FrontFacing) * 2.0f - 1.0f);
    vec3 V = normalize(-vertexdat.pos);
    vec3 P = vertexdat.pos;

    //reflectance at normal incidence (insulator value from texture)
    vec3 F0 = mix(mt_insulatorReflectanceNI, mt_albedo, mt_metalness);
    
    //outgoing irradiance in view direction, from reflectance equation approximation
    vec3 Lo = vec3(0.0f);

    //accumulate all the light

    //directional lights
    for(int i = 0; i < u_dirLightCount; ++i)
    {
        vec3 radiance = calcDirLightRadiance(i);
        //vectors
        vec3 L = normalize(-u_directionalLights[i].direction);
        //Outgoing irradiance for this light
        Lo += cookTorranceBRDF(N, V, L, mt_roughness, F0, mt_metalness, mt_albedo) * radiance;
    }
    //point lights
    for(int i = 0; i < u_pointLightCount; ++i)
    {
        vec3 radiance = calcPointLightRadiance(i, P);

        //vectors
        vec3 L = normalize(u_pointLights[i].position - P);
        //Outgoing irradiance for this light
        Lo += cookTorranceBRDF(N, V, L, mt_roughness, F0, mt_metalness, mt_albedo) * radiance;
    }
    //spot lights
    for(int i = 0; i < u_spotLightCount; ++i)
    {
        vec3 radiance = calcSpotLightRadiance(i, P);
        //vectors
        vec3 L = normalize(u_spotLights[i].position - P);
        //Outgoing irradiance for this light
        Lo += cookTorranceBRDF(N, V, L, mt_roughness, F0, mt_metalness, mt_albedo) * radiance;
    }

    //do ibl later
    vec3 ambient = u_ambientLight * mt_albedo * mt_ao;

    vec3 outcol = Lo + ambient + mt_emissive;

    //gamma correction, hdr stuff
    // HDR tonemapping
    //rheinhard operator
    //outcol = outcol / (outcol + vec3(1.0));
    outcol = vec3(1.0) - exp(-outcol * u_toneMappingExposure);
    // gamma correct
    outcol = pow(outcol, vec3(1.0/2.2));// * (u_enableShadows ? calcShadowFactor() : 1.0f);

    color = vec4(outcol, 1.0f);
}