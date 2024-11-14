#pragma once
#include "threadSafeVector.hpp"

#include <iostream>

namespace datatype {
	template <typename T>
	ThreadSafeVector<T>::ThreadSafeVector(uint64_t maxSize, uint64_t elementsPerMutex) {
		maxSize_ = maxSize;
		vector_.reserve(maxSize);
		vector_.clear();
		updateSize();
		elementsPerMutex_ = elementsPerMutex;
		updateMutexMap();
	}
	template <typename T>
	ThreadSafeVector<T>::ThreadSafeVector() {}
	template <typename T>
	void ThreadSafeVector<T>::setMaxSize(uint64_t maxSize, uint64_t elementsPerMutex) {
		maxSize_ = maxSize;
		vector_.reserve(maxSize);
		vector_.clear();
		updateSize();
		elementsPerMutex_ = elementsPerMutex;
	}
	template <typename T>
	bool ThreadSafeVector<T>::push_back(T& item) {
		LOCK_GUARD(*mutex_map_[size_ + 1 / elementsPerMutex_]);
		if (vector_.size() >= maxSize_) {
			return false;
		}
		vector_.push_back(item);
		updateSize();
		return true;
	}
	template <typename T>
	bool ThreadSafeVector<T>::push_back(T&& item) {
		LOCK_GUARD(*mutex_map_[size_ + 1 / elementsPerMutex_]);
		if (vector_.size() >= maxSize_) {
			return false;
		}
		vector_.push_back(std::move(item));
		updateSize();
		return true;
	}
	template <typename T>
	bool ThreadSafeVector<T>::pop_back() {
		LOCK_GUARD(*mutex_map_[size_ - 1 / elementsPerMutex_]);
		if (vector_.empty()) {
			return false;
		}
		vector_.pop_back();
		updateSize();
		return true;
	}
	template <typename T>
	std::optional<T> ThreadSafeVector<T>::access(uint64_t index) {
		if (index >= vector_.size()) {
			return std::nullopt;
		}
		LOCK_GUARD(*mutex_map_[index / elementsPerMutex_]);
		return vector_[index];
	}
	template <typename T>
	bool ThreadSafeVector<T>::set(uint64_t index, T& val) {
		if (index >= vector_.size()) {
			return false;
		}
		LOCK_GUARD(*mutex_map_[index / elementsPerMutex_]);
		vector_[index] = val;
		return true;
	}
	template <typename T>
	bool ThreadSafeVector<T>::set(uint64_t index, T&& val) {
		if (index >= maxSize_) {
			return false;
		}
		LOCK_GUARD(*mutex_map_[index / elementsPerMutex_]);
		std::cout << "g";
		vector_[index] = val;
		std::cout << vector_.size();
		updateSize();
		return true;
	}
	template <typename T>
	int64_t ThreadSafeVector<T>::move(ThreadSafeVector& destination, bool explicitAll) {
		std::lock_guard<std::mutex> lock1(destination.mtx());
		std::lock_guard<std::mutex> lock2(globalmtx_);
		uint64_t dest_remaining = destination.remaining();
		if (dest_remaining < vector_.size() && explicitAll == true) {
			return -1;
		}
		else if (dest_remaining < vector_.size() && explicitAll == false) {
			destination.vec().insert(destination.vec().end(), vector_.begin(), vector_.begin() + dest_remaining);
			vector_.erase(vector_.begin(), vector_.begin() + dest_remaining);
			destination.updateSize();
			updateSize();
			return 0;
		}
		destination.vec().insert(destination.vec().end(), vector_.begin(), vector_.end());
		vector_.clear();
		destination.updateSize();
		updateSize();
		return 1;
	}
	template <typename T>
	void ThreadSafeVector<T>::updateMutexMap() {
		for (uint64_t i = 0; i < maxSize_ / elementsPerMutex_; i++) {
			std::shared_ptr<std::mutex> shared_mtx = std::make_shared<std::mutex>();
			mutex_map_[i] = std::move(shared_mtx);
		}
	}
	// possibly not thread-safe?? fix next time
	template <typename T>
	std::vector<T>& ThreadSafeVector<T>::vec() {
		return vector_;
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
		return maxSize_ - vector_.size();
	}
	template <typename T>
	void ThreadSafeVector<T>::updateSize() {
		size_ = vector_.size();
	}
	template <typename T>
	std::mutex& ThreadSafeVector<T>::mtx() {
		return globalmtx_;
	}

};