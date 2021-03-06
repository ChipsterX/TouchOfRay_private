#include "common.h"
#include "ps_fxaa.hlsl"

uniform sampler2D s_base0;
uniform float4		screen_res;	

struct	v2p 
{
	float2 	tc0	: TEXCOORD0;
	float4 	HPos	: POSITION;	 
};

float4 main (  v2p I ) : COLOR0
{
	return FxaaPixelShader(I.tc0,
            FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),		// FxaaFloat4 fxaaConsolePosPos,
            s_base0,							// FxaaTex tex,
            s_base0,							// FxaaTex fxaaConsole360TexExpBiasNegOne,
            s_base0,							// FxaaTex fxaaConsole360TexExpBiasNegTwo,
            screen_res.zw,							// FxaaFloat2 fxaaQualityRcpFrame,
            FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),		// FxaaFloat4 fxaaConsoleRcpFrameOpt,
            FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),		// FxaaFloat4 fxaaConsoleRcpFrameOpt2,
            FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),		// FxaaFloat4 fxaaConsole360RcpFrameOpt2,
            0.35f,									// FxaaFloat fxaaQualitySubpix,
            0.125f,									// FxaaFloat fxaaQualityEdgeThreshold,
            0.0f,//0.0625f,								// FxaaFloat fxaaQualityEdgeThresholdMin,
            0.0f,									// FxaaFloat fxaaConsoleEdgeSharpness,
            0.0f,									// FxaaFloat fxaaConsoleEdgeThreshold,
            0.0f,									// FxaaFloat fxaaConsoleEdgeThresholdMin,
            FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f)		// FxaaFloat fxaaConsole360ConstDir,
            );
	
}
