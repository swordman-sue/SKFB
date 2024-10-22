#ifndef DreamSerializer_h__
#define DreamSerializer_h__

#include <string>
NS_CC_BEGIN

class ByteBuffer;

class  Serializer
{
public:
	static const int STREAM_OVERHEAD_SIZE;
    Serializer();
    virtual ~Serializer();

protected:
    unsigned int mCurrentstreamLen;
	std::string mVersion;
        
    virtual bool readFileHeader(ByteBuffer* stream);
    virtual unsigned short readChunk(ByteBuffer* stream);
        
    void readBools(ByteBuffer* stream, bool* pDest, size_t count);
    void readFloats(ByteBuffer* stream, float* pDest, size_t count);
    void readShorts(ByteBuffer* stream, unsigned short* pDest, size_t count);
    void readInts(ByteBuffer* stream, unsigned int* pDest, size_t count);
    void readObject(ByteBuffer* stream, Vec3& pDest);
    void readObject(ByteBuffer* stream, Quaternion& pDest);

    std::string readString(ByteBuffer* stream);

	bool eof(ByteBuffer* stream);
	void skip(ByteBuffer* stream, int count);
};

NS_CC_END


#endif
