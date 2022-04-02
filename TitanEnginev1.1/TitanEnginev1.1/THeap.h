#pragma once
class THeap
{
public:
	THeap();
	~THeap();



private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDescriptorHeap;

};

