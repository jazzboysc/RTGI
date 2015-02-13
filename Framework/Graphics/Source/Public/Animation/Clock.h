///
/// The clock, for timing
///

#pragma once

#include <time.h>
#include <windows.h>

class Clock
{

private:
	//struct timespec prev_ts;  /// Store previous time called.
	double frequency;		  // stores the motherboard time refresh frequency.
	LARGE_INTEGER prevCounter;   // windows version of timespec

public:
	/// Sets prev_ts to current time.
	Clock();

	/// Return time elapsed since delta() was called.
	/// Units are microseconds.
	/// Return -1 if error.
	long int delta(void);

	/// Return time elapsed since delta() was last called.
	/// Units are microseconds.
	/// Return -1 if error.
	long int split(void) const;
};