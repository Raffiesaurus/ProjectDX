Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);


cbuffer LightBuffer : register(b0)
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightPosition;
    float padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 position3D : TEXCOORD2;
};

float4 main(InputType input) : SV_TARGET
{
    float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    float4 color;

    lightDir = normalize(input.position3D - lightPosition);

    lightIntensity = saturate(dot(input.normal, -lightDir));

    color = ambientColor + (diffuseColor * lightIntensity); //adding ambient
    color = saturate(color);

    textureColor = shaderTexture.Sample(SampleType, input.tex);
    color = color * textureColor;

    return color;
}

