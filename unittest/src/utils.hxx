#ifndef _PEFRAMEWORK_UNITTEST_UTILITIES_
#define _PEFRAMEWORK_UNITTEST_UTILITIES_

#include <peframework.h>

#include <sdk/MemoryUtils.stream.h>

#include <assert.h>

// We want the ability to read PEFile from naked memory.
struct PEStreamConstMem final : public PEStream
{
    inline PEStreamConstMem( const void *memptr, size_t memSize ) : stream( memptr, memSize, allocMan )
    {
        return;
    }

    size_t Read( void *buf, size_t readCount ) override
    {
        return stream.Read( buf, readCount );
    }

    bool Write( const void *buf, size_t writeCount ) override
    {
        return ( stream.Write( buf, writeCount ) == writeCount );
    }

    bool Seek( pe_file_ptr_t ptr ) override
    {
        stream.Seek( ptr );
        return true;
    }

    pe_file_ptr_t Tell( void ) const override
    {
        return stream.Tell();
    }

private:
    nullBufAllocMan <pe_file_ptr_t> allocMan;
    memoryBufferStream <pe_file_ptr_t, decltype(allocMan), true, false> stream;
};

// Writing a PEFile to available memory buffer.
struct PEStreamDynMem final : public PEStream
{
    inline PEStreamDynMem( void ) : stream( nullptr, 0, allocMan )
    {
        return;
    }

    size_t Read( void *buf, size_t readCount ) override
    {
        return stream.Read( buf, readCount );
    }

    bool Write( const void *buf, size_t writeCount ) override
    {
        return ( stream.Write( buf, writeCount ) == writeCount );
    }

    bool Seek( pe_file_ptr_t ptr ) override
    {
        stream.Seek( ptr );
        return true;
    }

    pe_file_ptr_t Tell( void ) const override
    {
        return stream.Tell();
    }

    const void* Data( void ) const
    {
        return stream.Data();
    }

    size_t Size( void ) const
    {
        return (size_t)stream.Size();
    }

private:
    BasicMemStream::basicMemStreamAllocMan <pe_file_ptr_t> allocMan;
    BasicMemStream::basicMemoryBufferStream <pe_file_ptr_t> stream;
};

// Comparing two same-size memory blocks.
static void compare_memory( const void *srcMem, const void *dstMem, size_t memSize )
{
    for ( size_t n = 0; n < memSize; n++ )
    {
        unsigned char srcByte = *( (unsigned char*)srcMem + n );
        unsigned char dstByte = *( (unsigned char*)dstMem + n );

        assert( srcByte == dstByte );
    }
}

#endif //_PEFRAMEWORK_UNITTEST_UTILITIES_