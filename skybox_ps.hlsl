
TextureCube cubeTex : register(t0);
SamplerState samplers : register(s0);

struct InputType
{
    float4 position : SV_POSITION;
    float3 direction : TEXCOORD0;
};


float4 main(InputType input) : SV_TARGET
{
    return cubeTex.Sample(samplers, input.direction);
}