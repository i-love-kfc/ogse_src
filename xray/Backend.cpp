#include "Backend.h"

u16*	_IndexStream::Lock	( u32 Count, u32& vOffset )
{
//	PGO						(Msg("PGO:IB_LOCK:%d",Count));
	vOffset					= 0;
	BYTE* pLockedData		= 0;

	// Ensure there is enough space in the VB for this data
	R_ASSERT				((2*Count<=mSize) && Count);

	// If either user forced us to flush,
	// or there is not enough space for the index data,
	// then flush the buffer contents
	u32 dwFlags = LOCKFLAGS_APPEND;
	if ( 2*( Count + mPosition ) >= mSize )
	{
		mPosition	= 0;						// clear position
		dwFlags		= LOCKFLAGS_FLUSH;			// discard it's contens
		mDiscardID	++;
	}
	pIB->Lock				( mPosition * 2, Count * 2, (void**) &pLockedData, dwFlags);
	VERIFY					(pLockedData);

	vOffset					=	mPosition;

	return					LPWORD(pLockedData);
}

void	_IndexStream::Unlock(u32 RealCount)
{
//	PGO						(Msg("PGO:IB_UNLOCK:%d",RealCount));
	mPosition				+=	RealCount;
	VERIFY					(pIB);
	pIB->Unlock				();
}
