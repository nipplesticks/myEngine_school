struct GS_IN
{
    float4 position : SV_POSITION;
    float2 Tex : TEXCOORD;
};

struct GS_OUT
{
    float4 position : SV_POSITION;
    float2 Tex : TEXCOORD;
};


[maxvertexcount(6)]
void main(triangle GS_IN input[3], inout TriangleStream<GS_OUT> fragmentShaderInput)
{

}