#include "math/CCMath.h"
#include "bytebuffer.h"
#include "CCSerializer.h"

NS_CC_BEGIN

const int Serializer::STREAM_OVERHEAD_SIZE = sizeof(unsigned short) + sizeof(unsigned int);
const unsigned short HEADER_STREAM_ID = 0x1000;
//---------------------------------------------------------------------
Serializer::Serializer()
{
    mVersion = "[Serializer_v1.00]";
}
//---------------------------------------------------------------------
Serializer::~Serializer()
{
}

//---------------------------------------------------------------------
bool Serializer::readFileHeader(ByteBuffer* stream)
{
    unsigned short headerID;
    readShorts(stream, &headerID, 1);
        
    if (headerID == HEADER_STREAM_ID)
    {
		std::string ver = readString(stream);
        if (ver != mVersion)
        {
			return false;
        }
    }
    else
	{
		return false;
	}
	return true;
}
//---------------------------------------------------------------------
unsigned short Serializer::readChunk(ByteBuffer* stream)
{
    unsigned short id;
    readShorts(stream, &id, 1);

	if(!eof(stream))
	readInts(stream, &mCurrentstreamLen, 1);
	
	return id;
}
//---------------------------------------------------------------------
void Serializer::readBools(ByteBuffer* stream, bool* pDest, size_t count)
{
    stream->read((unsigned char*)pDest, sizeof(bool) * count);
}
//---------------------------------------------------------------------
void Serializer::readFloats(ByteBuffer* stream, float* pDest, size_t count)
{
    stream->read((unsigned char*)pDest, sizeof(float) * count);
}
//---------------------------------------------------------------------
void Serializer::readShorts(ByteBuffer* stream, unsigned short* pDest, size_t count)
{
    stream->read((unsigned char*)pDest, sizeof(unsigned short) * count);
}
//---------------------------------------------------------------------
void Serializer::readInts(ByteBuffer* stream, unsigned int* pDest, size_t count)
{
    stream->read((unsigned char*)pDest, sizeof(unsigned int) * count);
}
//---------------------------------------------------------------------
std::string Serializer::readString(ByteBuffer* stream)
{
	std::string temp;
	temp.clear();
	while (stream->rpos() < stream->size())                         // prevent crash at wrong string format in packet
	{
		char c = stream->read<char>();
		if (c == '\n')
			break;
		temp += c;
	}
	return temp;
}
//---------------------------------------------------------------------
void Serializer::readObject(ByteBuffer* stream, Vec3& pDest)
{
	float tmp[3];
    readFloats(stream, tmp, 3);
	pDest.x = tmp[0];
	pDest.y = tmp[1];
	pDest.z = tmp[2];
}
//---------------------------------------------------------------------
void Serializer::readObject(ByteBuffer* stream, Quaternion& pDest)
{
    float tmp[4];
    readFloats(stream, tmp, 4);
    pDest.x = tmp[0];
    pDest.y = tmp[1];
    pDest.z = tmp[2];
    pDest.w = tmp[3];
}

bool Serializer::eof(ByteBuffer* stream)
{
	return stream->rpos() >= stream->wpos();
}

void Serializer::skip(ByteBuffer* stream, int count)
{
	int pos = (int)stream->rpos() + count;
	if (pos >= (int)stream->wpos())
	{
		pos = (int)stream->wpos();
	}
	if (pos < 0)
	{
		pos = 0;
	}

	stream->rpos((size_t)pos);
}

NS_CC_END

