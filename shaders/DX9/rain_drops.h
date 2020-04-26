#ifndef        GASMASK_COMMON_H
#define        GASMASK_COMMON_H
//////////////////////////////////////////////////////////////////////////////////////////
//
struct		GasmaskStruct
{
	float2		tc0		:		TEXCOORD0;
	float2		tc1		:		TEXCOORD1;
	float2		tc2		:		TEXCOORD2;
	float2		tc3		:		TEXCOORD3;
	float2		tc4		:		TEXCOORD4;
	float4		tc5		:		TEXCOORD5;
	float4		tc6		:		TEXCOORD6;
};

//////////////////////////////////////////////////////////////////////////////////////////
//samplers

uniform sampler2D s_mask_1;
uniform sampler2D s_mask_2;
uniform sampler2D s_mask_3;
uniform sampler2D s_mask_4;
uniform sampler2D s_mask_5;
uniform sampler2D s_mask_6;
uniform sampler2D s_mask_7;
uniform sampler2D s_mask_8;
uniform sampler2D s_mask_9;
uniform sampler2D s_mask_10;

uniform sampler2D s_mask_v_1;
uniform sampler2D s_mask_v_2;
uniform sampler2D s_mask_v_3;

uniform sampler2D s_mask_nm_1;
uniform sampler2D s_mask_nm_2;
uniform sampler2D s_mask_nm_3;
uniform sampler2D s_mask_nm_4;
uniform sampler2D s_mask_nm_5;
uniform sampler2D s_mask_nm_6;
uniform sampler2D s_mask_nm_7;
uniform sampler2D s_mask_nm_8;
uniform sampler2D s_mask_nm_9;
uniform sampler2D s_mask_nm_10;

//////////////////////////////////////////////////////////////////////////////////////////
//uniforms
uniform float3 mask_control; 
uniform float3 drops_control;

//////////////////////////////////////////////////////////////////////////////////////////
//defines
#define CRACKS_INTENSITY 0.25
#define VIGNETTE_INTENSITY 0.5
////////////////////////////
#define DUDV_DISTORT float(0.025f)
///////////////////////////
#define DROPS_TURBSIZE 15.f
#define DROPS_TURBSHIFT float4(0.35, 1, 0, 1)
#define DROPS_TURBTIME sin(0.1/3.f)
#define DROPS_TURBCOF 0.33
///////////////////////////
#define VIS									// Enables/disables the visor reflection effect
#define VIS_XFACTOR float(2.0)
#define VIS_YFACTOR float(1.3)
#define VIS_CRUNCHINTENSITY float (0.25)	// Intensity of the peripheral/radial crunch effect in the visor
#define VIS_MIRRORINTENSITY float(0.15)		// Intensity of the mirroring effect in the visor
#define VIS_RADIALCOEFF float(1.0)			// Increment power of radial effect towards periphery (1.0 = linearly incrementing)
#define VIS_BREAK float(1.4)				// Was 0.9
#define VIS_MIRRORMINDIST float (2.50)		// Objects nearer than this values won't be mirror. Use 2.5 to prevent the player's weapon to be mirrored too much.
#define VIS_SCREENRATIO float(0.625)		// Screen Ratio; use 0.75 for 4:3 (normalscreen) and 0.625 for 16:10 (widescreen) resolution
#define VIS_SATURATION float(1.0)			// Level of color saturation before applying visor mirror color (if other than (1,1,1)). 0 is fully desaturated, 1 is original color
#define VIS_SAT_SUN_COEF float(0.01)			// How much influence sun lighting has on the visor mirror color saturation.
#define VIS_CONTRAST float(1.0)				// Contrast ratio of mirrored color (1.0 = normal, 0.0 = none).
#define VIS_COLOR float4(1,1,1,1)			// Color filter applied to visor ("sunglass effect") (RGBA)
#define VIS_COLORCOEFF float(2.0)			// Power of color filter of mirrored object (1.0 = linear, 2.0 = square)

// visor border settings
#define VIS_CENTER float2(0.0,0.0)		// Y-axix center of border black out.
#define VIS_BLACKASPECT float2(1.5,0.1)		// Aspect ratio of border black out.
#define VIS_BLACKRADIUSMIN float2(0,0)	// Screen radius at which border black out begins.
#define VIS_BLACKRADIUSMAX float2(0,0)	// Screen radius at which border black out is complete.

///////////////////////////
//#define ALPHA_MASKING
#endif
