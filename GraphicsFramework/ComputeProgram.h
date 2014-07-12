//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_ComputeProgram_H
#define RTGI_ComputeProgram_H

#include "FrameworkCommon.h"
#include "RefObject.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 07/11/2014
//----------------------------------------------------------------------------
class ComputeProgram : public RefObject
{
public:
	ComputeProgram(cl_context context, cl_device_id device,
        const std::string& programFileName);
	virtual ~ComputeProgram();

	void CreateDeviceResource();
	cl_program GetProgram() const;
    std::string GetProgramFileName() const;

protected:
	bool LoadFromFile(const std::string& programFileName);

    std::string mProgramFileName;
	std::string mProgramSource;
	cl_program mProgram;
    cl_context mContext;
    cl_device_id mDevice;
};

typedef RefPointer<ComputeProgram> ComputeProgramPtr;

}

#endif
