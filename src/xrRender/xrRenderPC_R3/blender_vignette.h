#pragma once

class CBlender_vignette : public IBlender
{
public:
	virtual		LPCSTR		getComment() { return "vignette"; }
	virtual		BOOL		canBeDetailed() { return FALSE; }
	virtual		BOOL		canBeLMAPped() { return FALSE; }

	virtual		void		Compile(CBlender_Compile& C);

	CBlender_vignette();
	virtual ~CBlender_vignette();
};
