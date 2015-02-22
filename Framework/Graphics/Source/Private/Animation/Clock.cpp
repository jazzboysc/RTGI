/* Module      : main
* Author      : Yong Piao
* Email       : ypiao@wpi.edu
* Course      : CS4732
*
* Description : Project 0 executable main.
*
* Date        : 10/30/2014
*
* History:
* Revision      Date          Changed By
* --------      ----------    ----------
* 01.00         10/30/2014		Yong Piao
* First release.
*
* (c) Copyright 2013, Worcester Polytechnic Institute.
*/

/* -- INCLUDE FILES ------------------------------------------------------ */
#include <windows.h>
#include <stdio.h>
#include "Clock.h"
/* -- DATA STRUCTURES ---------------------------------------------------- */

/* -- GLOBAL VARIABLES --------------------------------------------------- */

/* -- LOCAL VARIABLES ---------------------------------------------------- */



static LARGE_INTEGER currentCounter;

//Windows specific time query macros
#define QUERY_TIME()												\
if (!QueryPerformanceCounter(&currentCounter))						\
{																	\
	fprintf(stderr, "Clock: Fatal Error: Could not query time.\n"); \
}

#define TIME_DIFF(currentCounter, prevCounter)						\
((currentCounter.QuadPart - prevCounter.QuadPart) / frequency)

long int Clock::split(void) const
{
	QUERY_TIME();
	return static_cast<long>(TIME_DIFF(currentCounter, this->prevCounter));
}

long int Clock::delta(void)
{
	long int result = this->split();
	prevCounter = currentCounter;

	return result;
}

Clock::Clock()
{
	LARGE_INTEGER li;
	if (!QueryPerformanceFrequency(&li))
	{
		fprintf(stderr, "Clock: Fatal Error: Could not query frequency.\n");
	}
	else
	{
		this->frequency = double(li.QuadPart) / 1000.0; //Divide by 1000000.0 if microsec is desired.
	}

	this->delta(); // initialize delta time
}