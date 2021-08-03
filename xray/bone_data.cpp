#include "bone_data.h"

void CBoneData::CalculateM2B(const Fmatrix& parent)
{
	// Build matrix
	m2b_transform.mul_43	(parent,bind_transform);

	// Calculate children
	for ( size_t i = 0; i < children.size(); i++)
	{
		CBoneData* C = children[i];
		C->CalculateM2B(m2b_transform);
	}

	m2b_transform.invert	();            
}

u16	CBoneData::GetNumChildren(void)const
{
	return (u16)children.size();
}
CBoneData&	CBoneData::GetChild( u16 id )
{
	return *children[id];
}
const CBoneData&	CBoneData::GetChild(u16 id) const
{
	return *children[id];
}


