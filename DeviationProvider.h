/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C) 2019  Joseph Artsimovich, 4lex4 (Integration from ScanTailor Advanced - GPL-3.0)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
*/

#ifndef DEVIATIONPROVIDER_H_
#define DEVIATIONPROVIDER_H_

#include "NonCopyable.h"
#include <cmath>
#include <functional>
#include <unordered_map>

template <typename K, typename Hash = std::hash<K>>
class DeviationProvider
{
	DECLARE_NON_COPYABLE(DeviationProvider)
public:
	DeviationProvider() = default;

	explicit DeviationProvider(std::function<double(K const&)> const& computeValueByKey);

	bool isDeviant(K const& key, double coefficient = 1.0, double threshold = 0.0, bool defaultVal = false) const;

	double getDeviationValue(K const& key) const;

	void addOrUpdate(K const& key);
	void addOrUpdate(K const& key, double value);
	void remove(K const& key);
	void clear();
	void setComputeValueByKey(std::function<double(K const&)> const& computeValueByKey);

protected:
	void update() const;

private:
	std::function<double(K const&)> m_computeValueByKey;
	std::unordered_map<K, double, Hash> m_keyValueMap;
	mutable bool m_needUpdate = false;
	mutable double m_meanValue = 0.0;
	mutable double m_standardDeviation = 0.0;
};


template <typename K, typename Hash>
DeviationProvider<K, Hash>::DeviationProvider(std::function<double(K const&)> const& computeValueByKey)
	: m_computeValueByKey(computeValueByKey) {}

template <typename K, typename Hash>
bool DeviationProvider<K, Hash>::isDeviant(K const& key, double coefficient, double threshold, bool defaultVal) const
{
	if (m_keyValueMap.find(key) == m_keyValueMap.end()) {
		return false;
	}
	if (m_keyValueMap.size() < 3) {
		return false;
	}
	double value = m_keyValueMap.at(key);
	if (std::isnan(value)) {
		return defaultVal;
	}
	update();
	return (std::abs(value - m_meanValue)
		> std::max((coefficient * m_standardDeviation), (threshold / 100) * m_meanValue));
}

template <typename K, typename Hash>
double DeviationProvider<K, Hash>::getDeviationValue(K const& key) const
{
	if (m_keyValueMap.find(key) == m_keyValueMap.end()) {
		return -1.0;
	}
	if (m_keyValueMap.size() < 2) {
		return 0.0;
	}
	double value = m_keyValueMap.at(key);
	if (std::isnan(value)) {
		return -1.0;
	}
	update();
	return std::abs(m_keyValueMap.at(key) - m_meanValue);
}

template <typename K, typename Hash>
void DeviationProvider<K, Hash>::addOrUpdate(K const& key)
{
	m_needUpdate = true;
	m_keyValueMap[key] = m_computeValueByKey(key);
}

template <typename K, typename Hash>
void DeviationProvider<K, Hash>::addOrUpdate(K const& key, double value)
{
	m_needUpdate = true;
	m_keyValueMap[key] = value;
}

template <typename K, typename Hash>
void DeviationProvider<K, Hash>::remove(K const& key)
{
	m_needUpdate = true;
	if (m_keyValueMap.find(key) == m_keyValueMap.end()) {
		return;
	}
	m_keyValueMap.erase(key);
}

template <typename K, typename Hash>
void DeviationProvider<K, Hash>::update() const
{
	if (!m_needUpdate) {
		return;
	}
	if (m_keyValueMap.size() < 2) {
		return;
	}
	int count = 0;
	double sum = 0.0;
	for (typename std::unordered_map<K, double, Hash>::const_iterator it = m_keyValueMap.begin();
	     it != m_keyValueMap.end(); ++it) {
		if (!std::isnan(it->second)) {
			sum += it->second;
			++count;
		}
	}
	m_meanValue = sum / count;
	double differencesSum = 0.0;
	for (typename std::unordered_map<K, double, Hash>::const_iterator it = m_keyValueMap.begin();
	     it != m_keyValueMap.end(); ++it) {
		if (!std::isnan(it->second)) {
			differencesSum += std::pow(it->second - m_meanValue, 2);
		}
	}
	m_standardDeviation = std::sqrt(differencesSum / (count - 1));
	m_needUpdate = false;
}

template <typename K, typename Hash>
void DeviationProvider<K, Hash>::setComputeValueByKey(std::function<double(K const&)> const& computeValueByKey)
{
	m_computeValueByKey = computeValueByKey;
}

template <typename K, typename Hash>
void DeviationProvider<K, Hash>::clear()
{
	m_keyValueMap.clear();
	m_needUpdate = false;
	m_meanValue = 0.0;
	m_standardDeviation = 0.0;
}

#endif
