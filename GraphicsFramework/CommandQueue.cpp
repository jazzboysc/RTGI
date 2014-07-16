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
void CommandQueue::EnqueueKernel(ComputeKernel* kernel,  cl_uint workDim, 
	size_t* globalWorkSize, size_t* localWorkSize)
{
	cl_int res;
	res = clEnqueueNDRangeKernel(mQueue, kernel->GetKernel(), workDim, NULL, 
		(const size_t*)globalWorkSize, (const size_t*)localWorkSize, 0, 
		NULL, NULL);
    assert( res >= 0 );
}
//--------------------------------------------------------------------------
void CommandQueue::ExecuteKernel(ComputeKernel* kernel,  cl_uint workDim,
    size_t* globalWorkSize, size_t* localWorkSize)
{
    glFinish();
    
	cl_int res;
    cl_event kernelEvent;
    cl_mem object = kernel->GetArgument(0)->GetMO();
    res = clEnqueueAcquireGLObjects(mQueue, 1, &object, 0, NULL, NULL);
    assert( res >= 0 );
    
 	res = clEnqueueNDRangeKernel(mQueue, kernel->GetKernel(), workDim, NULL,
        (const size_t*)globalWorkSize, (const size_t*)localWorkSize, 0,
        NULL, &kernelEvent);
    assert( res >= 0 );
    
    clWaitForEvents(1, &kernelEvent);
    
    clEnqueueReleaseGLObjects(mQueue, 1, &object, 0, NULL, NULL);
    clFinish(mQueue);
    clReleaseEvent(kernelEvent);
}
//--------------------------------------------------------------------------
void CommandQueue::EnqueueReadBuffer(MemoryObject* srcBuffer, size_t offset, 
	size_t dataSize, void* dstData)
{
	cl_int res;
	res = clEnqueueReadBuffer(mQueue, srcBuffer->GetMO(), CL_TRUE, offset, 
		dataSize, dstData, 0, NULL, NULL);
}
//--------------------------------------------------------------------------