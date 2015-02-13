//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
inline void RefObject::IncrementReferenceCount()
{
    mReferenceCount++;
}
//----------------------------------------------------------------------------
inline int RefObject::GetReferenceCount() const
{
    return mReferenceCount;
}
//----------------------------------------------------------------------------