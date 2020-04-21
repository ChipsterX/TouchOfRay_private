#pragma once
#include "../state.h"
#include "../../../../xrServerEntities/clsid_game.h"

template<typename _Object>
class	CStateBloodsuckcsVampire : public CState<_Object> {
	typedef CState<_Object>		inherited;
	typedef CState<_Object>*	state_ptr;

	u32					m_time_last_vampire;

	const CEntityAlive *enemy;

public:
						CStateBloodsuckcsVampire		(_Object *obj);
	
	virtual void		reinit							();
	
	virtual void		initialize						();
	virtual	void		reselect_state					();
	virtual	void		finalize						();
	virtual	void		critical_finalize				();
	virtual bool		check_start_conditions			();
	virtual bool		check_completion				();
	virtual void		remove_links					(IGObj* object);

	virtual void		setup_substates					();
	virtual void		check_force_state				();
};

#include "bloodsuckcs_vampire_inline.h"
