#pragma once
#ifndef _THREAD_SAFE_VECTOR_HPP
#define _THREAD_SAFE_VECTOR_HPP

#ifndef LOCK_GUARD(mutex_)
#define LOCK_GUARD(mutex_) std::lock_guard<std::mutex> lock_mutex(mutex_)
#endif

#include <immintrin.h>

#include <mutex>
#include <unordered_map>
#include <condition_variable>
#include <vector>
#include <cstdint>
#include <optional>
#include <Windows.h>

/*
IMPLEMENT:
-User defined read/write bias
-Transaction memory mode
-Unlocked reading
-Lock acquisition timeout

-Snapshots?



*/

namespace datatype {
	typedef enum class vector_config_t : uint64_t {
		NO_CONFIG = 0x00,
		DANGEROUS = 0x01,
		READ_HEAVY = 0x02,
		WRITE_HEAVY = 0x04,
		ENABLE_MUTEX_TIMEOUT = 0x08,
		SNAPSHOT_ENABLED = 0x10,
		UNLOCKED_READING = 0x20,
		TRANSACTION_MODE = 0x40,
		ONLY_GLOBAL_MUTEX = 0x80,
		DISABLE_READING_UNTIL_FULL = 0x100,
		DISABLE_WRITING_UNTIL_FULL = 0x200,
		ENABLE_STATS = 0x400,
		ENABLE_SPINLOCK = 0x800,
		RESERVE_MAX_SIZE = 0x1000
	} vector_config_t ;

	// Bitwise OR operator
	constexpr bool operator&(vector_config_t lhs, vector_config_t rhs) {
		return (static_cast<uint64_t>(lhs) & static_cast<uint64_t>(rhs));
	}
	

	template <typename T = int>
	class ThreadSafeVector {
	public:
		// CONSTRUCTORS
		ThreadSafeVector(uint64_t maxSize = UINT64_MAX, vector_config_t CONFIG = vector_config_t::NO_CONFIG);
		ThreadSafeVector(vector_config_t CONFIG = vector_config_t::NO_CONFIG);
		// VECTOR MODIFIER/ACCESSORS
		bool push_back(T&& item);
		bool push_back(T& item);
		bool pop_back();
		std::optional<T> access(uint64_t index);
		bool set(uint64_t index, T& val);
		bool set(uint64_t index, T&& val);
		int64_t move(ThreadSafeVector& destination, bool explicitAll = true);
		// OPERATOR OVERLOADS
		// VECTOR GETTER FUNCTIONS
		uint64_t size();
		uint64_t maxSize();
		uint64_t remaining();
		// CONFIG SETTER FUNCTIONS
		// CONFIG GETTER FUNCTIONS
		bool snapshotEnabled();
		bool spinlockEnabled();
		bool statsEnabled();
		bool readingUnlocked();
		bool dangerous();
		bool transaction_mode();
		static uint64_t getTotalRAM();
		static uint64_t getFreeRAM();
	private:
		// CONFIG
		vector_config_t CONFIG_;
		static MEMORYSTATUSEX getMemStatusEX();
		// FUNCTIONS
		void init(uint64_t maxSize = UINT64_MAX, vector_config_t CONFIG = vector_config_t::NO_CONFIG);
		void updateSize();
		void updateMutexMap();
		std::mutex& mtx();
		// CONCURRENCY
		uint64_t elementsPerMutex_ = UINT64_MAX;
		std::unique_ptr<std::unordered_map<uint64_t, std::shared_ptr<std::mutex>>> mutex_map_;
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