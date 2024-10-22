#ifndef CCByteConv_h__
#define CCByteConv_h__

#define CC_LITTLE_ENDIAN 0
#define CC_BIG_ENDIAN 1

#if defined (i386) || defined (__i386__) || defined (_M_IX86) || \
	defined (vax) || defined (__alpha) || defined (__LITTLE_ENDIAN__) || \
	defined (ARM) || defined (_M_IA64) || defined (__ia64__) || \
	defined (_M_AMD64) || defined (__amd64)
// We know these are little endian.
#	define CC_ENDIAN CC_LITTLE_ENDIAN
#else
// Otherwise, we assume big endian.
#	define CC_ENDIAN CC_BIG_ENDIAN
#endif

NS_CC_BEGIN

	namespace ByteConverter
	{
		template<size_t T>
		inline void convert(char *val)
		{
			std::swap(*val, *(val + T - 1));
			convert<T - 2>(val + 1);
		}

		template<> inline void convert<0>(char *) {}
		template<> inline void convert<1>(char *) {}            // ignore central byte

		template<typename T> inline void apply(T *val)
		{
			convert<sizeof(T)>((char *)(val));
		}
	}

#if CC_ENDIAN == CC_BIG_ENDIAN
	template<typename T> inline void EndianConvert(T& val) { ByteConverter::apply<T>(&val); }
	template<typename T> inline void EndianConvertReverse(T&) { }
#else
	template<typename T> inline void EndianConvert(T&) { }
	template<typename T> inline void EndianConvertReverse(T& val) { ByteConverter::apply<T>(&val); }
#endif

	template<typename T> void EndianConvert(T*);         // will generate link error
	template<typename T> void EndianConvertReverse(T*);  // will generate link error

	inline void EndianConvert(unsigned char&) { }
	inline void EndianConvert(char&) { }
	inline void EndianConvertReverse(unsigned char&) { }
	inline void EndianConvertReverse(char&) { }

NS_CC_END

#endif // CCByteConv_h__
