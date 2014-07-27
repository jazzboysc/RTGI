//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "ComputingFrameworkPCH.h"
#include "ComputeProgram.h"

using namespace RTGI;

//----------------------------------------------------------------------------
ComputeProgram::ComputeProgram(cl_context context, cl_device_id device,
    const std::string& programFileName)
    :
    mProgram(0),
    mContext(context),
    mDevice(device)
{
    mProgramFileName = programFileName;
	bool res = LoadFromFile(programFileName);
	assert( res );

	// TODO:
	// Assign defalut source if LoadFromFile fails.
}
//----------------------------------------------------------------------------
ComputeProgram::~ComputeProgram()
{
    clReleaseProgram(mProgram);
}
//----------------------------------------------------------------------------
void ComputeProgram::CreateDeviceResource()
{
	if( mProgram )
	{
		return;
	}

	const char* programText = mProgramSource.c_str();
    size_t programSize = mProgramSource.size();

    cl_int res;
    mProgram = clCreateProgramWithSource(mContext, 1,
        (const char**)&programText, &programSize, &res);
    assert( res >= 0 );
    
    res = clBuildProgram(mProgram, 0, NULL, NULL, NULL, NULL);
	if( res < 0 )
	{
        size_t logSize;
        clGetProgramBuildInfo(mProgram, mDevice, CL_PROGRAM_BUILD_LOG, 0,
            NULL, &logSize);
        if( logSize > 1 )
        {
            char* acInfoLog = new char[logSize + 1];
            acInfoLog[logSize] = '\0';
            clGetProgramBuildInfo(mProgram, mDevice, CL_PROGRAM_BUILD_LOG,
                logSize + 1, acInfoLog, NULL);
            printf("Failed compiling %s\n%s\n", mProgramFileName.c_str(), acInfoLog);
            delete[] acInfoLog;
        }
        
		assert( false );
	}
   
#ifdef RTGI_OUTPUT_RESOURCE_LOADING
    printf("Loading compute program %s finished\n", mProgramFileName.c_str());
#endif
}
//----------------------------------------------------------------------------
cl_program ComputeProgram::GetProgram() const
{
	return mProgram;
}
//----------------------------------------------------------------------------
std::string ComputeProgram::GetProgramFileName() const
{
    return mProgramFileName;
}
//----------------------------------------------------------------------------
bool ComputeProgram::LoadFromFile(const std::string& programFileName)
{
	std::ifstream programFileData(programFileName);
    if( !programFileData )
    {
        return false;
    }
	
	std::string curLine;
	while( !programFileData.eof() )
	{
		getline(programFileData, curLine);
		mProgramSource += curLine + "\n";
	}

	return true;
}
//----------------------------------------------------------------------------