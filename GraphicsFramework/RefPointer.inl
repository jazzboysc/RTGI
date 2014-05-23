//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
template <class T>
RefPointer<T>::RefPointer(T* object)
{
    mObject = object;

    if( mObject )
    {
        mObject->IncrementReferenceCount();
    }
}
//----------------------------------------------------------------------------
template <class T>
RefPointer<T>::RefPointer(const RefPointer& refPointer)
{
    mObject = refPointer.mObject;

    if( mObject )
    {
        mObject->IncrementReferenceCount();
    }
}
//----------------------------------------------------------------------------
template <class T>
RefPointer<T>::~RefPointer()
{
    if( mObject )
    {
        mObject->DecrementReferenceCount();
    }
}
//----------------------------------------------------------------------------
template <class T>
RefPointer<T>::operator T*() const
{
    return mObject;
}
//----------------------------------------------------------------------------
template <class T>
T& RefPointer<T>::operator*() const
{
    return *mObject;
}
//----------------------------------------------------------------------------
template <class T>
T* RefPointer<T>::operator->() const
{
    return mObject;
}
//----------------------------------------------------------------------------
template <class T>
RefPointer<T>& RefPointer<T>::operator=(T* object)
{
    if( mObject != object )
    {
        if( object )
        {
            object->IncrementReferenceCount();
        }

        if( mObject )
        {
            mObject->DecrementReferenceCount();
        }

        mObject = object;
    }

    return *this;
}
//----------------------------------------------------------------------------
template <class T>
RefPointer<T>& RefPointer<T>::operator=(const RefPointer& refPointer)
{
    if( mObject != refPointer.mObject )
    {
        if( refPointer.mObject )
        {
            refPointer.mObject->IncrementReferenceCount();
        }

        if( mObject )
        {
            mObject->DecrementReferenceCount();
        }

        mObject = refPointer.mObject;
    }

    return *this;
}
//----------------------------------------------------------------------------
template <class T>
bool RefPointer<T>::operator==(T* object) const
{
    return (mObject == object);
}
//----------------------------------------------------------------------------
template <class T>
bool RefPointer<T>::operator!=(T* object) const
{
    return (mObject != object);
}
//----------------------------------------------------------------------------
template <class T>
bool RefPointer<T>::operator==(const RefPointer& refPointer) const
{
    return (mObject == refPointer.mObject);
}
//----------------------------------------------------------------------------
template <class T>
bool RefPointer<T>::operator!=(const RefPointer& refPointer) const
{
    return (mObject != refPointer.mObject);
}
//----------------------------------------------------------------------------