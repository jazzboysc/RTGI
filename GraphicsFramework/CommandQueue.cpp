//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "CommandQueue.h"

using namespace RTGI;

//--------------------------------------------------------------------------
CommandQueue::CommandQueue(cl_context context, cl_device_id device)
{
    cl_int res;
    mQueue = clCreateCommandQueue(context, device, 0, &res);
    assert( res >= 0 );
}
//--------------------------------------------------------------------------
CommandQueue::~CommandQueue()
{
    clReleaseCommandQueue(mQueue);
}
//--------------------------------------------------------------------------
cl_command_queue CommandQueue::GetQueue()
{
	return mQueue;
}
//--------------------------------------------------------------------------
