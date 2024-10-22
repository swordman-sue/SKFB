#ifndef CCByteBuffer_h__
#define CCByteBuffer_h__

#include "byteconv.h"
#include "cocos/base/ccMacros.h"
#include "platform/CCCommon.h"
#include "cocos/base/ccUtils.h"

#include <string>
#include <vector>
#include <list>
#include <map>

NS_CC_BEGIN
template<class T>
struct Unused
{
	Unused() {}
};

class ByteBuffer
{
public:
	const static size_t DEFAULT_SIZE = 0x0;

	// constructor
	ByteBuffer(): _rpos(0), _wpos(0), _capacity(DEFAULT_SIZE), _storage(NULL), _is_grow_2_pow(false)
	{
		if(_capacity > 0)
		{
			_storage = (unsigned char*)malloc(_capacity);
		}
		else
		{
			_storage = 0;
		}
	}

	// constructor
	ByteBuffer(size_t res): _rpos(0), _wpos(0), _capacity(res), _is_grow_2_pow(false)
	{
		_storage = (unsigned char*)malloc(_capacity);
	}

	// copy constructor
	ByteBuffer(const ByteBuffer &buf): 
		_rpos(buf._rpos), 
		_wpos(buf._wpos),
		_capacity(buf._capacity),
		_storage(NULL), 
		_is_grow_2_pow(false)
	{
		if(_capacity > 0)
		{
			_storage = (unsigned char*)malloc(_capacity);
			memcpy(_storage,buf._storage,_capacity);
		}
		else
		{
			_storage = 0;
		}
	}

	~ByteBuffer()
	{
		if(_storage)
		{
			free(_storage);
			_storage=NULL;
			_capacity=0;
		}
	}

	void clear()
	{
		memset(_storage,0,_capacity);
		_rpos = _wpos = 0;
	}

	template <typename T> void put(size_t pos,T value)
	{
		EndianConvert(value);
		put(pos,(unsigned char *)&value,sizeof(value));
	}

	ByteBuffer &operator<<(unsigned char value)
	{
		append<unsigned char>(value);
		return *this;
	}

	ByteBuffer &operator<<(unsigned short value)
	{
		append<unsigned short>(value);
		return *this;
	}

	ByteBuffer &operator<<(unsigned int value)
	{
		append<unsigned int>(value);
		return *this;
	}

	// by zquan 2014.04.03 暂时不支持64位
	//ByteBuffer &operator<<(uint64c value)
	//{
	//	append<uint64c>(value);
	//	return *this;
	//}
	// end

	// signed as in 2e complement
	ByteBuffer &operator<<(char value)
	{
		append<char>(value);
		return *this;
	}

	ByteBuffer &operator<<(short value)
	{
		append<short>(value);
		return *this;
	}

	ByteBuffer &operator<<(int value)
	{
		append<int>(value);
		return *this;
	}

	// by zquan 2014.04.03 暂时不支持64位
	//ByteBuffer &operator<<(int64c value)
	//{
	//	append<int64c>(value);
	//	return *this;
	//}
	// end

	// floating points
	ByteBuffer &operator<<(float value)
	{
		append<float>(value);
		return *this;
	}

	ByteBuffer &operator<<(double value)
	{
		append<double>(value);
		return *this;
	}

	ByteBuffer &operator<<(const std::string &value)
	{
		append((unsigned char const *)value.c_str(), value.length());
		append((unsigned char)0);
		return *this;
	}

	ByteBuffer &operator<<(const char *str)
	{
		append((unsigned char const *)str, str ? strlen(str) : 0);
		append((unsigned char)0);
		return *this;
	}

	ByteBuffer &operator>>(bool &value)
	{
		value = read<char>() > 0 ? true : false;
		return *this;
	}

	ByteBuffer &operator>>(unsigned char &value)
	{
		value = read<unsigned char>();
		return *this;
	}

	ByteBuffer &operator>>(unsigned short &value)
	{
		value = read<unsigned short>();
		return *this;
	}

	ByteBuffer &operator>>(unsigned int &value)
	{
		value = read<unsigned int>();
		return *this;
	}

	// by zquan 2014.04.03 暂时不支持64位
	//ByteBuffer &operator>>(uint64c &value)
	//{
	//	value = read<uint64c>();
	//	return *this;
	//}
	// end

	//signed as in 2e complement
	ByteBuffer &operator>>(char &value)
	{
		value = read<char>();
		return *this;
	}

	ByteBuffer &operator>>(short &value)
	{
		value = read<short>();
		return *this;
	}

	ByteBuffer &operator>>(int &value)
	{
		value = read<int>();
		return *this;
	}

	// by zquan 2014.04.03 暂时不支持64位
	//ByteBuffer &operator>>(int64c &value)
	//{
	//	value = read<int64c>();
	//	return *this;
	//}
	// end

	ByteBuffer &operator>>(float &value)
	{
		value = read<float>();
		return *this;
	}

	ByteBuffer &operator>>(double &value)
	{
		value = read<double>();
		return *this;
	}

	ByteBuffer &operator>>(std::string& value)
	{
		value.clear();
		while (rpos() < size())                         // prevent crash at wrong string format in packet
		{
			char c = read<char>();
			if (c == 0)
				break;
			value += c;
		}
		return *this;
	}

	template<class T>
	ByteBuffer &operator>>(Unused<T> const&)
	{
		read_skip<T>();
		return *this;
	}


	unsigned char operator[](size_t pos) const
	{
		return read<unsigned char>(pos);
	}

	size_t rpos() const { return _rpos; }

	size_t rpos(size_t rpos_)
	{
		_rpos = rpos_;
		return _rpos;
	}

	size_t wpos() const { return _wpos; }

	size_t wpos(size_t wpos_)
	{
		_wpos = wpos_;
		return _wpos;
	}

	unsigned char* rd_ptr()
	{
		return &_storage[_rpos];
	}

	unsigned char* wr_ptr()
	{
		return &_storage[_wpos];
	}

	size_t space() const
	{
		if(_wpos>=_capacity)
		{
			return 0;
		}
		else
		{
			return _capacity-_wpos;
		}
	}

	//丢弃已读取数据
	bool crunch()
	{
		if(_rpos>_wpos)
		{
			return false;
		}

		size_t len = notReadSize();
		memmove(_storage,rd_ptr(),len);
		rpos(0);
		wpos(len);
		return true;
	}

	void resetAsBuffer()
	{
		memset(_storage,0,_capacity);
		_rpos = _wpos =0;
	}

	size_t notReadSize() const
	{
		if(_wpos>_rpos)
		{
			return _wpos-_rpos;
		}
		else
		{
			return 0;
		}
	}

	template<typename T>
	void read_skip() { read_skip(sizeof(T)); }

	void read_skip(size_t skip)
	{
		if(_rpos + skip > size())
		{
			log("Read Skip Error:BufferSize=%d SkipLen=%d,Pos=%d",size(),skip,_rpos);
			return;
		}
		_rpos += skip;
	}

	template <typename T> T read()
	{
		T r = read<T>(_rpos);
		_rpos += sizeof(T);
		return r;
	}

	template <typename T> T read(size_t pos) const
	{
		if(pos + sizeof(T) > size())
		{
			CCLOGERROR("Read Type Error:BufferSize=%d TypeSize=%d,pos=%d",size(),sizeof(T),pos);
			return T();
		}
		T val = *((T const*)&_storage[pos]);
		EndianConvert(val);
		return val;
	}

	void read(unsigned char *dest, size_t len)
	{
		if(_rpos  + len > size())
		{
			log("Read Len Error:BufferSize=%d ReadLen=%d,pos=%d",size(),len, _rpos);
			return;
		}
		memcpy(dest, &_storage[_rpos], len);
		_rpos += len;
	}

	// by zquan 2014.04.03 暂时不支持64位
	//uint64c readPackGUID()
	//{
	//	uint64c guid = 0;
	//	unsigned char guidmark = 0;
	//	(*this) >> guidmark;

	//	for(int i = 0; i < 8; ++i)
	//	{
	//		if(guidmark & (unsigned char(1) << i))
	//		{
	//			unsigned char bit;
	//			(*this) >> bit;
	//			guid |= (uint64c(bit) << (i * 8));
	//		}
	//	}

	//	return guid;
	//}
	// end

	const unsigned char *contents() const { return _storage; }

	size_t size() const { return _wpos; }
	bool empty() const { return _wpos==0; }

	////void resize(size_t newsize)
	////{
	////	_rpos = 0;
	////	_wpos = newsize;
	////}

	void reserve(size_t ressize)
	{
		if(ressize>_capacity)
		{
			if(_is_grow_2_pow)
			{
				ressize = ccNextPOT(ressize);
			}

			if(_storage==0)
			{
				_storage= (unsigned char*)(malloc(ressize));
				_capacity=ressize;
			}
			else
			{
				unsigned char* new_data = (unsigned char*)(malloc(ressize));
				size_t cpy_size = std::min<size_t>(ressize, _capacity);
				if(cpy_size > 0)
				{
					memcpy(new_data, _storage, cpy_size);
				}
				if(_capacity > 0)
				{
					free(_storage);
				}
				_storage = new_data;
				_capacity=ressize;
			}
		}
	}

	void append(const std::string& str)
	{
		append((unsigned char const*)str.c_str(), str.size() + 1);
	}

	void append(const char *src, size_t cnt)
	{
		return append((const unsigned char *)src, cnt);
	}


	void append(const unsigned char *src, size_t cnt)
	{
		if (!cnt)
			return;

		assert(size()<100*1024*1024 && "ByteBuffer过长!");

		if (_capacity < _wpos + cnt)
		{
			reserve(_wpos + cnt);
		}
		memcpy(&_storage[_wpos], src, cnt);
		_wpos += cnt;
	}

	void append(const ByteBuffer& buffer)
	{
		if(buffer.wpos())
			append(buffer.contents(), buffer.wpos());
	}

	// can be used in SMSG_MONSTER_MOVE opcode
	void appendPackXYZ(float x, float y, float z)
	{
		unsigned int packed = 0;
		packed |= ((int)(x / 0.25f) & 0x7FF);
		packed |= ((int)(y / 0.25f) & 0x7FF) << 11;
		packed |= ((int)(z / 0.25f) & 0x3FF) << 22;
		*this << packed;
	}

	//void appendPackGUID(UINT64 guid)
	//{
	//	unsigned char packGUID[8+1];
	//	packGUID[0] = 0;
	//	size_t size = 1;
	//	for (unsigned char i = 0; guid != 0; ++i)
	//	{
	//		if (guid & 0xFF)
	//		{
	//			packGUID[0] |= unsigned char(1 << i);
	//			packGUID[size] =  unsigned char(guid & 0xFF);
	//			++size;
	//		}

	//		guid >>= 8;
	//	}

	//	append(packGUID, size);
	//}

	void put(size_t pos, const unsigned char *src, size_t cnt)
	{
		if(pos + cnt > size())
		{
			log("Put Error:BufferSize=%1% Pos=%2%,WriteLen=%3%",size(),pos, cnt);
			return;
		}
		memcpy(&_storage[pos], src, cnt);
	}

	void print_storage() const
	{
		////if(LogManager::getSingleton().getLowLogLevel()>Debug)
		////{
		////	return;
		////}

		////LOG_SRC(Debug) << "STORAGE_SIZE:" << size();
		////std::string tmpStr;
		////for(unsigned int i = 0; i < size(); ++i)
		////{
		////	tmpStr.append((boost::format("%1$03u") % (unsigned int)read<unsigned char>(i)).str());
		////	tmpStr.append("-");
		////}
		////LOG_SRC(Debug) << tmpStr;
	}

	void textlike() const
	{
		////if(LogManager::getSingleton().getLowLogLevel()>Debug)
		////{
		////	return;
		////}
		////
		////LOG_SRC(Debug) << "STORAGE_SIZE:" << size();

		////std::string tmpstr;
		////for(unsigned int i = 0; i < size(); ++i)
		////{
		////	tmpstr.append(boost::lexical_cast<std::string>((char)read<unsigned char>(i)));
		////}
	
		////LOG_SRC(Debug) << tmpstr;
	}

	std::string getAsHexString() const
	{
		std::string tmpStr;
		if(size()>0)
		{
			tmpStr.reserve(size()*2);
		}
		for(unsigned int i = 0; i < size(); ++i)
		{
			char buf[100];
			memset(buf,0,sizeof(buf));
			sprintf(buf,"%02X",(unsigned int)read<unsigned char>(i));
			tmpStr.append(std::string(buf));
		}
		return tmpStr;
	}

	void setAsHexString(const std::string& hexStr)
	{
		char buf[3];
		buf[2]=0;
		clear();
		int byteLen = (hexStr.size()/2);
		reserve(byteLen);
		for (int i=0; i<byteLen; i++)
		{
			buf[0] = hexStr[i*2];
			buf[1] = hexStr[i*2+1];
			int tmpVal=0;
			sscanf(buf,"%X",&tmpVal);
			*this << (unsigned char)tmpVal;
		}
	}

	bool isGrow2Pow() const{return _is_grow_2_pow;}

	void setGrow2Pow(bool is_grow_2_pow){_is_grow_2_pow = is_grow_2_pow;}

	void hexlike() const
	{
		////if(LogManager::getSingleton().getLowLogLevel()>Debug)
		////{
		////	return;
		////}

		////unsigned int j = 1, k = 1;

		////LOG_SRC(Debug) << "STORAGE_SIZE:" << size();
		////////if (sLog.IsIncludeTime())
		////////	sLog.outDebugInLine("         ");

		////std::string tmpStr;
		////for(unsigned int i = 0; i < size(); ++i)
		////{
		////	if ((i == (j * 8)) && ((i != (k * 16))))
		////	{
		////		tmpStr.append((boost::format("| %1$02X ") % (unsigned int)read<unsigned char>(i)).str());
		////		++j;
		////	}
		////	else if (i == (k * 16))
		////	{
		////		tmpStr.append("\n");
		////		for(int spaceNum=0;spaceNum<38;spaceNum++)
		////		{
		////			tmpStr.append(" ");
		////		}
		////		////if(sLog.IsIncludeTime())
		////		////	sLog.outDebugInLine("         ");
		////		tmpStr.append((boost::format("%1$02X ") % (unsigned int)(read<unsigned char>(i))).str());
		////		++k;
		////		++j;
		////	}
		////	else
		////	{
		////		tmpStr.append((boost::format("%1$02X ") % (unsigned int)read<unsigned char>(i)).str());
		////	}
		////}
		////LOG_SRC(Debug) << tmpStr;
	}
private:
	// limited for internal use because can "append" any unexpected type (like pointer and etc) with hard detection problem
	template <typename T> void append(T value)
	{
		EndianConvert(value);
		append((unsigned char *)&value, sizeof(value));
	}

	////template<class T> void append(const T *src, size_t cnt)
	////{
	////	return append((const unsigned char *)src, cnt * sizeof(T));
	////}

protected:
	size_t _rpos, _wpos, _capacity;
	unsigned char* _storage;
	bool _is_grow_2_pow;
	//std::vector<unsigned char> _storage;

};

template <typename T>
inline ByteBuffer &operator<<(ByteBuffer &b,std::vector<T> const& v)
{
	b << (unsigned int)v.size();
    //memcpy()
    //b.append((const unsigned char*)&v[0],sizeof(T)*v.size());
	for (typename std::vector<T>::const_iterator i = v.begin(); i != v.end(); ++i)
	{
		b << *i;
	}
	return b;
}

template <typename T>
inline ByteBuffer &operator>>(ByteBuffer &b, std::vector<T> &v)
{
	unsigned int vsize;
	b >> vsize;
	v.clear();
    v.reserve(vsize);
	while(vsize--)
	{
		T t;
		b >> t;
		v.push_back(t);
	}
	return b;
}

template <typename T>
inline ByteBuffer &operator<<(ByteBuffer &b, std::list<T> const& v)
{
	b << (unsigned int)v.size();
	for (typename std::list<T>::iterator i = v.begin(); i != v.end(); ++i)
	{
		b << *i;
	}
	return b;
}

template <typename T>
inline ByteBuffer &operator>>(ByteBuffer &b, std::list<T> &v)
{
	unsigned int vsize;
	b >> vsize;
	v.clear();
	while(vsize--)
	{
		T t;
		b >> t;
		v.push_back(t);
	}
	return b;
}

template <typename K, typename V>
inline ByteBuffer &operator<<(ByteBuffer &b, std::map<K, V> &m)
{
	b << (unsigned int)m.size();
	for (typename std::map<K, V>::iterator i = m.begin(); i != m.end(); ++i)
	{
		b << i->first << i->second;
	}
	return b;
}

template <typename K, typename V>
inline ByteBuffer &operator>>(ByteBuffer &b, std::map<K, V> &m)
{
	unsigned int msize;
	b >> msize;
	m.clear();
	while(msize--)
	{
		K k;
		V v;
		b >> k >> v;
		m.insert(make_pair(k, v));
	}
	return b;
}

template<>
inline void ByteBuffer::read_skip<char*>()
{
	std::string temp;
	*this >> temp;
}

template<>
inline void ByteBuffer::read_skip<char const*>()
{
	read_skip<char*>();
}

template<>
inline void ByteBuffer::read_skip<std::string>()
{
	read_skip<char*>();
}
NS_CC_END

#endif // CCByteBuffer_h__


