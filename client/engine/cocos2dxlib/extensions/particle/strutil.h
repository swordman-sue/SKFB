#ifndef STRUTIL_H
#define STRUTIL_H

#include "base/ccTypes.h"
#include <string>
#include <vector>

NS_CC_BEGIN
typedef std::string _StringBase;
typedef std::basic_stringstream<char,std::char_traits<char>,std::allocator<char> > _StringStreamBase;
////typedef _StringBase String;
typedef _StringStreamBase StringStream;
typedef StringStream stringstream;

typedef std::vector<std::string> StrVec;
class CC_DLL StrUtil
{
public:
	typedef StringStream StrStreamType;

    /** Removes any whitespace characters, be it standard space or
        TABs and so on.
        @remarks
            The user may specify wether they want to trim only the
            beginning or the end of the String ( the default action is
            to trim both).
    */
	static void trim( std::string& str, bool left = true, bool right = true );

    /** Returns a StringVector that contains all the substrings delimited
        by the characters in the passed <code>delims</code> argument.
        @param
            delims A list of delimiter characters to split by
        @param
            maxSplits The maximum number of splits to perform (0 for unlimited splits). If this
            parameters is > 0, the splitting process will stop after this many splits, left to right.
        @param
            preserveDelims Flag to determine if delimiters should be saved as substrings
    */
	static StrVec split( const std::string& str, const std::string& delims = "\t\n ", unsigned int maxSplits = 0, bool preserveDelims = false);

	static void Split(const std::string &split_string, const std::string &splitter, std::vector<std::string> *split_result);
	/** Returns a StringVector that contains all the substrings delimited
        by the characters in the passed <code>delims</code> argument, 
		or in the <code>doubleDelims</code> argument, which is used to include (normal) 
		delimeters in the tokenised string. For example, "strings like this".
        @param
            delims A list of delimiter characters to split by
		@param
            delims A list of double delimeters characters to tokenise by
        @param
            maxSplits The maximum number of splits to perform (0 for unlimited splits). If this
            parameters is > 0, the splitting process will stop after this many splits, left to right.
    */
	static StrVec tokenise( const std::string& str, const std::string& delims = "\t\n ", const std::string& doubleDelims = "\"", unsigned int maxSplits = 0);

	/** Lower-cases all the characters in the string.
    */
    static void toLowerCase( std::string& str );

    /** Upper-cases all the characters in the string.
    */
    static void toUpperCase( std::string& str );


    /** Returns whether the string begins with the pattern passed in.
    @param pattern The pattern to compare with.
    @param lowerCase If true, the start of the string will be lower cased before
        comparison, pattern should also be in lower case.
    */
    static bool startsWith(const std::string& str, const std::string& pattern, bool lowerCase = true);

    /** Returns whether the string ends with the pattern passed in.
    @param pattern The pattern to compare with.
    @param lowerCase If true, the end of the string will be lower cased before
        comparison, pattern should also be in lower case.
    */
    static bool endsWith(const std::string& str, const std::string& pattern, bool lowerCase = true);

    /** Method for standardising paths - use forward slashes only, end with slash.
    */
    static std::string standardisePath( const std::string &init);
	/** Returns a normalized version of a file path
	This method can be used to make file path strings which point to the same directory  
	but have different texts to be normalized to the same text. The function:
	- Transforms all backward slashes to forward slashes.
	- Removes repeating slashes.
	- Removes initial slashes from the beginning of the path.
	- Removes ".\" and "..\" meta directories.
	- Sets all characters to lowercase (if requested)
	@param init The file path to normalize.
	@param makeLowerCase If true, transforms all characters in the string to lowercase.
	*/
    static std::string normalizeFilePath(const std::string& init, bool makeLowerCase = true);


    /** Method for splitting a fully qualified filename into the base name
        and path.
    @remarks
        Path is standardised as in standardisePath
    */
    static void splitFilename(const std::string& qualifiedName,
        std::string& outBasename, std::string& outPath);

	/** Method for splitting a fully qualified filename into the base name,
	extension and path.
	@remarks
	Path is standardised as in standardisePath
	*/
	static void splitFullFilename(const std::string& qualifiedName, 
		std::string& outBasename, std::string& outExtention, 
		std::string& outPath);

	/** Method for splitting a filename into the base name
	and extension.
	*/
	static void splitBaseFilename(const std::string& fullName, 
		std::string& outBasename, std::string& outExtention);

	static std::string combinePath(const std::string& path0, const std::string& path1);

    /** Simple pattern-matching routine allowing a wildcard pattern.
    @param str String to test
    @param pattern Pattern to match against; can include simple '*' wildcards
    @param caseSensitive Whether the match is case sensitive or not
    */
    static bool match(const std::string& str, const std::string& pattern, bool caseSensitive = true);


	/** replace all instances of a sub-string with a another sub-string.
	@param source Source string
	@param replaceWhat Sub-string to find and replace
	@param replaceWithWhat Sub-string to replace with (the new sub-string)
	@returns An updated string with the sub-string replaced
	*/
	static const std::string replaceAll(const std::string& source, const std::string& replaceWhat, const std::string& replaceWithWhat);
		
	/*
	以某个连接符为标准，返回一个vector所有字符串字段连接结果

	@join_table 需要连接的所有字段，数据类型为vector
	@joiner 连接符
	@return 用连接符连接后的字符串
	*/
	static std::string join(const StrVec &join_src, const std::string &joiner);

    /// Constant blank string, useful for returning by ref where local does not exist
    static const std::string BLANK;
};
		
//--------------------------------------------------

	
NS_CC_END

#endif // DreamStringUtil_h__