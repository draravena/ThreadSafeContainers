#pragma once

#include "threadSafeVector.hpp"
 
namespace datatype {
	template <typename T>
	ThreadSafeVector<T>::ThreadSafeVector(uint64_t maxSize, vc_t CONFIG) : resizeable_(false), CONFIG_(CONFIG) {
		init(maxSize, CONFIG);
	}
	template <typename T>
	ThreadSafeVector<T>::ThreadSafeVector(vc_t CONFIG) : resizeable_(true) {

	}
	template <typename T>
	void ThreadSafeVector<T>::init(uint64_t maxSize, vc_t CONFIG) {
		if (CONFIG & vc_t::RESERVE_MAX_SIZE) {
			if (maxSize >= getTotalRAM()) {
				throw std::runtime_error("Allocation size exceeds total memory.");
			}
			vector_.reserve(maxSize);
		}
	}

	// GETTERS
	template <typename T>
	uint64_t ThreadSafeVector<T>::getTotalRAM() {	
		MEMORYSTATUSEX msx = getMemStatusEX();
		return msx.ullTotalPhys;
	}
	template <typename T>
	uint64_t ThreadSafeVector<T>::getFreeRAM() {
		MEMORYSTATUSEX msx = getMemStatusEX();
		return msx.ullAvailPhys;
	}
	template <typename T>
	MEMORYSTATUSEX ThreadSafeVector<T>::getMemStatusEX() {
		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);
		return memInfo;
	}
	template <typename T>
	bool ThreadSafeVector<T>::resizeable() {
		return resizeable_;
	}
	template <typename T>
	uint64_t ThreadSafeVector<T>::size() {
		return size_;
	}	
	template <typename T>
	uint64_t ThreadSafeVector<T>::maxSize() {
		return maxSize_;
	}
	template <typename T>
	uint64_t ThreadSafeVector<T>::remaining() {
		return maxSize_ - size_;
	}
	template <typename T>
	T* ThreadSafeVector<T>::data() {
		return vector_.data();
	}
};