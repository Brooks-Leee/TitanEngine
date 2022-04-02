#pragma once
#include "THeap.h"

class THeapManager
{
public:
	THeapManager();
	~THeapManager();


	THeap* GetRtvHeap() { return mRtvHeap; }
	THeap* GetDsvHeap() { return mDsvHeap; }
	THeap* GetCbvSrvHeap() { return mCbvSrvHeap; }
	THeap* GetSrvHeap() { return mSrvheap; }

private:
	THeap* mRtvHeap;
	THeap* mDsvHeap;
	THeap* mCbvSrvHeap;
	THeap* mSrvheap;



};

