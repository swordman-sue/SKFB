#include "strutil.h"
#include <algorithm>
////#include "utilityc/DreamStringConverter.h"

NS_CC_BEGIN
//---------------------------------------------------------
//-----------------------------------------------------------------------
const std::string StrUtil::BLANK;
//-----------------------------------------------------------------------
void StrUtil::trim(std::string& str, bool left, bool right)
{
    /*
    size_t lspaces, rspaces, len = length(), i;

    lspaces = rspaces = 0;

    if( left )
    {
        // Find spaces / tabs on the left
        for( i = 0;
            i < len && ( at(i) == ' ' || at(i) == '\t' || at(i) == '\r');
            ++lspaces, ++i );
    }
        
    if( right && lspaces < len )
    {
        // Find spaces / tabs on the right
        for( i = len - 1;
            i >= 0 && ( at(i) == ' ' || at(i) == '\t' || at(i) == '\r');
            rspaces++, i-- );
    }

    *this = substr(lspaces, len-lspaces-rspaces);
    */
    static const std::string delims = " \t\r";
    if(right)
        str.erase(str.find_last_not_of(delims)+1); // trim right
    if(left)
        str.erase(0, str.find_first_not_of(delims)); // trim left
}

//-----------------------------------------------------------------------
StrVec StrUtil::split( const std::string& str, const std::string& delims, unsigned int maxSplits, bool preserveDelims)
{
    StrVec ret;
    // Pre-allocate some space for performance
    ret.reserve(maxSplits ? maxSplits+1 : 10);    // 10 is guessed capacity for most case

    unsigned int numSplits = 0;

    // Use STL methods 
    size_t start, pos;
    start = 0;
    do 
    {
        pos = str.find_first_of(delims, start);
        if (pos == start)
        {
            // Do nothing
            start = pos + 1;
        }
        else if (pos == std::string::npos || (maxSplits && numSplits == maxSplits))
        {
            // Copy the rest of the string
            ret.push_back( str.substr(start) );
            break;
        }
        else
        {
            // Copy up to delimiter
            ret.push_back( str.substr(start, pos - start) );

            if(preserveDelims)
            {
                // Sometimes there could be more than one delimiter in a row.
                // Loop until we don't find any more delims
                size_t delimStart = pos, delimPos;
                delimPos = str.find_first_not_of(delims, delimStart);
                if (delimPos == std::string::npos)
                {
                    // Copy the rest of the string
                    ret.push_back( str.substr(delimStart) );
                }
                else
                {
                    ret.push_back( str.substr(delimStart, delimPos - delimStart) );
                }
            }

            start = pos + 1;
        }
        // parse up to next real data
        start = str.find_first_not_of(delims, start);
        ++numSplits;

    } while (pos != std::string::npos);



    return ret;
}

void StrUtil::Split(const std::string &split_string, const std::string &splitter, std::vector<std::string> *split_result)
{
	size_t search_begin_pos = 0;
	size_t splitter_len = splitter.length();
	while (true)
	{
		size_t find_pos = split_string.find(splitter, search_begin_pos);
		if (find_pos == std::string::npos)
		{
			break;
		}

		split_result->push_back(split_string.substr(search_begin_pos, find_pos - search_begin_pos));
		search_begin_pos = find_pos + splitter_len;
	}

	if (search_begin_pos != split_string.length())
	{
		split_result->push_back(split_string.substr(search_begin_pos));
	}
}
//-----------------------------------------------------------------------
StrVec StrUtil::tokenise( const std::string& str, const std::string& singleDelims, const std::string& doubleDelims, unsigned int maxSplits)
{
    StrVec ret;
    // Pre-allocate some space for performance
    ret.reserve(maxSplits ? maxSplits+1 : 10);    // 10 is guessed capacity for most case

    unsigned int numSplits = 0;
	std::string delims = singleDelims + doubleDelims;

	// Use STL methods 
    size_t start, pos;
	char curDoubleDelim = 0;
    start = 0;
    do 
    {
		if (curDoubleDelim != 0)
		{
			pos = str.find(curDoubleDelim, start);
		}
		else
		{
			pos = str.find_first_of(delims, start);
		}

        if (pos == start)
        {
			char curDelim = str.at(pos);
			if (doubleDelims.find_first_of(curDelim) != std::string::npos)
			{
				curDoubleDelim = curDelim;
			}
            // Do nothing
            start = pos + 1;
        }
        else if (pos == std::string::npos || (maxSplits && numSplits == maxSplits))
        {
			if (curDoubleDelim != 0)
			{
				//Missing closer. Warn or throw exception?
			}
            // Copy the rest of the string
            ret.push_back( str.substr(start) );
            break;
        }
        else
        {
			if (curDoubleDelim != 0)
			{
				curDoubleDelim = 0;
			}

			// Copy up to delimiter
			ret.push_back( str.substr(start, pos - start) );
			start = pos + 1;
        }
		if (curDoubleDelim == 0)
		{
			// parse up to next real data
			start = str.find_first_not_of(singleDelims, start);
		}
            
        ++numSplits;

    } while (pos != std::string::npos);

    return ret;
}
//-----------------------------------------------------------------------
void StrUtil::toLowerCase(std::string& str)
{
    std::transform(
        str.begin(),
        str.end(),
        str.begin(),
		tolower);
}

//-----------------------------------------------------------------------
void StrUtil::toUpperCase(std::string& str) 
{
    std::transform(
        str.begin(),
        str.end(),
        str.begin(),
		toupper);
}
//-----------------------------------------------------------------------
bool StrUtil::startsWith(const std::string& str, const std::string& pattern, bool lowerCase)
{
    size_t thisLen = str.length();
    size_t patternLen = pattern.length();
    if (thisLen < patternLen || patternLen == 0)
        return false;

    std::string startOfThis = str.substr(0, patternLen);
    if (lowerCase)
        StrUtil::toLowerCase(startOfThis);

    return (startOfThis == pattern);
}
//-----------------------------------------------------------------------
bool StrUtil::endsWith(const std::string& str, const std::string& pattern, bool lowerCase)
{
    size_t thisLen = str.length();
    size_t patternLen = pattern.length();
    if (thisLen < patternLen || patternLen == 0)
        return false;

    std::string endOfThis = str.substr(thisLen - patternLen, patternLen);
    if (lowerCase)
        StrUtil::toLowerCase(endOfThis);

    return (endOfThis == pattern);
}
//-----------------------------------------------------------------------
std::string StrUtil::standardisePath(const std::string& init)
{
    std::string path = init;

    std::replace( path.begin(), path.end(), '\\', '/' );
    if( path[path.length() - 1] != '/' )
        path += '/';

    return path;
}
//-----------------------------------------------------------------------
std::string StrUtil::normalizeFilePath(const std::string& init, bool makeLowerCase)
{
	const char* bufferSrc = init.c_str();
	int pathLen = (int)init.size();
	int indexSrc = 0;
	int indexDst = 0;
	int metaPathArea = 0;

	char reservedBuf[1024];
	char* bufferDst = reservedBuf;
	bool isDestAllocated = false;
	if (pathLen > 1023)
	{
		//if source path is to long ensure we don't do a buffer overrun by allocating some
		//new memory
		isDestAllocated = true;
		bufferDst = new char[pathLen + 1];
	}

	//The outer loop loops over directories
	while (indexSrc < pathLen)
	{		
		if ((bufferSrc[indexSrc] == '\\') || (bufferSrc[indexSrc] == '/'))
		{
			//check if we have a directory delimiter if so skip it (we should already
			//have written such a delimiter by this point
			++indexSrc;
			continue;
		}
		else
		{
			//check if there is a directory to skip of type ".\"
			if ((bufferSrc[indexSrc] == '.') && 
				((bufferSrc[indexSrc + 1] == '\\') || (bufferSrc[indexSrc + 1] == '/')))
			{
				indexSrc += 2;
				continue;			
			}

			//check if there is a directory to skip of type "..\"
			else if ((bufferSrc[indexSrc] == '.') && (bufferSrc[indexSrc + 1] == '.') &&
				((bufferSrc[indexSrc + 2] == '\\') || (bufferSrc[indexSrc + 2] == '/')))
			{
				if (indexDst > metaPathArea)
				{
					//skip a directory backward in the destination path
					do {
						--indexDst;
					}
					while ((indexDst > metaPathArea) && (bufferDst[indexDst - 1] != '\\'));
					indexSrc += 3;
					continue;
				}
				else
				{
					//we are about to write "..\" to the destination buffer
					//ensure we will not remove this in future "skip directories"
					metaPathArea += 3;
				}
			}
		}

		//transfer the current directory name from the source to the destination
		while (indexSrc < pathLen)
		{
			char curChar = bufferSrc[indexSrc];
			if (makeLowerCase) curChar = tolower(curChar);
			if (curChar == '\\') curChar = '/';
			bufferDst[indexDst] = curChar;
			++indexDst;
			++indexSrc;
			if (curChar == '/') break;
		}
	}
	bufferDst[indexDst] = 0;

	std::string normalized(bufferDst); 
	if (isDestAllocated)
	{
		delete[] bufferDst;
	}

	return normalized;		
}
//-----------------------------------------------------------------------
void StrUtil::splitFilename(const std::string& qualifiedName, 
    std::string& outBasename, std::string& outPath)
{
    std::string path = qualifiedName;
    // Replace \ with / first
    std::replace( path.begin(), path.end(), '\\', '/' );
    // split based on final /
    size_t i = path.find_last_of('/');

    if (i == std::string::npos)
    {
        outPath.clear();
		outBasename = qualifiedName;
    }
    else
    {
        outBasename = path.substr(i+1, path.size() - i - 1);
        outPath = path.substr(0, i+1);
    }

}
//-----------------------------------------------------------------------
void StrUtil::splitBaseFilename(const std::string& fullName, 
	std::string& outBasename, std::string& outExtention)
{
	size_t i = fullName.find_last_of(".");
	if (i == std::string::npos)
	{
		outExtention.clear();
		outBasename = fullName;
	}
	else
	{
		outExtention = fullName.substr(i+1);
		outBasename = fullName.substr(0, i);
	}
}
// ----------------------------------------------------------------------------------------------------------------------------------------------
void StrUtil::splitFullFilename(	const std::string& qualifiedName, 
	std::string& outBasename, std::string& outExtention, std::string& outPath )
{
	std::string fullName;
	splitFilename( qualifiedName, fullName, outPath );
	splitBaseFilename( fullName, outBasename, outExtention );
}
//-----------------------------------------------------------------------
std::string StrUtil::combinePath( const std::string& path0, const std::string& path1 )
{
	std::string tmp_path0 = path0;
	std::string tmp_path1 = path1;
	StrUtil::trim(tmp_path0);
	StrUtil::trim(tmp_path1);
	if(tmp_path0.empty())
	{
		return tmp_path1;
	}
	else
	{
		if(!StrUtil::endsWith(tmp_path0, "\\") && !StrUtil::endsWith(tmp_path0, "/"))
		{
			tmp_path0 += "/";
		}

		if(StrUtil::startsWith(tmp_path1, "\\") || StrUtil::startsWith(tmp_path1, "/"))
		{
			tmp_path1 = tmp_path1.substr(1);
		}
		return tmp_path0 + tmp_path1;
	}
}
//-----------------------------------------------------------------------
bool StrUtil::match(const std::string& str, const std::string& pattern, bool caseSensitive)
{
    std::string tmpStr = str;
	std::string tmpPattern = pattern;
    if (!caseSensitive)
    {
        StrUtil::toLowerCase(tmpStr);
        StrUtil::toLowerCase(tmpPattern);
    }

    std::string::const_iterator strIt = tmpStr.begin();
    std::string::const_iterator patIt = tmpPattern.begin();
	std::string::const_iterator lastWildCardIt = tmpPattern.end();
    while (strIt != tmpStr.end() && patIt != tmpPattern.end())
    {
        if (*patIt == '*')
        {
			lastWildCardIt = patIt;
            // Skip over looking for next character
            ++patIt;
            if (patIt == tmpPattern.end())
			{
				// Skip right to the end since * matches the entire rest of the string
				strIt = tmpStr.end();
			}
			else
            {
				// scan until we find next pattern character
                while(strIt != tmpStr.end() && *strIt != *patIt)
                    ++strIt;
            }
        }
        else
        {
            if (*patIt != *strIt)
            {
				if (lastWildCardIt != tmpPattern.end())
				{
					// The last wildcard can match this incorrect sequence
					// rewind pattern to wildcard and keep searching
					patIt = lastWildCardIt;
					lastWildCardIt = tmpPattern.end();
				}
				else
				{
					// no wildwards left
					return false;
				}
            }
            else
            {
                ++patIt;
                ++strIt;
            }
        }

    }
	// If we reached the end of both the pattern and the string, we succeeded
	if (patIt == tmpPattern.end() && strIt == tmpStr.end())
	{
        return true;
	}
	else
	{
		return false;
	}

}
//-----------------------------------------------------------------------
const std::string StrUtil::replaceAll(const std::string& source, const std::string& replaceWhat, const std::string& replaceWithWhat)
{
	std::string result = source;
    std::string::size_type pos = 0;
	while(1)
	{
		pos = result.find(replaceWhat,pos);
		if (pos == std::string::npos) break;
		result.replace(pos,replaceWhat.size(),replaceWithWhat);
        pos += replaceWithWhat.size();
	}
	return result;
}

std::string StrUtil::join( const StrVec &join_src, const std::string &joiner )
{
	std::string join_result = "";
	for (StrVec::const_iterator iter = join_src.begin(); iter != join_src.end(); ++iter)
	{
		if (join_result.length() == 0)
		{
			join_result = *iter;
			continue;
		}

		join_result += (joiner + *iter);
	}

	return join_result;
}

NS_CC_END