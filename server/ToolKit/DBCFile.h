#pragma once

#include <vector>
#include <map>
#include <string>
#include <atlstr.h>

class DBCFile	
{
	// Data type
protected:
	enum FIELD_TYPE
	{
		FIELD_TYPE_INT,		// ����
		FIELD_TYPE_FLOAT,	// ������
		FIELD_TYPE_STRING	// �ַ���
	};

	typedef std::map<INT, char*> FILED_INDEX;
	typedef std::vector<char> FIELD_DATA;
	typedef std::vector<FIELD_TYPE>	FIELD_TYPE_VEC;
	typedef std::vector<std::string> FIELD_DATA_STRING;

	// Interface
public:
	DBCFile();
	virtual ~DBCFile();
	bool Load(const char* fileName, const char* typeFormat = NULL);
	bool Save();
	// FIELD* ��ʾһ����¼��FIELD �����ʾ��¼��
	// ���ڱ��������ռ�¼���ڴ��е�λ�ò��ң�pos �� 0 ��ʼ
	const char* IterateRecord(int pos) const;
	// ���������в��Ҽ�¼
	const char* GetRecord(int index) const;
	// ���� record ���������ж� record �Ƿ���ڣ�����������޸Ĳ�������������һ��
	bool SetRecord(const char* record);
	bool DeleteRecord(int index);
	int GetRecordNum() const { return m_recordNum; }
	const char* GetLastError() const { return m_lastError; }
	const char* GetFileName() const { return m_fileName.c_str(); }
	const char* GetTypeFormat() const { return m_typeFormat.c_str(); }

protected:
	static const char* ReadLine(char* lineBuffer, int bufferSize, const char* fileImageBegin, const char* fileImageEnd, char separator);
	static bool ParseLine(FIELD_DATA_STRING& fields, const char* lineBuffer, char separator, bool emptyFieldIsError = false);

	bool ProcessFileImage(const char* fileImage, const char* fileImageEnd, const char* typeFormat, const char* fileName);
	bool CreateIndex();
	bool GetIndex(const char* record, int& index) const;
	bool ModifyRecord(const char* record);
	bool InsertRecord(const char* record);
	char* GetModifiableRecord(int index);
	const char* GetRecordByPos(int recordPos) const;
	char* GetRecordByPos(int recordPos);
	void ReleaseRecord(char* record);
	bool CheckType(const FIELD_TYPE_VEC& fieldType, const char* typeFormat);

private:
	char* SaveString(const char* pSrc);

protected:
	FIELD_TYPE_VEC m_fieldType;
	int m_recordSize;

	FIELD_DATA m_field;
	int m_recordNum;
	FILED_INDEX m_fieldIndex;
	std::string m_fileHeadInfo;
	std::string m_fileName;
	std::string m_typeFormat;

	CStringA m_lastError;

private:
	DBCFile(const DBCFile&);
	void operator=(const DBCFile&);
};
