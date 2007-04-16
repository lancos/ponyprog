#include "busio.h"

#include "e2app.h"

BusIO::BusIO(BusInterface *p)
{
	err_no = last_addr = 0;
	busI = p;
	old_progress = 0;
}

int BusIO::CheckAbort(int progress)
{
	int abort = THEAPP->GetAbortFlag();

	if (!abort)
	{
		if ( (progress == 0 && old_progress != 0) ||
			progress > old_progress + 4 )
		{
			THEAPP->SetProgress(progress);
			old_progress = progress;
		//	if (progress == 100)
				THEAPP->CheckEvents();
		}
	}

	return abort;
}

int BusIO::Error()
{
	int old_val = err_no;
	err_no = 0;
	return old_val;
}

void BusIO::SetDelay()
{
	shot_delay = 5;	//basic timing of 5usec
}

void BusIO::SetDelay(int delay)
{
	if (delay >= 0)
		shot_delay = delay;
}
