#pragma once

#include "threadSafeVector.hpp"
 
namespace datatype {
	template <typename T>
	ThreadSafeVector<T>::ThreadSafeVector(uint64_t maxSize, vc_t CONFIG) : resizeable_(false) {
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

	template <typename T>
	T& ThreadSafeVector<T>::operator[](uint64_t index) {
		return OVERLOAD_set(index);
	}

	template <typename T>
	T& ThreadSafeVector<T>::OVERLOAD_set(uint64_t index) {
		if (index >= maxSize_) {
			throwException("Out of bounds. Larger than max size");
		}
		if (index >= vector_.size()) {
			throwException("Out of bounds. Larger than vector size.");
		}
		return OVERLOAD_condition_set(index);
		// IF DANGEROUS, OP WITHOUT MUTEX
		if (isConfigured(vc_t::DANGEROUS)) {
			return vector_[index];
		}
		// IF GLOBAL MUTEX ONLY AND NO TIMEOUT
		if (isConfigured(vc_t::ONLY_GLOBAL_MUTEX) && isNOTConfigured(vc_t::ENABLE_MUTEX_TIMEOUT)) {
			std::lock_guard<std::mutex> lock(globalmtx_);
			return vector_[index];
		}
		if (isConfigured(vc_t::ONLY_GLOBAL_MUTEX) && isConfigured(vc_t::ENABLE_MUTEX_TIMEOUT)) {
			if (global_timedmtx_.try_lock_for(timedMutexWaitDuration_)) {
				return vector_[index];;
			}
		}
		if (isConfigured(vc_t::ONLY_GLOBAL_MUTEX) && isConfigured(vc_t::ENABLE_MUTEX_TIMEOUT)) {

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