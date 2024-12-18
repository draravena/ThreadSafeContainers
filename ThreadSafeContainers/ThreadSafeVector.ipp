#pragma once

#include "threadSafeVector.hpp"
#include <iostream> // debug

namespace datatype {

	/*
		CONSTRUCTORS
	*/

	template <typename T>
	ThreadSafeVector<T>::ThreadSafeVector(uint64_t maxSize, const vc_t CONFIG) : resizeable_(false), CONFIG_(CONFIG) {
		init(maxSize, CONFIG);
	}
	template <typename T>
	ThreadSafeVector<T>::ThreadSafeVector(const vc_t CONFIG) : resizeable_(true) {
		static_assert(!((CONFIG & vc_t::RESERVE_MAX_SIZE) == 1), "Cannot reserve memory for vector with no max size set."); // DOES NOT WORK
		init(UINT64_MAX, CONFIG);
	}

	/*
		CONSTRUCTOR HELPER
	*/

	template <typename T>
	void ThreadSafeVector<T>::init(uint64_t maxSize, const vc_t CONFIG) {
		if (CONFIG & vc_t::RESERVE_MAX_SIZE) {
			if (maxSize >= getTotalRAM()) {
				throw std::runtime_error("Allocation size exceeds total memory.");
			}
			vector_.reserve(maxSize);
		}
	}

	/*
		VECTOR FUNCTIONS
	*/

	template <typename T>
	bool ThreadSafeVector<T>::push_back(T& item) {
		return _PUSH_BACK_HELPER(item);
	}

	template <typename T>
	bool ThreadSafeVector<T>::push_back(T&& item) {
		return _PUSH_BACK_HELPER(item);
	}

	template <typename T>
	bool ThreadSafeVector<T>::pop_back(uint64_t count) {
		return _POP_BACK_HELPER(count);
	}

	template <typename T>
	void ThreadSafeVector<T>::clear() {
		if (isConfigured(vc_t::DANGEROUS)) {
			vector_.clear();
			return;
		}
		// IMPLEMENT MUTEX TIMEOUT
		std::lock_guard<std::mutex> lock(globalmtx_);
		vector_.clear();
		return;
	}

	/*
		VECTOR FUNCTIONS HELPERS
	*/

	template <typename T>
	bool ThreadSafeVector<T>::_PUSH_BACK_HELPER(T&& item) {
		if (isConfigured(vc_t::DANGEROUS)) {
			vector_.push_back(item);
			return true;
		}
		if (isConfigured(vc_t::ONLY_GLOBAL_MUTEX) && isConfigured(vc_t::ENABLE_MUTEX_TIMEOUT)) {

		}
		if (isConfigured(vc_t::ONLY_GLOBAL_MUTEX) && isNOTConfigured(vc_t::ENABLE_MUTEX_TIMEOUT)) {
			std::lock_guard<std::mutex> lock(*globalmtx_);
			vector_.push_back(item);
		}
	}

	template <typename T>
	bool ThreadSafeVector<T>::_POP_BACK_HELPER(uint64_t count) {
		if (isConfigured(vc_t::DANGEROUS)) {
			vector_.pop_back();
			return true;
		}
		// IMPLEMENT MUTEX TIMEOUT
	}

	/*
		SETTER FUNCTIONS
	*/

	template <typename T>
	void ThreadSafeVector<T>::setConfig(vc_t CONFIG) {
		CONFIG_ = CONFIG;
	}

	/*
		LAZY INITIALIZATION FUNCTIONS
	*/

	template <typename T>
	void ThreadSafeVector<T>::lazyInitialization() {
		std::call_once(mutexesInitialized_, &ThreadSafeVector::initializeMutexes, this);
		std::call_once(mutexMapsInitialized_, &ThreadSafeVector::initializeMutexMaps, this);
	}
	template <typename T>
	void ThreadSafeVector<T>::initializeMutexes() {
		if (isConfigured(vc_t::ONLY_GLOBAL_MUTEX) && isNOTConfigured(vc_t::ENABLE_MUTEX_TIMEOUT)) {
			globalmtx_ = std::make_unique<std::mutex>();
		}
		if (isConfigured(vc_t::ONLY_GLOBAL_MUTEX) && isConfigured(vc_t::ENABLE_MUTEX_TIMEOUT)) {
			global_timedmtx_ = std::make_unique<std::mutex>();
		}
	}

	template <typename T>
	void ThreadSafeVector<T>::initializeMutexMaps() {
		if (isNOTConfigured(vc_t::ONLY_GLOBAL_MUTEX) && isNOTConfigured(vc_t::ENABLE_MUTEX_TIMEOUT_)) {
			mutex_map_ = std::make_unique<std::unordered_map<std::uint64_t, std::shared_ptr<std::mutex>>>();
		}
	}

	/*
		HELPER FUNCTIONS
	*/

	template <typename T>
	inline uint64_t ThreadSafeVector<T>::getMutexMapKey(uint64_t index) {

	}
	template <typename T>
	inline constexpr bool ThreadSafeVector<T>::isConfigured(vc_t CONFIG) const {
		return (CONFIG & CONFIG_);
	}
	template <typename T>
	inline constexpr bool ThreadSafeVector<T>::isNOTConfigured(vc_t CONFIG) const {
		return !(CONFIG & CONFIG_);
	}

	/*
		GETTER FUNCTIONS
	*/

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

	template <typename T>
	std::vector<T>& ThreadSafeVector<T>::vec() {
		if (isConfigured(vc_t::DANGEROUS)) {
			return vector_;
		}
	}

	template <typename T>
	vc_t ThreadSafeVector<T>::getConfig() {
		return CONFIG_;
	}
};