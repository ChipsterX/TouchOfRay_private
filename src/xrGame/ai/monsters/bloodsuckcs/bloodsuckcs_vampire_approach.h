#pragma once
#include "../state.h"

template<typename _Object>
class CStateBloodsuckcsVampireApproach : public CState<_Object> {
	typedef CState<_Object> inherited;

public:
						CStateBloodsuckcsVampireApproach	(_Object *obj);
	virtual				~CStateBloodsuckcsVampireApproach	();

	virtual void		initialize							();
	virtual	void		execute								();
	virtual void		remove_links						(IGObj* object) { inherited::remove_links(object);}
};

#include "bloodsuckcs_vampire_approach_inline.h"
