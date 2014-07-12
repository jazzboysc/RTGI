//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_CommandQueue_H
#define RTGI_CommandQueue_H

#include "FrameworkCommon.h"
#include "RefObject.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 07/11/2014
//----------------------------------------------------------------------------
class CommandQueue : public RefObject
{
public:
	CommandQueue(cl_context context, cl_device_id device);
	~CommandQueue();

	cl_command_queue GetQueue();

private:
	cl_command_queue mQueue;
};

typedef RefPointer<CommandQueue> CommandQueuePtr;

}

#endif