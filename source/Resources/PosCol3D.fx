float4x4 gWorldViewProj : WorldViewProjection;
float4x4 gWorldMatrix : World;
float3 gOnb : ViewInverse;

Texture2D gDiffuseMap : DiffuseMap;
Texture2D gGlossyMap : GlossyMap;
Texture2D gSpecularMap : SpecularMap;
Texture2D gNormalMap : NormalMap;

float3 lightDirection = float3(0.577f, -0.577f, 0.5777f);
float diffuseReflectance = 7.f; //kd
float shininess = 25.f;
float PI = 3.14;

//SamplerState gState : SV_SampleIndex;

SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState samPointLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState samPointAni
{
    Filter = ANISOTROPIC;
    AddressU = Wrap;
    AddressV = Wrap;
};

//
// Input/Output Structs
//

struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Color : COLOR;
    float2 Uv : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR0;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

 float3 Phong(float exp, float3 light, float3 viewdir, float3 normal, float3 ks)
{
	float3 reflect = light - 2 * (dot(normal,light) * normal);
	const float cosa = dot(reflect, viewdir);
	return ks * pow(cosa, exp);
}

float3 Lambert(float3 color)
{
    return (color * diffuseReflectance) / PI;
}

//
// Vertex Shader
//
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
    output.Color = float4(input.Color, 1.f);
    output.Normal = mul(normalize(input.Normal), (float3x3) gWorldMatrix);
    output.Tangent = mul(normalize(input.Tangent), (float3x3) gWorldMatrix);
    output.Uv = input.Uv;
    return output;
}

//
//Pixel Shader
//
float4 PS1(VS_OUTPUT input) : SV_TARGET
{    
    //maps
    float4 gloss = gGlossyMap.Sample(samPoint, input.Uv) * input.Color;
    float4 spec = gSpecularMap.Sample(samPoint, input.Uv);
    float4 color = gDiffuseMap.Sample(samPoint, input.Uv);
    float4 normals = gNormalMap.Sample(samPoint, input.Uv);
    
    float3 viewDir = (input.Position.rgb - gOnb);
    
    //Lambert cosine law (is correct)
    float3 computedNormals = normalize(input.Normal + normals.rgb);
    float observedArea = saturate(dot(computedNormals, -lightDirection));
    
    //diffuse (is correct)
    float diffuseAmount = dot(-lightDirection, computedNormals);
    float3 diffuse = diffuseAmount * Lambert(color.rgb);
  
    //specular
    float specular = Phong(gloss.x * shininess, lightDirection, -viewDir, input.Normal, spec.rgb);
    
    //phong
    float3 phong = diffuse + specular;

    float4 output;
    output.rgb = observedArea;
    //output.rgb = diffuse * observedArea;
    //output.rgb = phong * observedArea;
    output.a = 1;
    return output;
}

float4 PS2(VS_OUTPUT input) : SV_TARGET
{
    //maps
    float4 gloss = gGlossyMap.Sample(samPointLinear, input.Uv) * input.Color;
    float4 spec = gSpecularMap.Sample(samPointLinear, input.Uv);
    float4 color = gDiffuseMap.Sample(samPointLinear, input.Uv);
    float4 normals = gNormalMap.Sample(samPointLinear, input.Uv);
    
    float3 viewDir = (input.Position.rgb - gOnb);
    
    //Lambert cosine law (is correct)
    float3 computedNormals = normalize(input.Normal + normals.rgb);
    float observedArea = saturate(dot(computedNormals, -lightDirection));
    
    //diffuse (is correct)
    float diffuseAmount = dot(-lightDirection, computedNormals);
    float3 diffuse = diffuseAmount * Lambert(color.rgb);
  
    //specular
    float specular = Phong(gloss.x * shininess, lightDirection, -viewDir, input.Normal, spec.rgb);
    
    //phong
    float3 phong = diffuse + specular;

    float4 output;
    //output.rgb = observedArea;
    output.rgb = diffuse * observedArea;
    //output.rgb = phong * observedArea;
    output.a = 1;
    return output;
}

float4 PS3(VS_OUTPUT input) : SV_TARGET
{
    //maps
    float4 gloss = gGlossyMap.Sample(samPointAni, input.Uv) * input.Color;
    float4 spec = gSpecularMap.Sample(samPointAni, input.Uv);
    float4 color = gDiffuseMap.Sample(samPointAni, input.Uv);
    float4 normals = gNormalMap.Sample(samPointAni, input.Uv);
    
    float3 viewDir = (input.Position.rgb - gOnb);
    
    //Lambert cosine law (is correct)
    float3 computedNormals = normalize(input.Normal + normals.rgb);
    float observedArea = saturate(dot(computedNormals, -lightDirection));
    
    //diffuse (is correct)
    float diffuseAmount = dot(-lightDirection, computedNormals);
    float3 diffuse = diffuseAmount * Lambert(color.rgb);
  
    //specular
    float specular = Phong(gloss.x * shininess, lightDirection, -viewDir, input.Normal, spec.rgb);
    
    //phong
    float3 phong = diffuse + specular;

    float4 output;
    //output.rgb = observedArea;
    //output.rgb = diffuse * observedArea;
    output.rgb = phong * observedArea;
    output.a = 1;
    return output;
}



//
//Technique
//
technique11 DefaultTechnique
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS1()));
    }
}

technique11 LinearTechnique
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS2()));
    }
}

technique11 AniTechnique
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS3()));
    }
}