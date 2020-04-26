/*
	Screen-Space Directional Occlusion.
	Based on Tobias Ritschel tech paper.
	Implemented by K.D. (OGSE team)
	Last Changed: 23.02.20 - LVutner
*/

#ifndef SSDO_H
#define SSDO_H
#include "common.h"
#include "ogse_functions.h"

#define SSDO_RADIUS float(0.04)					// радиус семплирования в игровых метрах. Чем выше, тем более заметен эффект, но тем больше нежелательных наложений теней
#define SSDO_DISCARD_THRESHOLD float(1.0)		// максимальная разница в глубине пикселей, при которой пиксель еще учитывается в расчете затенения. Уменьшение убирает "шлейф" в некоторых случаях.
#define SSDO_COLOR_BLEEDING float(15.0)			// сила цвета семплов. Дает более цветные тени, но уменьшает интенсивность эффекта в целом. Для компенсации увеличивайте SSDO_BLEND_FACTOR.
#define SSDO_BLEND_FACTOR float(1.500)
#define GRASS_TUNING float(1.5)					// чем больше, тем меньше затеняется трава


static const float3 arr[7] = 
{
	float3(	0.8113	,	0.0000	,	-0.4	),
	float3(	0.3183	,	0.6913	,	-0.3	),
	float3(	-0.2074	,	0.5981	,	-0.5	),
	float3(	-0.5497	,	0.3373	,	-0.5	),
	float3(	-0.8652	,	-0.4010	,	-1.0	),
	float3(	-0.1994	,	-0.5347	,	-0.5	),
	float3(	0.4934	,	-0.5759	,	-0.8	),
};

float get_depth_fast(float2 tc)	
{
	return tex2Dlod(s_position, float4(tc,0,0)).z;
}

#ifndef AO_QUALITY
float3 calc_ssdo (float3 P, float3 N, float2 tc)
{
	return 1.0h;
}
#else // AO_QUALITY		 
float3 calc_ssdo (float3 P, float3 N, float2 tc)
{
	int quality = AO_QUALITY + 1;
	float3 occ = float3(0,0,0);
	float scale = P.z/10.f * SSDO_RADIUS;
	for (int a = 1; a < quality; ++a)
	{
		scale *= a;
		for (int i = 0; i < 7; i++)
		{
			float3 occ_pos_view = P.xyz + (arr[i] + N) * scale;
			float4 occ_pos_screen = proj_to_screen(mul(m_P, float4(occ_pos_view, 1.0)));
			occ_pos_screen.xy /= occ_pos_screen.w;
			float screen_occ = get_depth_fast(occ_pos_screen);	
			screen_occ = lerp(screen_occ, 0.f, is_sky(screen_occ));
			float is_occluder = step(occ_pos_view.z, screen_occ);
			float occ_coeff = saturate(is_occluder + saturate(2.1 - screen_occ) + step(SSDO_DISCARD_THRESHOLD, abs(P.z-screen_occ)));
			occ += float3(occ_coeff, occ_coeff, occ_coeff);
		}
	}
	occ /= (7 * AO_QUALITY);
	
	float grass_tuning = 1 + GRASS_TUNING*is_in_range(float3(P.z, 0.360, 0.365));
	occ = saturate(occ*grass_tuning);
	
	return (occ + (1 - occ)*(1 - SSDO_BLEND_FACTOR));
}
#endif

#endif

