#include "ziputilsex.h"

#include <zlib.h>
#include <stdio.h>
#if defined(_WIN32)
	#include <memory.h>
#elif defined(__APPLE__)
	#include <memory.h>
#elif defined(__ANDROID__)
	#include "CCStdC.h"
#endif
static int get_byte(z_stream *stream) {
	if (stream->avail_in <= 0) return EOF;
	stream->avail_in--;
	return *(stream->next_in)++;
}

static int checkheader(z_stream *stream) {
	int flags, c;
	unsigned int len;

	if (get_byte(stream) != 0x1f || get_byte(stream) != 0x8b)
		return Z_DATA_ERROR;
	if (get_byte(stream) != Z_DEFLATED || ((flags = get_byte(stream)) & 0xE0) != 0)
		return Z_DATA_ERROR;
	for (len = 0; len < 6; len++) (void)get_byte(stream);

	if ((flags & 0x04) != 0) { /* skip the extra field */
		len  =  (unsigned int)get_byte(stream);
		len += ((unsigned int)get_byte(stream)) << 8;
		/* len is garbage if EOF but the loop below will quit anyway */
		while (len-- != 0 && get_byte(stream) != EOF) ;
	}
	if ((flags & 0x08) != 0) { /* skip the original file name */
		while ((c = get_byte(stream)) != 0 && c != EOF) ;
	}
	if ((flags & 0x10) != 0) {   /* skip the .gz file comment */
		while ((c = get_byte(stream)) != 0 && c != EOF) ;
	}
	if ((flags & 0x02) != 0) {  /* skip the header crc */
		for (len = 0; len < 2; len++) (void)get_byte(stream);
	}
	return Z_OK;
}

int ZipUtilsEx::InflateGZipMemory( unsigned char *target, int target_size, unsigned char *source, int source_size )
{
	int src_len  = source_size;
	int dest_len = target_size;
	int zerr     = Z_DATA_ERROR;

	if (src_len > 0)
	{
		z_stream stream;
		memset(&stream, 0, sizeof (stream));
		if ((zerr = inflateInit2(&stream, -MAX_WBITS)) == Z_OK)
		{
			stream.next_in  = source;
			stream.avail_in = source_size;

			stream.next_out  = target;
			stream.avail_out = target_size;

			if ((zerr = checkheader(&stream)) == Z_OK)
			{
				zerr = inflate (&stream, Z_NO_FLUSH);
				dest_len = target_size - stream.avail_out;

				if (zerr == Z_OK || zerr == Z_STREAM_END)
				{
					inflateEnd(&stream);
				}
			}
		}
	}
	if (zerr != Z_OK && zerr != Z_STREAM_END)
	{
		return 0;
	}
	return dest_len;
}
