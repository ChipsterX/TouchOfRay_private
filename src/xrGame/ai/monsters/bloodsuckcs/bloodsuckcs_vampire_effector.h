#pragma once

#include "../../../../xrEngine/effectorPP.h"
#include "../../../CameraEffector.h"
#include "../../../../xrEngine/cameramanager.h"

class CVampirePPEffectcs : public CEffectorPP {
	typedef CEffectorPP inherited;	

	SPPInfo state;			//current state
	float	m_total;		// total PP time

public:
					CVampirePPEffectcs		(const SPPInfo &ppi, float life_time);
	virtual	BOOL	Process					(SPPInfo& pp);
};

//////////////////////////////////////////////////////////////////////////
// Vampire Camera Effector
//////////////////////////////////////////////////////////////////////////
class CVampireCameraEffectcs : public CEffectorCam {
	typedef CEffectorCam inherited;	

	float	m_time_total;
	Fvector	dangle_target;
	Fvector dangle_current;

	float	m_dist;
	Fvector m_direction;

public:
					CVampireCameraEffectcs	(float time, const Fvector &src, const Fvector &tgt);
	virtual BOOL	ProcessCam				(SCamEffectorInfo& info);
};



