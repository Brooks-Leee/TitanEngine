#include "stdafx.h"
#include "FRHI.h"
#include "FRHIDX12.h"


FRHI* FRHI::RHI = nullptr;

FRHI* FRHI::Get()
{
	return RHI;
}

void FRHI::DestoryRHI()
{
	delete RHI;
	RHI = nullptr;
}



void FRHI::CreateRHI()
{
#if defined (_PLATFORM_WINDOWS) && (_RHI_DX12)
	RHI = new FRHIDX12;
#endif



}
