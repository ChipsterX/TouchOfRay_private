////////////////////////////////////////////////////////////////////////////
//	Module 		: condition_state.h
//	Created 	: 26.02.2004
//  Modified 	: 26.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Condition state
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "xrAICore/Components/operator_condition.h"

template <typename _world_property>
class CConditionState
{
public:
    typedef _world_property COperatorCondition;

protected:
    xr_vector<COperatorCondition> m_conditions;
    u32 m_hash;

public:
    inline CConditionState();
    inline virtual ~CConditionState();
    inline const xr_vector<COperatorCondition>& conditions() const;
    inline u8 weight(const CConditionState& condition) const;
    inline void add_condition(const COperatorCondition& condition);
    inline void remove_condition(const typename COperatorCondition::_condition_type& condition);
    inline void add_condition(
        typename xr_vector<COperatorCondition>::const_iterator& J, const COperatorCondition& condition);
    inline void add_condition_back(const COperatorCondition& condition);
    inline bool includes(const CConditionState& condition) const;
    inline void clear();
    inline bool operator<(const CConditionState& condition) const;
    inline CConditionState<_world_property>& operator-=(const CConditionState& condition);
    inline bool operator==(const CConditionState& condition) const;
    inline u32 hash_value() const;
    inline const COperatorCondition* property(const typename COperatorCondition::_condition_type& condition) const;
};

#include "xrAICore/Components/condition_state_inline.h"
