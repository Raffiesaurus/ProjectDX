// Light pixel shader
// Calculate diffuse lighting for a single directional light(also texturing)

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

struct InputType {
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET {
    float4 textureColor;

    textureColor = shaderTexture.Sample(SampleType, input.tex);

    return textureColor;
}