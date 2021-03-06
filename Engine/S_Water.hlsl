struct DirectionalLight
{
    float3 direction;
    float4 diffuse;
    float4 ambient;
    float intensity;
};
struct PointLight
{
    float3 position;
    float4 diffuse;
    float intensity;
    float radius;
};
struct Parameters
{
    float4 diffuse;
    float roughness;
    float metallic;
    float opacity;
};

cbuffer cbMatrix : register(b0)
{
    float4x4 WVP;
    float4x4 World;
    float3 WCP;
};
cbuffer cbLighting : register(b1)
{
    DirectionalLight dirLight;
    PointLight pointLight;
};
cbuffer cbParameter : register(b3)
{
    float time;
    Parameters params;
};

struct appdata
{
    float3 vertex : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};
struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float3 worldPos : POSITION;
    float3 camPos : POSITION1;
    float3 vPos : POSITION2;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float colH : POSITION3;
};

float CalculateFallOff(float _radius, float3 _lightDir)
{
    float fallOff = max(0, _radius - length(_lightDir)); //calculating the fallOff acording to the radius of the light

    //When no radius is applied, then calculate without any radius
    if (_radius < 0)
        fallOff = 1;

    
    return fallOff;
}
float4 CalculateDiffuse(float3 _normal, float3 _lightDir, float4 _diffuse, float _intensity, float _radius = -1)
{
    float fallOff = CalculateFallOff(_radius, _lightDir);

    float d = saturate(dot(_normal, normalize(_lightDir)) * fallOff); //calculating the dot product of the lightDir and the surface normal with fallOff


    return float4(d * _diffuse * _intensity);
}
float4 CalculateSpecular(float3 _normal, float3 _viewDir, float3 _lightDir, float4 _diffuse, float _intensity, float _radius = -1)
{
    float3 viewDir = normalize(_viewDir);
    float3 lightDir = normalize(_lightDir);

    float3 reflectedLightDir = normalize(reflect(lightDir, _normal));
    float3 halfVec = normalize(viewDir + lightDir); //the half Vector between the view Dir and the reflected light
    float fallOff = CalculateFallOff(_radius, _lightDir);

    float d = saturate(dot(lightDir, _normal) * fallOff); //calculating the dot product of the lightDir and the surface normal with fallOff
    float d2 = dot(-reflectedLightDir, viewDir); //calculating the area hit by the specular light
    float d3 = saturate(dot(_normal, viewDir)); //calculating the fresnel

    //Specular
    d2 = fallOff *
         (1 - params.roughness) * //calculating the specular according to roughness 1 => no specular
         (1 + params.metallic * 2) * //calculating the factor of the specular according to the metalic 1 => factor 9
         pow(d2, //calculating the power of the specular to make it the step smoother or more harsh
             params.metallic * 1000 + //makes the spec sharper with metalic
             90 - (70 * (params.roughness))) - //base 90 to -70 roughness 1 => power 20 (smoother)
             (params.metallic * 0.5); //makes the spec smaller with metalic

    //Fresnel
    d3 = fallOff *
         (1 - params.roughness) * //with roughness no fresnel
         saturate(1 - pow(d3,
                          0.5 + //Base fresnel Power
                          0.5 * params.metallic * (1 - d3))); //additional fresnel power with metallic

    
    return float4(saturate(d * (max(d2, (d3 * 0.75)))) * _diffuse * _intensity);
}

float rand(float2 n)
{
    return frac(sin(dot(n, float2(12.9898, 4.1414))) * 43758.5453);
}
float noise(float2 p)
{
    float2 ip = floor(p);
    float2 u = frac(p);
    u = u * u * (3.0 - 2.0 * u);
	
    float res = lerp(
		lerp(rand(ip), rand(ip + float2(1.0, 0.0)), u.x),
		lerp(rand(ip + float2(0.0, 1.0)), rand(ip + float2(1.0, 1.0)), u.x), u.y);
    return res * res;
}

Texture2D ObjTexture : register(t0);
Texture2D ObjHeight : register(t2);
SamplerState ObjSamplerState : register(s0);

VS_OUTPUT VS(appdata v)
{
    VS_OUTPUT o;
    
    float displacement = ObjHeight.SampleLevel(ObjSamplerState, v.uv, 0).r;
    v.vertex += (1 - displacement) * float3(
        sin((cos(v.vertex.x * 0.2 + time) + v.vertex.x * 0.2) + time),
        pow(noise(float2((v.vertex.x * 0.2 + time), (v.vertex.z * 0.2 + time))) * 3, 1) + sin((v.vertex.x * 0.2 + v.vertex.z * 0.2) + time),
        sin((cos(v.vertex.z * 0.2 - time) + v.vertex.z * 0.2) - time));

    v.vertex -= float3(0, 1, 0) * displacement * 25;

    o.pos = mul(WVP, float4(v.vertex, 1));
    o.normal = mul(World, float4(v.normal, 0));
    o.worldPos = mul(World, float4(v.vertex, 1));
    o.camPos = WCP;
    o.uv = float2(v.uv.x - time * 0.005, v.uv.y + time * 0.005);
    o.vPos = v.vertex;
    o.colH = displacement;
    
    return o;
}

[maxvertexcount(3)]
void GS(triangle VS_OUTPUT i[3], inout TriangleStream<VS_OUTPUT> os)
{
    VS_OUTPUT t = (VS_OUTPUT) 0;
    
    float3 normal = normalize(cross(
        i[2].worldPos.xyz - i[0].worldPos.xyz,
        i[1].worldPos.xyz - i[0].worldPos.xyz));

    for (int j = 0; j < 3; j++)
    {
        t.normal = normal;
        t.pos = i[j].pos;
        t.worldPos = i[j].worldPos;
        t.camPos = i[j].camPos;
        t.uv = i[j].uv;
        t.vPos = i[j].vPos;
        t.colH = i[j].colH;
        os.Append(t);
    }
    
    os.RestartStrip();
}

float4 PS(VS_OUTPUT i) : SV_TARGET
{
    //calculating normal
    float3 normal = normalize(i.normal);

    float tiling = 14;
    float4 col = ObjTexture.Sample(ObjSamplerState, tiling * i.uv);
    //base color for gray foam texture
    col *= float4(0.58, 0.76, 0.76, 1); 
    
    float3 base = float3(0.22, 0.32, 0.37); //(5, 30, 42)
    float3 light = float3(0.38, 0.56, 0.56); //(96, 142, 142);

    
    //calculating directionalLight
    float4 directionalLight =
        CalculateDiffuse(
            normal,
            dirLight.direction,
            dirLight.diffuse, dirLight.intensity)
        + CalculateSpecular(
            normal,
            i.worldPos - i.camPos,
            dirLight.direction,
            dirLight.diffuse, dirLight.intensity);
    
    float range = (pow(i.vPos.y, 0.25) - 1); //the foam range from the vertexPos
    float4 detail = float4(base + saturate((light * range)).rgb, 0.2 + 2 * saturate(range)); //foam detail with base col
    float alpha = 99 * pow(i.colH, 2) - 0.1; //alpha value from heightmap

    
    return (directionalLight + dirLight.ambient) * float4(col.rgb, 0.1 - alpha) + detail;
}