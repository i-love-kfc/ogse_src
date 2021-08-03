#include "shader.h"
#include "RenderDevice.h"

SConstantList::~SConstantList			()			{	Device.Resources->_DeleteConstantList	(this);			}
SPass::~SPass							()			{	Device.Resources->_DeletePass			(this);			}
ShaderElement::~ShaderElement			()			{	Device.Resources->_DeleteElement		(this);			}
SGeometry::~SGeometry					()			{	Device.Resources->DeleteGeom			(this);			}
Shader::~Shader							()			{	Device.Resources->Delete				(this);			}
R_constant_table::~R_constant_table		()			{	Device.Resources->_DeleteConstantTable	(this);			}

// predicates
IC bool	p_search	(ref_constant C, LPCSTR S)
{
	return xr_strcmp(*C->name,S)<0;
}
IC bool	p_sort		(ref_constant C1, ref_constant C2)
{
	return xr_strcmp(C1->name,C2->name)<0;
}
ref_constant R_constant_table::get	(LPCSTR S)
{
	// assumption - sorted by name
	c_table::iterator I	= std::lower_bound(table.begin(),table.end(),S,p_search);
	if (I==table.end() || (0!=xr_strcmp(*(*I)->name,S)))	return 0;
	else												return *I;
}
ref_constant R_constant_table::get	(shared_str& S)
{
	// linear search, but only ptr-compare
	c_table::iterator I	= table.begin	();
	c_table::iterator E	= table.end		();
	for (; I!=E; ++I)	{
		ref_constant	C		= *I;
		if (C->name.equal(S))	return C;
	}
	return	0;
}