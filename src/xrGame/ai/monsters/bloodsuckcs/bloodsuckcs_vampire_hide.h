#pragma once
#include "../state.h"

template<typename _Object>
class CStateBloodsuckcsVampireHide : public CState<_Object> {
	typedef CState<_Object>		inherited;
	typedef CState<_Object>*	state_ptr;

public:
						CStateBloodsuckcsVampireHide	(_Object *obj);

	virtual	void		reselect_state					();
	virtual void		setup_substates					();
	virtual bool		check_completion				();
	virtual void		remove_links					(IGObj* object) { inherited::remove_links(object);}
};

#include "bloodsuckcs_vampire_hide_inline.h"
