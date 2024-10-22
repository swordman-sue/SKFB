#include "StdAfx.h"
#include <map>
#include <errno.h>
#include "DBCFile.h"

#define EMPTY_STRING	""

// 一行最大长度
const int LINE_BUFFER_SIZE = 10240;

// 域类型的字符串形式
const char* FIELD_TYPE_INT_STR = "INT";
const char* FIELD_TYPE_FLOAT_STR = "FLOAT";
const char* FIELD_TYPE_STRING_STR = "STRING";

// 域类型的长度
const int FIELD_TYPE_INT_LEN = 4;
const int FIELD_TYPE_FLOAT_LEN = 4;
#if defined(_M_IX86)
const int FIELD_TYPE_STRING_LEN = 4;
#elif defined(_M_X64)
const int FIELD_TYPE_STRING_LEN = 8;
#else
#error "Can not supported on this architecture."
#endif

// 校验字符
const char FIELD_TYPE_INT_CHAR = 'i';
const char FIELD_TYPE_FLOAT_CHAR = 'f';
const char FIELD_TYPE_STRING_CHAR = 's';

// emptyFieldIsError 表示检查到空的域则出错
bool DBCFile::ParseLine(FIELD_DATA_STRING& fields, const char* lineBuffer, char separator, bool emptyFieldIsError /* = false */)
{
	fields.clear();
	std::string line = lineBuffer;
	_ASSERT(!line.empty());
	if (line.empty())
		return false;

	std::string::size_type left = 0;
	std::string::size_type right = line.find_first_of(separator);

	// 是否只有一列
	if (right == std::string::npos)
		right = line.length();

	while (true)
	{
		std::string field = line.substr(left, right - left);
		if (field.length() == 0 && emptyFieldIsError)
			return false;

		// 消除多余的 " 字符
		if (field[0] == '"')
		{
			_ASSERT(field[field.length() - 1] == '"');

			const int MAX_BUFFER_SIZE = 8192;
			char buffer[MAX_BUFFER_SIZE];
			_ASSERT(field.length() < MAX_BUFFER_SIZE);
			if (field.length() >= MAX_BUFFER_SIZE)
				fields.push_back("");
			else
			{
				char* f = buffer;
				for (size_t i=0; i<field.length(); ++i)
				{
					if (i == 0 || i == field.length() - 1)
						continue;
					if (field[i] == '"')
						++i;
					*(f++) = field[i];
				}
				*f = 0;
				fields.push_back(buffer);
			}
		}
		else
			fields.push_back(field);

		if (right == line.length())
			break;

		left = right + 1;
		right = line.substr(left).find_first_of(separator);
		if (right != std::string::npos)
			right += left;
		else
			right = line.length();
	}

	return true;
}

// 从文件的内存镜像中读取一行，返回值为读取后指向内存的位置
const char* DBCFile::ReadLine(char* lineBuffer, int bufferSize, const char* fileImageBegin, const char* fileImageEnd, char separator)
{
	_ASSERT(lineBuffer);
	_ASSERT(fileImageBegin);
	_ASSERT(fileImageEnd);
	if (lineBuffer == NULL || fileImageBegin == NULL || fileImageEnd == NULL)
		return NULL;

	const char* currPos = fileImageBegin;
	if (currPos >= fileImageEnd || *currPos == 0)
		return 0;

	bool quote = false;
	while (true)
	{
		if (currPos >= fileImageEnd)
			break;
		if (currPos - fileImageBegin + 1 /* for '\0' */ >= bufferSize)
			break;
		if (*currPos == 0)
			break;
		if (!quote && (*currPos == '\r' || *currPos == '\n'))
			break;

		if (*currPos == '"')
		{
			if (currPos - 1 < fileImageBegin || *(currPos - 1) == separator)
				quote = true;
			else if (currPos + 1 == fileImageEnd
				|| *(currPos + 1) == separator || *(currPos + 1) == '\r'
				|| *(currPos + 1) == '\n'|| *(currPos + 1) == 0)
				quote = false;
		}

		*(lineBuffer++) = *(currPos++);
	}

	_ASSERT(currPos - fileImageBegin + 1 < bufferSize);
	if (currPos - fileImageBegin + 1 >= bufferSize)
		return 0;

	*lineBuffer = 0;

	// skip all next \r and \n
	while (currPos < fileImageEnd && *currPos != 0 && (*currPos=='\r' || *currPos=='\n'))
		currPos++;

	return currPos;
}

bool DBCFile::CheckType(const FIELD_TYPE_VEC& fieldType, const char* typeFormat)
{
	// 为空表示不做检查
	if (typeFormat == NULL || *typeFormat == 0)
		return true;

	size_t typeFormatLen = strlen(typeFormat);
	_ASSERT(typeFormatLen == fieldType.size());
	if (typeFormatLen != fieldType.size())
	{
		m_lastError.Format("DBC 文件域类型和程序中域类型定义个数不匹配，文件中读出列数为 %d，域类型字符串中列数为 %d（FileName=%s）",
			fieldType.size(), typeFormatLen, m_fileName.c_str());
		return false;
	}

	for (size_t i=0; i<typeFormatLen; ++i)
	{
		switch (fieldType[i])
		{
		case FIELD_TYPE_INT:
			if (typeFormat[i] != FIELD_TYPE_INT_CHAR)
			{
				m_lastError.Format("DBC 文件第 %d 列为 %c 类型，程序中此列域类型为 %c（FileName=%s）",
					i, FIELD_TYPE_INT_CHAR, typeFormat[i], m_fileName.c_str());
				return false;
			}
			break;
		case FIELD_TYPE_FLOAT:
			if (typeFormat[i] != FIELD_TYPE_FLOAT_CHAR)
			{
				m_lastError.Format("DBC 文件第 %d 列为 %c 类型，程序中此列域类型为 %c（FileName=%s）",
					i, FIELD_TYPE_FLOAT_CHAR, typeFormat[i], m_fileName.c_str());
				return false;
			}
			break;
		case FIELD_TYPE_STRING:
			if (typeFormat[i] != FIELD_TYPE_STRING_CHAR)
			{
				m_lastError.Format("DBC 文件第 %d 列为 %c 类型，程序中此列域类型为 %c（FileName=%s）",
					i, FIELD_TYPE_STRING_CHAR, typeFormat[i], m_fileName.c_str());
				return false;
			}
			break;
		default:
			_ASSERT(!"未知类型");
			return false;
		}
	}

	return true;
}

char* DBCFile::SaveString(const char* pSrc)
{
	wchar_t		wcTemp[512];
	char*		pDes;

	if (!pSrc)
		return EMPTY_STRING;

	// ANSI 2 Unicode
	int dwNum = MultiByteToWideChar (CP_ACP, 0, pSrc, -1, 0, 0);
	if (dwNum > 512)
		return EMPTY_STRING;

	memset(&wcTemp, 0, sizeof(wcTemp));
	if (MultiByteToWideChar(CP_ACP, 0, pSrc, -1, wcTemp, dwNum) < 0)
		return EMPTY_STRING;

	//Unicode 2 Utf-8
	dwNum = WideCharToMultiByte(CP_UTF8, 0, wcTemp ,-1 ,0 ,0 ,0 ,0);
	pDes = new char[dwNum];
	if (!pDes)
		return NULL;

	memset(pDes, 0, dwNum*sizeof(char));
	WideCharToMultiByte(CP_UTF8, 0, wcTemp, -1 , pDes, dwNum, 0, 0);

	return pDes;
}

DBCFile::DBCFile()
{
	m_recordSize = 0;
	m_recordNum = 0;
}

DBCFile::~DBCFile()
{
	for (int i=0; i<this->GetRecordNum(); ++i)
		this->ReleaseRecord(this->GetRecordByPos(i));
}

// typeForamt 形如 "iiisif"
bool DBCFile::ProcessFileImage(const char* fileImage, const char* fileImageEnd, const char* typeFormat, const char* fileName)
{
	_ASSERT(fileImage);
	_ASSERT(fileImageEnd);
	_ASSERT(fileName);
	if (!fileImage || !fileImageEnd || !fileName)
		return false;

	char lineBuffer[LINE_BUFFER_SIZE];

	// 首先构建数据类型信息
	// 首行必须用于描述数据类型信息
	const char* currFileImagePos = fileImage;
	currFileImagePos = DBCFile::ReadLine(lineBuffer, LINE_BUFFER_SIZE, currFileImagePos, fileImageEnd, '\t');
	_ASSERT(currFileImagePos);
	if (!currFileImagePos)
	{
		m_lastError.Format("读取 DBC 文件首行记录失败，可能此记录过长（MaxSize=%d bytes; FileName=%s）",
			LINE_BUFFER_SIZE, fileName);
		return false;
	}

	FIELD_DATA_STRING strField;
	if (!DBCFile::ParseLine(strField, lineBuffer, '\t', true))
	{
		m_lastError.Format("DBC 文件首行不能为空或者出现空字段（FileName=%s）", fileName);
		return false;
	}
	_ASSERT(0 < strField.size());
	if (0 >= strField.size())
	{
		m_lastError.Format("DBC 解析器内部错误，索引列定义错误");
		return false;
	}

	if (strField[0] != FIELD_TYPE_INT_STR)
	{
		m_lastError.Format("DBC 文件域类型错误，文件第 0 列为索引列，其类型必须为 \"%s\"（FileName=%s），注意，文件请勿使用 Unicode 编码",
			FIELD_TYPE_INT_STR, fileName);
		return false;
	}

	m_fieldType.resize(strField.size());
	m_recordSize = 0;
	for (int i=0; i<(int)m_fieldType.size(); ++i) 
	{
		if (strField[i] == FIELD_TYPE_INT_STR)
		{
			m_fieldType[i] = FIELD_TYPE_INT;
			m_recordSize += FIELD_TYPE_INT_LEN;
		}
		else if (strField[i] == FIELD_TYPE_FLOAT_STR)
		{
			m_fieldType[i] = FIELD_TYPE_FLOAT;
			m_recordSize += FIELD_TYPE_FLOAT_LEN;
		}
		else if (strField[i] == FIELD_TYPE_STRING_STR)
		{
			m_fieldType[i] = FIELD_TYPE_STRING;
			m_recordSize += FIELD_TYPE_STRING_LEN;
		}
		else
		{
			m_lastError.Format("DBC 文件发现到未定义的域数据类型（%s），目前支持的域数据类型为 %s %s %s（FileName=%s）",
				strField[i].c_str(), FIELD_TYPE_INT_STR, FIELD_TYPE_FLOAT_STR, FIELD_TYPE_STRING_STR, fileName);
			return false;
		}
	}

	if (!DBCFile::CheckType(m_fieldType, typeFormat))
		return false;

	m_fileHeadInfo = lineBuffer;

	// 跳过第二行
	currFileImagePos = DBCFile::ReadLine(lineBuffer, LINE_BUFFER_SIZE, currFileImagePos, fileImageEnd, '\t');
	_ASSERT(currFileImagePos);
	if (!currFileImagePos)
	{
		m_lastError.Format("描述域信息的行为空，请填写（FileName=%s）", fileName);
		return false;
	}

	m_fileHeadInfo += "\n";
	m_fileHeadInfo += lineBuffer;
	m_fileHeadInfo += "\n";

	// 开始读取数据
	m_recordNum = 0;
	int fieldNum = (int)m_fieldType.size();
	do
	{
		currFileImagePos = DBCFile::ReadLine(lineBuffer, LINE_BUFFER_SIZE, currFileImagePos, fileImageEnd, '\t');
		if (!currFileImagePos)
			break;

		// 跳过注释行
		if (lineBuffer[0] == '#')
			continue;

		if (!DBCFile::ParseLine(strField, lineBuffer, '\t'))
		{
			_ASSERT(!"DBC 文件中存在空行，跳过此行");
			continue;
		}
		int nLen = (int)strField.size();
		_ASSERT( nLen<= fieldNum);
		if ((int)strField.size() < fieldNum) 
		{
			// 补上空格
			int subNum = fieldNum - (int)strField.size();
			for (int i=0; i<subNum; ++i)
				strField.push_back("");
		}

		// 索引列不能为空
		if (strField[0].empty())
		{
			_ASSERT(!"DBC 文件中索引列为空，跳过此行");
			continue;
		}

		for (int i=0; i<fieldNum; ++i)
		{
			size_t len = m_field.size();
			switch (m_fieldType[i])
			{
			case FIELD_TYPE_INT:
				{
					m_field.resize(len + FIELD_TYPE_INT_LEN);
					int intValue = atoi(strField[i].c_str());
					memcpy(&m_field[len], &intValue, FIELD_TYPE_INT_LEN);
				}
				break;
			case FIELD_TYPE_FLOAT:
				{
					m_field.resize(len + FIELD_TYPE_FLOAT_LEN);
					float floatValue = (float)atof(strField[i].c_str());
					memcpy(&m_field[len], &floatValue, FIELD_TYPE_FLOAT_LEN);
				}
				break;
			case FIELD_TYPE_STRING:
				{
					//size_t newFieldLen = strField[i].length() + 1;
					//char* newField = new char[newFieldLen];
					//memcpy(newField, strField[i].c_str(), newFieldLen);
					char* newField = SaveString(strField[i].c_str());
					
					m_field.resize(len + FIELD_TYPE_STRING_LEN);
					memcpy(&m_field[len], &newField, FIELD_TYPE_STRING_LEN);
				}
				break;
			}
		}

		++m_recordNum;

	} while(true);

	// 生成索引列
	return this->CreateIndex();
}

bool DBCFile::Load(const char* fileName, const char* typeFormat /* = NULL */)
{
	_ASSERT(m_fileName.empty());
	if (!m_fileName.empty())
		return false;
	_ASSERT(fileName != NULL);
	if (!fileName)
		return false;
	m_fileName = fileName;
	if (typeFormat != NULL)
		m_typeFormat = typeFormat;

	FILE* file = fopen(fileName, "rb");
	_ASSERT(file != NULL);
	if (!file)
	{
		m_lastError.Format("无法打开 DBC 文件 %s", fileName);
		return false;
	}

	// 将文件载入内存
	fseek(file, 0, SEEK_END);
	const int FILE_SIZE = ftell(file);
	_ASSERT(FILE_SIZE != 0);
	fseek(file, 0, SEEK_SET);

	char* fileImage = new char[FILE_SIZE + 1];
	fread(fileImage, 1, FILE_SIZE, file);
	fileImage[FILE_SIZE] = 0;

	// 处理文件的内存镜像
	bool success = ProcessFileImage(fileImage, fileImage + FILE_SIZE + 1, typeFormat, fileName);

	delete[] fileImage;
	fileImage = 0;

	fclose(file);

	return success;
}

bool DBCFile::Save()
{
	FILE* newFile = fopen(m_fileName.c_str(), "wb");
	_ASSERT(newFile);
	if (!newFile)
	{
		m_lastError.Format("保存文件失败，文件无法打开 %s（FileName=%s）",
			strerror(errno), m_fileName.c_str());
		return false;
	}

	fprintf(newFile, "%s", m_fileHeadInfo.c_str());

	for (int i=0; i<m_recordNum; ++i)
	{
		char* record = this->GetRecordByPos(i);
		_ASSERT(record);
		if (!record)
			continue;

		// 索引列表中不存在的记录不写文件
		int index;
		_ASSERT(this->GetIndex(record, index));
		if (!this->GetIndex(record, index))
			continue;
		_ASSERT(m_fieldIndex.find(index) != m_fieldIndex.end());
		if (m_fieldIndex.find(index) == m_fieldIndex.end())
			continue;

		// 开始写文件
		int fieldPos = 0;
		for (int j=0; j<(int)m_fieldType.size(); ++j)
		{
			char* curField = record + fieldPos;
			switch (m_fieldType[j])
			{
			case FIELD_TYPE_INT:
				fprintf(newFile, "%d", *(int*)curField);
				fieldPos += FIELD_TYPE_INT_LEN;
				break;
			case FIELD_TYPE_FLOAT:
				fprintf(newFile, "%f", *(float*)curField);
				fieldPos += FIELD_TYPE_FLOAT_LEN;
				break;
			case FIELD_TYPE_STRING:
				if (*(char**)curField == NULL)
					fprintf(newFile, "");
				else
				{
					bool quote = false;
					for (char* i=*(char**)curField; *i!='\0'; ++i)
					{
						if (*i == '\t' || *i == '\r' || *i == '\n' || *i == '"')
						{
							quote = true;
							break;
						}
					}

					for (char* i=*(char**)curField; *i!='\0'; ++i)
					{
						if (i == *(char**)curField && quote)
							fprintf(newFile, "\"");
						if (*i == '"')
							fprintf(newFile, "\"");
						fprintf(newFile, "%c", *i);
					}
					if (quote)
						fprintf(newFile, "\"");
				}
				fieldPos += FIELD_TYPE_STRING_LEN;
				break;
			default:
				break;
			}

			if (j != (int)m_fieldType.size() - 1)
				fprintf(newFile, "\t");
			else
				fprintf(newFile, "\r\n");
		}
	}

	fclose(newFile);

	return true;
}

bool DBCFile::CreateIndex()
{
	m_fieldIndex.clear();

	for (int i=0; i<(int)m_recordNum; ++i)
	{
		// 获取索引值
		char* record = &(m_field[i * m_recordSize]);
		int index;
		_ASSERT(this->GetIndex(record, index));
		if (!this->GetIndex(record, index))
		{
			m_lastError.Format("DBC 文件索引构建失败（FileName=%s）",
				index, m_fileName.c_str());
			return false;
		}

		// 检查索引值是否重复
		FILED_INDEX::iterator it = m_fieldIndex.find(index);
		if (it != m_fieldIndex.end())
		{
			m_lastError.Format("DBC 文件索引重复，构建索引失败，冲突的索引值为 %d（FileName=%s）",
				index, m_fileName.c_str());
			return false;
		}
		m_fieldIndex.insert(std::make_pair(index, record));
	}

	return true;
}

bool DBCFile::GetIndex(const char* record, int& index) const
{
	_ASSERT(record);
	if (!record)
		return false;
	index = *(int*)record;
	return true;
}

const char* DBCFile::GetRecord(int index) const
{
	FILED_INDEX::const_iterator it = m_fieldIndex.find(index);
	if (it == m_fieldIndex.end())
		return NULL;
	return it->second;
}

char* DBCFile::GetModifiableRecord(int index)
{
	FILED_INDEX::iterator it = m_fieldIndex.find(index);
	if (it == m_fieldIndex.end())
		return NULL;
	return it->second;
}

const char* DBCFile::IterateRecord(int pos) const
{
	return this->GetRecordByPos(pos);
}

bool DBCFile::SetRecord(const char* record)
{
	if (!record)
		return false;
	int index;
	_ASSERT(this->GetIndex(record, index));
	if (!this->GetIndex(record, index))
		return false;

	char* oldRecord = this->GetModifiableRecord(index);
	if (oldRecord)
		return this->ModifyRecord(record);
	else
		return this->InsertRecord(record);
}

bool DBCFile::ModifyRecord(const char* record)
{
	if (!record)
		return false;
	int index;
	_ASSERT(this->GetIndex(record, index));
	if (!this->GetIndex(record, index))
		return false;
	char* oldRecord = this->GetModifiableRecord(index);
	if (!oldRecord)
		return false;

	int fieldPos = 0;
	for (int i=0; i<(int)m_fieldType.size(); ++i)
	{
		switch (m_fieldType[i])
		{
		case FIELD_TYPE_INT:
			memcpy(oldRecord + fieldPos, record + fieldPos, FIELD_TYPE_INT_LEN);
			fieldPos += FIELD_TYPE_INT_LEN;
			break;
		case FIELD_TYPE_FLOAT:
			memcpy(oldRecord + fieldPos, record + fieldPos, FIELD_TYPE_FLOAT_LEN);
			fieldPos += FIELD_TYPE_FLOAT_LEN;
			break;
		case FIELD_TYPE_STRING:
			{
				size_t len = 1;
				const char* str = *((char**)(record + fieldPos));
				if (str != NULL)
					len = strlen(str) + 1;
				char* newStr = new char[len];

				if (str != NULL)
					memcpy(newStr, str, len);
				else
					newStr[0] = 0;
				delete[] *((char**)(oldRecord + fieldPos));
				memcpy(oldRecord + fieldPos, &newStr, FIELD_TYPE_STRING_LEN);

				fieldPos += FIELD_TYPE_STRING_LEN;
			}
			break;
		default:
			break;
		}
	}
	
	return true;
}

bool DBCFile::InsertRecord(const char* record)
{
	if (!record)
		return false;
	int index;
	_ASSERT(this->GetIndex(record, index));
	if (!this->GetIndex(record, index))
		return false;
	if (this->GetRecord(index))
		return false;

	int fieldPos = 0;
	for (int i=0; i<(int)m_fieldType.size(); ++i)
	{
		size_t len = m_field.size();
		switch (m_fieldType[i])
		{
		case FIELD_TYPE_INT:
			m_field.resize(len + FIELD_TYPE_INT_LEN);
			memcpy(&m_field[len], record + fieldPos, FIELD_TYPE_INT_LEN);
			fieldPos += FIELD_TYPE_INT_LEN;
			break;
		case FIELD_TYPE_FLOAT:
			m_field.resize(len + FIELD_TYPE_FLOAT_LEN);
			memcpy(&m_field[len], record + fieldPos, FIELD_TYPE_FLOAT_LEN);
			fieldPos += FIELD_TYPE_FLOAT_LEN;
			break;
		case FIELD_TYPE_STRING:
			{
				m_field.resize(len + FIELD_TYPE_STRING_LEN);

				size_t strLen = 1;
				const char* str = *((char**)(record + fieldPos));
				if (str != NULL)
					strLen = strlen(str) + 1;
				char* newStr = new char[strLen];

				if (str != NULL)
					memcpy(newStr, str, strLen);
				else
					newStr[0] = 0;
				memcpy(&m_field[len], &newStr, FIELD_TYPE_STRING_LEN);

				fieldPos += FIELD_TYPE_STRING_LEN;
			}
			break;
		default:
			break;
		}
	}

	++m_recordNum;
	return this->CreateIndex();
}


bool DBCFile::DeleteRecord(int index)
{
	FILED_INDEX::iterator it = m_fieldIndex.find(index);
	if (it == m_fieldIndex.end())
		return false;

	char* memoryBegin = it->second;
	char* memoryEnd = memoryBegin + m_recordSize - 1;
	FIELD_DATA::iterator itDataBegin = m_field.begin();
	FIELD_DATA::iterator itDataEnd = itDataBegin;
	FIELD_DATA::iterator itData = itDataBegin;
	for (; itData != m_field.end(); ++itData)
	{
		char* dataAddr = &(*itData);
		if (dataAddr == memoryBegin)
			itDataBegin = itData;
		else if (dataAddr == memoryEnd)
			itDataEnd = itData;
	}

	_ASSERT(itDataEnd != m_field.begin());
	_ASSERT(itDataEnd != m_field.end());
	if (itDataEnd != m_field.begin() && itDataEnd != m_field.end())
	{
		this->ReleaseRecord(memoryBegin);
		m_field.erase(itDataBegin, itDataEnd + 1);
		m_fieldIndex.erase(it);
		--m_recordNum;
		return this->CreateIndex();
	}

	return false;
}

const char* DBCFile::GetRecordByPos(int recordPos) const
{
	int pos = recordPos * m_recordSize;

	if (pos < 0 || pos >= (int)m_field.size()) 
	{
		_ASSERT(!"查找域时出错");
		return NULL;
	}

	return &(m_field[pos]);
}

char* DBCFile::GetRecordByPos(int recordPos)
{
	int pos = recordPos * m_recordSize;

	if (pos < 0 || pos >= (int)m_field.size()) 
	{
		_ASSERT(!"查找域时出错");
		return NULL;
	}

	return &(m_field[pos]);
}

void DBCFile::ReleaseRecord(char* record)
{
	if (!record)
		return;

	int fieldPos = 0;
	for (int i=0; i<(int)m_fieldType.size(); ++i)
	{
		if (m_fieldType[i] == FIELD_TYPE_INT)
			fieldPos += FIELD_TYPE_INT_LEN;
		else if (m_fieldType[i] == FIELD_TYPE_FLOAT)
			fieldPos += FIELD_TYPE_FLOAT_LEN;
		else if (m_fieldType[i] == FIELD_TYPE_STRING)
		{
			delete[] *((char**)(record + fieldPos));
			fieldPos += FIELD_TYPE_STRING_LEN;
		}
	}
}