#pragma once

#include <wtypes.h>

class BitSet
{
public:
	BitSet(DWORD valuesCount)
	{
		mCount = valuesCount;
		mBlocks = (valuesCount + 31) / 32;

		mBitSet = new DWORD[mBlocks];
		memset(mBitSet, 0, mBlocks << 2);
	}

	virtual ~BitSet( )
	{
		if(mBitSet)
			delete [] mBitSet;
	}

	void SetBit (DWORD index)
	{
		( (UCHAR *)mBitSet )[ index >> 3 ] |= 1 << ( index & 0x7 );
	}

	void UnsetBit (DWORD index)
	{
		( (UCHAR *)mBitSet )[ index >> 3 ] &= (0xff ^ (1 <<  ( index & 0x7 ) ) );
	}

	bool GetBit (DWORD index) const
	{
		return ( ( (UCHAR *)mBitSet)[ index >> 3 ] & ( 1 << ( index & 0x7 ) )) != 0;
	}

	DWORD GetBlockCount() const { return mBlocks; }
	DWORD GetLength() const { return mBlocks << 2; }
	DWORD GetCount() const { return mCount; }
	UCHAR* GetBitSet() { return (UCHAR*)mBitSet; }


	void FillData(const BYTE* pInBuff)
	{
		memcpy(mBitSet,pInBuff,mBlocks << 2);
	}

	void CopyData(BYTE* pOutBuff)
	{
		memcpy(pOutBuff,mBitSet,mBlocks << 2);
	}

	void Clear()
	{
		if (mBitSet)
			memset(mBitSet, 0, mBlocks << 2);
	}

private:
	DWORD mCount;
	DWORD mBlocks;
	DWORD *mBitSet;
};
