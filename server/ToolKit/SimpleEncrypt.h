#pragma once

class SimpleEncrypt
{
public:

	SimpleEncrypt(void)
	{
	}

	~SimpleEncrypt(void)
	{
	}

	static void Encrypt(CHAR* pData,DWORD dwSize,CHAR* pEncryptKey,DWORD dwBeginPlace)
	{
		if(pData!=NULL ) 
		{ 

			CHAR* t_pBuffer = pData;
			CHAR* pKey = pEncryptKey;
			UINT KeyLen = (UINT)strlen(pKey);
			for (UINT i = 0; i < dwSize; i++)
			{ 
				*t_pBuffer ^= pKey[(i+dwBeginPlace)%KeyLen];
				t_pBuffer++;
			} 
		}
	}
};
