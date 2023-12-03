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
    
    //color = float4(color.r, color.g, color.b, 1.0f); 
    float4 originalColor = color;
    color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    //Box
    if (input.position3D.x > 7.9 || input.position3D.x < -7.9 || input.position3D.z > 14.9 || input.position3D.z < -14.9)
    {
        color = float4(originalColor.r, originalColor.g, originalColor.b, 1.0f);
    }
    
    // Middle
    if (input.position3D.z > -0.05 && input.position3D.z < 0.05)
    {
        color = float4(originalColor.r, originalColor.g, originalColor.b, 1.0f);
    }
    
    // Middle Circle
    if (input.position3D.z > -1.5 && input.position3D.z < 1.5)
    {
        float len = length(float2(input.position3D.x, input.position3D.z));
        if (len < 1.35 && len > 1.25)
        {
            color = float4(originalColor.r, originalColor.g, originalColor.b, 1.0f);
        }
    }
    
    //Front Pen Box
    if (input.position3D.x > -4.4 && input.position3D.x < 5.1)
    {
        
        if ((input.position3D.z > -14.9 && input.position3D.z < -10))
        {
            // Left And Right Line
            if ((input.position3D.x > -4.4 && input.position3D.x < -4.3) || (input.position3D.x < 5.1 && input.position3D.x > 5.0))
            {
                color = float4(originalColor.r, originalColor.g, originalColor.b, 1.0f);
            }
            
            //Keeper Left and Right line
            if ((input.position3D.x > -3.4 && input.position3D.x < -3.3) || (input.position3D.x < 4.1 && input.position3D.x > 4.0))
            {
                if (input.position3D.z > -14.9 && input.position3D.z < -13)
                {
                    color = float4(originalColor.r, originalColor.g, originalColor.b, 1.0f);
                }
                
            }

            //Keeper Horizontal                    
            if ((input.position3D.z > -13 && input.position3D.z < -12.9))
            {
                if ((input.position3D.x > -3.4 && input.position3D.x < 4.1))
                {                    
                    color = float4(originalColor.r, originalColor.g, originalColor.b, 1.0f);
                }
            }
           
            //Pen Box Horizontal
            if (input.position3D.z < -10 && input.position3D.z > -10.1)
            {
                color = float4(originalColor.r, originalColor.g, originalColor.b, 1.0f);
            }
        }

    }

    //Back Pen Box
    if (input.position3D.x > -4.4 && input.position3D.x < 5.1)
    {
        
        if ((input.position3D.z < 14.9 && input.position3D.z > 10))
        {
            // Left And Right Line
            if ((input.position3D.x > -4.4 && input.position3D.x < -4.3) || (input.position3D.x < 5.1 && input.position3D.x > 5.0))
            {
                color = float4(originalColor.r, originalColor.g, originalColor.b, 1.0f);
            }
            
            //Keeper Left and Right line
            if ((input.position3D.x > -3.4 && input.position3D.x < -3.3) || (input.position3D.x < 4.1 && input.position3D.x > 4.0))
            {
                if (input.position3D.z < 14.9 && input.position3D.z > 13)
                {
                    color = float4(originalColor.r, originalColor.g, originalColor.b, 1.0f);
                }
                
            }

            //Keeper Horizontal                    
            if ((input.position3D.z < 13 && input.position3D.z > 12.9))
            {
                if ((input.position3D.x > -3.4 && input.position3D.x < 4.1))
                {
                    color = float4(originalColor.r, originalColor.g, originalColor.b, 1.0f);
                }
            }
           
            //Pen Box Horizontal
            if (input.position3D.z > 10 && input.position3D.z < 10.1)
            {
                color = float4(originalColor.r, originalColor.g, originalColor.b, 1.0f);
            }
        }
    }
    
    /*if (input.position3D.z > 10 && input.position3D.z < 10.1)
    {
        float len = length(float2(input.position3D.x, input.position3D.z));
        if (len > 1)
        {
            color = float4(originalColor.r, originalColor.g, originalColor.b, 1.0f);
        }
    }*/
    
    return color;
}

