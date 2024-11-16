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
		NO_CONFIG = 0x00 | 0x2000,
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
		// OPTIONS
		RESIZEABLE = 0x200,
		TRANSACTION_MODE = 0x400, // ?
		ENABLE_SPINLOCK = 0x800, // ?
		// DEBUG
		ENABLE_STATS = 0x1000,
		THROW_EXCEPTIONS = 0x2000



	} vc_t;

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
		// VECTOR FUNCTIONS
		bool push_back(T&& item);
		bool push_back(T& item);
		bool pop_back(uint64_t count = 1);
		int64_t append(ThreadSafeVector& destination, bool explicitAll = true, vc_t RESIZE_FLAG = vc_t::NO_CONFIG);

		void clear();

		// OPERATOR OVERLOADS
		using SimdType = typename
			std::conditional<
			std::is_same<T, int>::value, __m512i,
			typename std::conditional<std::is_same<T, long>::value, __m512i,
			typename std::conditional<std::is_same<T, short>::value, __m512i,
			typename std::conditional<std::is_same<T, float>::value, __m512,
			typename std::conditional<std::is_same<T, double>::value, __m512d,
			void>::type>::type>::type>::type>::type;

		SimdType AVXRegister;

		bool operator==(const ThreadSafeVector& other) const;
		const T& operator[](uint64_t index) const;
		T& operator[](uint64_t index);

		// AVX Increment (NOT IMPLEMENTED)
		template <typename U = T>
		typename std::enable_if<std::is_integral<U>::value, ThreadSafeVector<T>&>::type
			operator++(int) {
			std::lock_guard<std::mutex> lock(*(this->globalmtx_));
			std::vector<T>& vec = this->vector_;
			return *this;
		}

		bool strictestEquality(const ThreadSafeVector& other) const;
		// VECTOR GETTER FUNCTIONS
		uint64_t size();
		bool resizeable();
		uint64_t maxSize();
		uint64_t remaining();
		// CONFIG SETTER FUNCTIONS
		void setConfig(vc_t CONFIG);
		// CONFIG GETTER FUNCTIONS
		vc_t getConfig();
		static uint64_t getTotalRAM();
		static uint64_t getFreeRAM();
		std::vector<T> vector_;
	private:
		// CONFIG
		std::atomic<vc_t> CONFIG_;

		bool resizeable_;

		std::once_flag mutexesInitialized_;

		std::once_flag mutexMapsInitialized_;

		std::chrono::microseconds timedMutexWaitDuration_;

		// FUNCTIONS
		void init(uint64_t maxSize = UINT64_MAX, vc_t CONFIG = vc_t::NO_CONFIG);

		void updateMutexMap();                     // NO IMPLEMENTATION 

		static MEMORYSTATUSEX getMemStatusEX();

		inline std::vector<T>& vec();

		void lazyInitialization();

		void initializeMutexes();

		void initializeMutexMaps();

		T* data();

		// CONCURRENCY
		uint64_t elementsPerMutex_ = UINT64_MAX;

		std::unique_ptr<std::unordered_map<uint64_t, std::shared_ptr<std::mutex>>> mutex_map_;

		std::unique_ptr<std::unordered_map<uint64_t, std::shared_ptr<std::timed_mutex>>> timed_mutex_map_;

		mutable std::unique_ptr<std::mutex> globalmtx_;

		mutable std::unique_ptr<std::timed_mutex> global_timedmtx_;

		// VECTOR
		std::atomic<uint64_t> maxSize_;

		std::atomic<uint64_t> size_;
		// HELPER FUNCTIONS

		inline constexpr bool isConfigured(vc_t CONFIG) const;

		inline constexpr bool isNOTConfigured(vc_t CONFIG) const;

		inline uint64_t getMutexMapKey(uint64_t index);

		bool _PUSH_BACK_HELPER(T&& item);

		bool _POP_BACK_HELPER(uint64_t count = 1);

		// OVERLOAD FUNCTIONS
		T& _SET_OPERATOR_(uint64_t index);

		template <typename U = T>
		typename std::enable_if<std::is_default_constructible<U>::value, T&>::type
			_SET_OPERATOR_FALLBACK();

		template <typename U = T>
		typename std::enable_if<!std::is_default_constructible<U>::value, T&>::type
			_SET_OPERATOR_FALLBACK();

		const T& _GET_OPERATOR_(uint64_t index) const;

		template <typename U = T>
		typename std::enable_if<std::is_default_constructible<U>::value, const T&>::type
			_GET_OPERATOR_FALLBACK() const;

		template <typename U = T>
		typename std::enable_if<!std::is_default_constructible<U>::value, const T&>::type
			_GET_OPERATOR_FALLBACK() const;

	};
};

#include "threadSafeVector.ipp"
#include "ThreadSafeVectorOperator.ipp"

#endif