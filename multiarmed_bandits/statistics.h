#ifndef STATISTICS_H_
#define STATISTICS_H_

#include <nlohmann/json.hpp>

#include <iostream>
#include <iterator>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace stats {

/**
 * Point: A single value.
 * Series: A sequence of values.
 * WideSeries: A sequence of array of values.
 */
enum class Type { Point, Series, WideSeries };

template <Type T> struct Traits;
template <> struct Traits<Type::WideSeries> {
  using data_type = std::vector<std::vector<double>>;
};
template <> struct Traits<Type::Series> {
  using data_type = std::vector<double>;
};
template <> struct Traits<Type::Point> { using data_type = double; };

class Store {
  template <Type T>
  using map_type =
      typename std::unordered_map<std::string, typename Traits<T>::data_type>;
  // point stats
  map_type<Type::Point> m_point;
  // running stats
  map_type<Type::WideSeries> m_wide_series;
  // static stats
  map_type<Type::Series> m_series;
  // stats names
  std::vector<std::string> m_names;

public:
  template <Type T>
  typename Traits<T>::data_type &add(const std::string &name) {
    typename map_type<T>::iterator it;
    bool inserted;
    if constexpr (T == Type::WideSeries) {
      std::tie(it, inserted) = m_wide_series.emplace(
          std::piecewise_construct, std::forward_as_tuple(name),
          std::forward_as_tuple());
    } else if constexpr (T == Type::Series) {
      std::tie(it, inserted) = m_series.emplace(std::piecewise_construct,
                                                std::forward_as_tuple(name),
                                                std::forward_as_tuple());
    }

    if (inserted) {
      m_names.push_back(name);
      return it->second;
    }

    throw std::runtime_error("Duplicate statistics name");
  }

  template <Type T>
  const typename Traits<T>::data_type &get(const std::string &name) const {
    if constexpr (T == Type::WideSeries) {
      return m_wide_series.at(name);
    } else if constexpr (T == Type::Series) {
      return m_series.at(name);
    }
  }

  const std::vector<std::string> &list() const { return m_names; }
  auto wide_series_beg() const { return m_wide_series.begin(); }
  auto wide_series_end() const { return m_wide_series.end(); }
  auto series_beg() const { return m_series.begin(); }
  auto series_end() const { return m_series.end(); }
  auto points_beg() const { return m_point.begin(); }
  auto points_end() const { return m_point.end(); }
};

} // namespace stats

#endif // STATISTICS_H_
