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
#include <chrono>
#include <functional>
#include <type_traits>
#include <stdexcept>

/*
IMPLEMENT:
-User defined read/write bias
-Transaction memory mode
-Unlocked reading
-Lock acquisition timeout

-Snapshots?



*/

namespace datatype {
	typedef enum class vc_t : uint64_t {
		// CORE
		NO_CONFIG = 0x00 | 0x01 | 0x10,
		DANGEROUS = 0x01,
		READ_HEAVY = 0x02,
		WRITE_HEAVY = 0x04,
		ENABLE_MUTEX_TIMEOUT = 0x08,
		ONLY_GLOBAL_MUTEX = 0x10,
		RESERVE_MAX_SIZE = 0x20,
		// NICHE
		SNAPSHOT_ENABLED = 0x40,
		DISABLE_READING_UNTIL_FULL = 0x80,
		DISABLE_WRITING_UNTIL_EMPTY = 0x100,
		// IDK
		UNLOCKED_READING = 0x200, // ?
		TRANSACTION_MODE = 0x400, // ?
		ENABLE_SPINLOCK = 0x800, // ?
		// DEBUG
		ENABLE_STATS = 0x1000,
		THROW_EXCEPTION = 0x2000
		
		
		
	} vc_t ;

	// Bitwise OR operator
	constexpr bool operator&(vc_t lhs, vc_t rhs) {
		return (static_cast<uint64_t>(lhs) & static_cast<uint64_t>(rhs));
	}

	constexpr vc_t operator|(vc_t lhs, vc_t rhs) {
		return static_cast<vc_t>((static_cast<uint64_t>(lhs) | static_cast<uint64_t>(rhs)));
	}
	

	template <typename T>
	class ThreadSafeVector {
	public:
		// CONSTRUCTORS
		ThreadSafeVector(uint64_t maxSize = UINT64_MAX, vc_t CONFIG = vc_t::NO_CONFIG);
		ThreadSafeVector(vc_t CONFIG = vc_t::NO_CONFIG);
		// VECTOR MODIFIER/ACCESSORS
		bool push_back(T&& item);                     // NO IMPLEMENTATION 
		bool push_back(T& item);                     // NO IMPLEMENTATION 
		bool pop_back();                     // NO IMPLEMENTATION 
		std::optional<T> access(uint64_t index);                     // NO IMPLEMENTATION 
		bool set(uint64_t index, T& val);                     // NO IMPLEMENTATION 
		bool set(uint64_t index, T&& val);                     // NO IMPLEMENTATION 
		int64_t move(ThreadSafeVector& destination, bool explicitAll = true);                     // NO IMPLEMENTATION 
		// OPERATOR OVERLOADS
		bool operator==(const ThreadSafeVector& other) const;
		const T& operator[](uint64_t index) const;
		T& operator[](uint64_t index);
		bool strictestEquality(const ThreadSafeVector& other) const;
		// VECTOR GETTER FUNCTIONS
		uint64_t size();
		bool resizeable();
		uint64_t maxSize();
		uint64_t remaining();
		// CONFIG SETTER FUNCTIONS
		// CONFIG GETTER FUNCTIONS
		bool snapshotEnabled();                     // NO IMPLEMENTATION 
		bool spinlockEnabled();                     // NO IMPLEMENTATION 
		bool statsEnabled();                     // NO IMPLEMENTATION 
		bool readingUnlocked();                     // NO IMPLEMENTATION 
		bool dangerous();                     // NO IMPLEMENTATION 
		bool transaction_mode();                     // NO IMPLEMENTATION 
		static uint64_t getTotalRAM();
		static uint64_t getFreeRAM();
		std::vector<T> vector_;
	private:
		// CONFIG
		vc_t CONFIG_;
		bool resizeable_;
		std::chrono::microseconds timedMutexWaitDuration_;
		// FUNCTIONS
		void init(uint64_t maxSize = UINT64_MAX, vc_t CONFIG = vc_t::NO_CONFIG);
		void updateMutexMap();                     // NO IMPLEMENTATION 
		static MEMORYSTATUSEX getMemStatusEX();
		inline std::vector<T>& vec();                     // NO IMPLEMENTATION 
		T* data();
		// CONCURRENCY
		uint64_t elementsPerMutex_ = UINT64_MAX;
		std::unordered_map<uint64_t, std::shared_ptr<std::mutex>> mutex_map_;                     // NO IMPLEMENTATION 
		mutable std::mutex globalmtx_;
		mutable std::timed_mutex global_timedmtx_;
		// VECTOR
		std::atomic<uint64_t> maxSize_;
		std::atomic<uint64_t> size_;

		// HELPER FUNCTIONS
		inline bool isConfigured(vc_t CONFIG) const {
			return (CONFIG & CONFIG_);
		}
		inline bool isNOTConfigured(vc_t CONFIG) const {
			return !(CONFIG & CONFIG_);
		}
		inline void throwException(const char* message) {
			if (isConfigured(vc_t::THROW_EXCEPTION)) {
				throw std::exception(message);
			}
		}
		// OVERLOAD FUNCTIONS

		T& _SET_OPERATOR_(uint64_t index);
		template <typename U = T>
		typename std::enable_if<std::is_default_constructible<U>::value, T&>::type
			_SET_OPERATOR_FALLBACK() {
			T dummy;
			return dummy;
		}

		template <typename U = T>
		typename std::enable_if<!std::is_default_constructible<U>::value, T&>::type
			_SET_OPERATOR_FALLBACK() {
			int dummy = 0;
			return dummy;
		}

		const T& _GET_OPERATOR_(uint64_t index) const;
		template <typename U = T>
		typename std::enable_if<std::is_default_constructible<U>::value, const T&>::type
			_GET_OPERATOR_FALLBACK() const {
			T dummy;
			return dummy;
		}

		template <typename U = T>
		typename std::enable_if<!std::is_default_constructible<U>::value, const T&>::type
			_GET_OPERATOR_FALLBACK() const {
			int dummy = 0;
			return dummy;
		}
	};
};

#include "threadSafeVector.ipp"
#include "ThreadSafeVectorOperator.ipp"

#endif