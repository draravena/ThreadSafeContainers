#pragma once
#include "threadSafeVector.hpp"

namespace datatype {
	template <typename T>
	ThreadSafeVector<T>::ThreadSafeVector(uint64_t maxSize, vector_config_t CONFIG) {
		init(maxSize, CONFIG);
	}
	template <typename T>
	void ThreadSafeVector<T>::init(uint64_t maxSize, vector_config_t CONFIG) {
		if (CONFIG & vector_config_t::RESERVE_MAX_SIZE) {
			if (maxSize >= getTotalRAM()) {
				throw std::runtime_error("Allocation size exceeds total memory.");
			}
			vector_.reserve(maxSize);
		}
	}
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
};