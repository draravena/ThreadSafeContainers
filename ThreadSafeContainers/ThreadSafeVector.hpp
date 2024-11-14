#pragma once
#ifndef _THREAD_SAFE_VECTOR_HPP
#define _THREAD_SAFE_VECTOR_HPP

#ifndef LOCK_GUARD(mutex_)
#define LOCK_GUARD(mutex_) std::lock_guard<std::mutex> lock_mutex(mutex_)
#endif

#include <mutex>
#include <unordered_map>
#include <condition_variable>
#include <vector>
#include <cstdint>
#include <optional>

/*
IMPLEMENT:
-User defined read/write bias
-Transaction memory mode
-Unlocked reading
-Lock acquisition timeout

-Snapshots?



*/

namespace datatype {
	enum class vector_config {
		ALLOW_FREE_WRITES = 0x00,
		READ_HEAVY = 0x01,
		WRITE_HEAVY = 0x02,
		SNAPSHOT_ENABLED = 0x03,
		UNLOCKED_READING = 0x04,
		TRANSACTION_MODE = 0x05,

	};
	template <typename T>
	class ThreadSafeVector {
	public:
		ThreadSafeVector(uint64_t maxSize, uint64_t elementsPerMutex = UINT64_MAX);
		ThreadSafeVector();
		void setMaxSize(uint64_t maxSize, uint64_t elementsPerMutex = UINT64_MAX);
		bool push_back(T&& item);
		bool push_back(T& item);
		bool pop_back();
		std::optional<T> access(uint64_t index);
		bool set(uint64_t index, T& val);
		bool set(uint64_t index, T&& val);
		void setOperationBias();
		int64_t move(ThreadSafeVector& destination, bool explicitAll = true);
		uint64_t size();
		uint64_t maxSize();
		uint64_t remaining();
	private:
		// FUNCTIONS
		void init();
		void updateSize();
		void updateMutexMap();
		// CONCURRENCY
		std::mutex& mtx();
		uint64_t elementsPerMutex_;
		std::unordered_map<uint64_t, std::shared_ptr<std::mutex>> mutex_map_;
		std::mutex globalmtx_;
		// i dunno
		std::vector<T>& vec();
		uint64_t maxSize_;
		std::atomic<uint64_t> size_;
		std::vector<T> vector_;

	};
};

#include "threadSafeVector.ipp"

#endif