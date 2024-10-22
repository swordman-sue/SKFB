#pragma once

class RC4Encry
{
public:
	RC4Encry(void);
	~RC4Encry(void);

public:
	bool SetKey(const unsigned char* key, int keyLen);
	bool Encrypt(unsigned char* outData, const unsigned char* inData, int len);

private:
	struct RC4_KEY
	{
		unsigned int x;
		unsigned int y;
		unsigned int data[256];
	};

	RC4_KEY		m_RC4Key;
};
