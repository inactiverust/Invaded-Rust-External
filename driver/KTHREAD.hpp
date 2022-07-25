#pragma once
#include "includes.h"

class _CKTHREAD
{
public:
	VOID set_previous_mode(UCHAR mode) 
	{
		UCHAR* current_mode = reinterpret_cast<UCHAR*>(this + KTHREAD::PreviousMode);
		*current_mode = mode;
	}
private:
};