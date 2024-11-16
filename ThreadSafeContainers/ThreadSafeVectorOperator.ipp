#pragma once

#include "threadSafeVector.hpp"

namespace datatype {
	/*
		EQUATING / [] == [] OPERATOR
	*/
	template <typename T>
	bool ThreadSafeVector<T>::operator==(const ThreadSafeVector& other) const {
		std::lock_guard<std::mutex> thisLock(this->globalmtx_);
		std::lock_guard<std::mutex> otherLock(other.globalmtx_);
		return this->vector_ == other.vector_;
	}
	/*
		STRICT EQUATION / FUNCTION
		INCOMPLETE
	*/
	template <typename T>
	bool ThreadSafeVector<T>::strictestEquality(const ThreadSafeVector& other) const {
		if (!(*this == other)) {
			return false;
		}
		if constexpr (this->maxSize_ != other.maxSize_) {
			return false;
		}
		if constexpr (this->CONFIG_ != other.CONFIG_) {
			return false;
		}
		if constexpr (this->resizeable_ != other.resizeable_) {
			return false;
		}
		if constexpr (this->elementsPerMutex_ != other.elementsPerMutex_) {
			return false;
		}
		return true;
	}
	/*
		GETTING / T == [] OPERATOR
	*/
	template <typename T>
	const T& ThreadSafeVector<T>::operator[](uint64_t index) const {
		return _GET_OPERATOR_(index);
	}

	template <typename T>
	const T& ThreadSafeVector<T>::_GET_OPERATOR_(uint64_t index) const {
		if (CONFIG_ == vc_t::NO_CONFIG) {
			if (isConfigured(vc_t::THROW_EXCEPTIONS)) {
				throw std::runtime_error("Thread Safe Vector not configured.");
			}
			return _GET_OPERATOR_FALLBACK;
		}
		if (index >= maxSize_) {
			if (isConfigured(vc_t::THROW_EXCEPTIONS)) {
				throw std::out_of_range("Index out of range. Exceeds max size.");
			}
			return _GET_OPERATOR_FALLBACK;
		}
		if (index >= vector_.size()) {
			if (isConfigured(vc_t::THROW_EXCEPTIONS)) {
				throw std::out_of_range("Index out of range. Exceeds current size.");
			}
			return _GET_OPERATOR_FALLBACK;
		}
		// DANGEROUS
		if (isConfigured(vc_t::DANGEROUS)) {
			return vector_[index];
		}
	}

	template <typename T>
	template <typename U>
	typename std::enable_if<std::is_default_constructible<U>::value, const T&>::type
		ThreadSafeVector<T>::_GET_OPERATOR_FALLBACK() const {
		T dummy;
		return dummy;
	}

	template <typename T>
	template <typename U>
	typename std::enable_if<!std::is_default_constructible<U>::value, const T&>::type
		ThreadSafeVector<T>::_GET_OPERATOR_FALLBACK() const {
		int dummy = 0;
		return dummy;
	}

	/*
		SETTING / [] = T OPERATOR
	*/
	template <typename T>
	T& ThreadSafeVector<T>::operator[](uint64_t index) {
		return _SET_OPERATOR_(index);
	}

	template <typename T>
	T& ThreadSafeVector<T>::_SET_OPERATOR_(uint64_t index) {
		if (CONFIG_ == vc_t::NO_CONFIG) {
			if (isConfigured(vc_t::THROW_EXCEPTIONS)) {
				throw std::runtime_error("Thread Safe Vector not configured.");
			}
			return _SET_OPERATOR_FALLBACK();
		}
		if (index >= maxSize_) {
			if (isConfigured(vc_t::THROW_EXCEPTIONS)) {
				throw std::out_of_range("Index out of range. Exceeds max size.");
			}
			return _SET_OPERATOR_FALLBACK;
		}
		if (index >= vector_.size()) {
			if (isConfigured(vc_t::THROW_EXCEPTIONS)) {
				throw std::out_of_range("Index out of range. Exceeds current size.");
			}
			return _SET_OPERATOR_FALLBACK;
		}
		// DANGEROUS
		if (isConfigured(vc_t::DANGEROUS)) {
			return vector_[index];
		}
	}

	template <typename T>
	template <typename U>
	typename std::enable_if<std::is_default_constructible<U>::value, T&>::type
		ThreadSafeVector<T>::_SET_OPERATOR_FALLBACK() {
		T dummy;
		return dummy;
	}
	template <typename T>
	template <typename U>
	typename std::enable_if<!std::is_default_constructible<U>::value, T&>::type
		ThreadSafeVector<T>::_SET_OPERATOR_FALLBACK() {
		int dummy = 0;
		return dummy;
	}
};