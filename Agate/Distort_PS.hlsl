struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
    float2 uv : TEXCOORD0;
};
//https://zhuanlan.zhihu.com/p/86237202

sampler sampler0;
sampler sampler1;

Texture2D texture0 : register(t0);
Texture2D texture1 : register(t1);
            
float4 main(PS_INPUT input) : SV_Target
{
    float2 disp = texture1.Sample(sampler1, input.uv).rg;
    disp = ((disp * 2) - 1) * 1.2f;

    float4 out_col = input.col * texture0.Sample(sampler0, disp);
    return out_col;
}