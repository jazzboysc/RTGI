#pragma once

#include <time.h>
#include <windows.h>

namespace RTGI
{

class Timer : public RefObject
{

private:
	double frequency;		  // time refresh frequency.
	LARGE_INTEGER prevCounter;   // windows version of timespec

public:
	/// Sets prev_ts to current time.
	Timer();

	/// Return time elapsed since delta() was called.
	/// In microseconds.
	/// Return -1 if error.
	long int delta(void);

	/// Return time elapsed since delta() was last called.
	/// In microseconds.
	/// Return -1 if error.
	long int split(void) const;

	typedef RefPointer<Timer> TimerPtr;
};

}