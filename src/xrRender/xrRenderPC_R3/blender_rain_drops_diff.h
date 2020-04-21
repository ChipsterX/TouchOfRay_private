#pragma once

class CBlender_rain_drops_diff : public IBlender
{
public:
	virtual		LPCSTR		getComment() { return "Rain drops_diff"; }
	virtual		BOOL		canBeDetailed() { return FALSE; }
	virtual		BOOL		canBeLMAPped() { return FALSE; }

	virtual		void		Compile(CBlender_Compile& C);

	CBlender_rain_drops_diff();
	virtual ~CBlender_rain_drops_diff();
};