#pragma once
#ifndef FCTDEF_H
#define FCTDEF_H

#include <vector>
#include <winerror.h>
#include <comdef.h>

#define returnOnFail(hr) if(HRESULT h = hr; FAILED(h)) return h;
#define throwOnFail(hr) if(HRESULT h = hr; FAILED(h)) throw _com_error(h);

template <class T, class U>
size_t dichotomous_search(_In_ const std::vector<T>& list, _In_ U searched_value, _In_ U (*access_value)(T a), _In_opt_ size_t begin = 0, _In_opt_ size_t end = UINT64_MAX)
{
	if (end == UINT64_MAX) end = list.size();
	if (begin >= end) return begin;
	
	size_t mid = (begin + end) >> 1;

	if (searched_value >= access_value(list[mid])) // The searched value is more or equal than the current value
	{
		return dichotomous_search(list, searched_value, access_value, mid+1, end);
	}
	return dichotomous_search(list, searched_value, access_value, begin, mid-1);
}

template <class T>
void safeRelease(T* ppT)
{
	if (ppT)
	{
		ppT->Release();
		ppT = nullptr;
	}
}

#endif // FCTDEF_H