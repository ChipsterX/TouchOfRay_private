#pragma once

class CBlender_gamma : public IBlender
{
public:
	virtual		LPCSTR		getComment() { return "gamma"; }
	virtual		BOOL		canBeDetailed() { return FALSE; }
	virtual		BOOL		canBeLMAPped() { return FALSE; }

	virtual		void		Compile(CBlender_Compile& C);

	CBlender_gamma();
	virtual ~CBlender_gamma();
};
