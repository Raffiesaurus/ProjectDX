// colour vertex shader
// Simple geometry pass
// texture coordinates and normals will be ignored.

cbuffer MatrixBuffer : register(b0) {
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct InputType {
    float4 position : POSITION;
};

struct OutputType {
    float4 position : SV_POSITION;
    float3 direction : TEXCOORD0;
};

OutputType main(InputType input) {
    OutputType output;
	
    input.position.w = 1.0f;

    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    output.direction = normalize(input.position);

    return output;
}