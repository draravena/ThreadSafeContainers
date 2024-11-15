#pragma once

#include "threadSafeVector.hpp"

namespace datatype {
	/*
		GETTING / T == [] OPERATOR
	*/
	template <typename T>
	const T& ThreadSafeVector<T>::operator[](uint64_t index) const {
		return _GET_OPERATOR_(index);
	}

	template <typename T>
	const T& ThreadSafeVector<T>::_GET_OPERATOR_(uint64_t index) const {
		if (index >= maxSize_) {
			throwException("Out of bounds. Larger than max size");
		}
		if (index >= vector_.size()) {
			throwException("Out of bounds. Larger than vector size.");
		}
		// DANGEROUS
		if (isConfigured(vc_t::DANGEROUS)) {
			return vector_[index];
		}
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
		if (index >= maxSize_) {
			throwException("Out of bounds. Larger than max size");
		}
		if (index >= vector_.size()) {
			throwException("Out of bounds. Larger than vector size.");
		}
		_SET_OPERATOR_FALLBACK();
		// DANGEROUS
		if (isConfigured(vc_t::DANGEROUS)) {
			return vector_[index];
		}
	}
};