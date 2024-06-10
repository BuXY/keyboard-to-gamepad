#include "InternalButtonStates.h"

void InternalButtonStates::updateLatches()
{
	updateLatch(&alwaysRunLatch, &SetRunOn, &SetRunOff);
}

void InternalButtonStates::updateLatch(bool* io_latch, bool* i_setOn, bool* i_setOff)
{
	if (io_latch == nullptr || i_setOn == nullptr || i_setOff == nullptr)
	{
		return;
	}

	if (!(*io_latch) && (*i_setOn))
	{
		*io_latch = true;
	}
	else if ((*io_latch) && (*i_setOff))
	{
		*io_latch = false;
	}
}

bool InternalButtonStates::getRunState() const
{
	return Run ^ alwaysRunLatch;
}
