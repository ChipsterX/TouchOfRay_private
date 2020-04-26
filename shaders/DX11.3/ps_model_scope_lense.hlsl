#include "common.h"
	
struct 	v2p
{
 	float2 	tc0: 		TEXCOORD0;	// base
 	float3 	tc1: 		TEXCOORD1;	// environment
  	float4	c0:			COLOR0;		// sun.(fog*fog)
};

//////////////////////////////////////////////////////////////////////////////////////////

// Pixel
uniform	float4		screen_res;

#include "pnv.h"

Texture2D 	s_vp2;

float resize(float input, float factor, float offset)
{
	return (input - 0.5f + offset) / factor + 0.5f - offset;
}

float4 main( v2p I ) : SV_Target
{
	float4 t_base = s_base.Sample(smp_base, I.tc0); // �������� �����	

	// ����������� �������� � ����� ���, ����� �� ����� ���������� ������ ���� ���������� ���������
	I.tc0.x = resize(I.tc0.x, screen_res.x / screen_res.y, 0);
	float4 t_vp2 = s_vp2.Sample(smp_base, I.tc0); // ����������� �� ������� ��������

	if (!isSecondVPActive()) {
		t_vp2.rgb /= 100; //KRodin: ����� ����������� �� ������ �������� �� ��������� ����� ��� ���������� - ������ �������� ��� � 100 ���.
	} else if (m_blender_mode.x == 1.f) { //������� ���
		t_vp2.rgb = calc_night_vision_effect(I.tc0, t_vp2, float3(0.66, 2.0, 0.5));
	} else if (m_blender_mode.x == 2.f) { //�������� ����������, �� ����� ���� ��� � ������� ��������
		t_vp2.rgb = calc_night_vision_effect(I.tc0, t_vp2, float3(0.79, 0.191, 0.247));
	}

	// ������� �� � �������� ��������� ��������
	float3 final = lerp(t_vp2, t_base, t_base.a); // ����� � ���������

	// out
	return float4(final.r, final.g, final.b, m_hud_params.x);
}