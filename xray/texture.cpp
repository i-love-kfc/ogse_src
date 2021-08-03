#include "texture.h"

#include "RenderDevice.h"
#include "ResourceManager.h"

/*text_pt::~text_pt(void)
{
	
  CTexture *v22 = this->p_;
  if ( v22 )
  {
    ++v22->baseclass_4.baseclass_0.baseclass_0.dwReference;
    ++v22->baseclass_4.baseclass_0.baseclass_0.dwReference;
    bool v12 = v22->baseclass_4.baseclass_0.baseclass_0.dwReference-- == 1;
    if ( v12 )
    {
      void*v23 = __RTCastToVoid(v22);
      CTextureVtbl *v24 = v22->vfptr;
      void*fi = v23;
      v24->__vecDelDtor(v22, 0);
	  Memory.mem_free(fi);
    }
  }
}*/

CTexture::~CTexture()
{
	Unload				();

	// release external reference
	Device.Resources->_DeleteTexture	(this);	
}

STextureList::~STextureList				()			{	Device.Resources->_DeleteTextureList	(this);			}
SMatrixList::~SMatrixList				()			{	Device.Resources->_DeleteMatrixList		(this);			}