//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_CommandQueue_H
#define RTGI_CommandQueue_H

#include "FrameworkCommon.h"
#include "RefObject.h"
#include "ComputeKernel.h"
#include "MemoryObject.h"

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

	void EnqueueKernel(ComputeKernel* kernel,  cl_uint workDim, 
		size_t* globalWorkSize, size_t* localWorkSize);

	// Only support blocking read for now.
	void EnqueueReadBuffer(MemoryObject* srcBuffer, size_t offset, 
		size_t dataSize, void* dstData);

	cl_command_queue GetQueue();

private:
	cl_command_queue mQueue;
};

typedef RefPointer<CommandQueue> CommandQueuePtr;

}

#endif