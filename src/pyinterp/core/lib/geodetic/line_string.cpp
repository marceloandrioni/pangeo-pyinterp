// Copyright (c) 2022 CNES
//
// All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.
#include "pyinterp/geodetic/line_string.hpp"

#include "pyinterp/detail/broadcast.hpp"

namespace pyinterp::geodetic {

LineString::LineString(const Eigen::Ref<const Vector<double>>& lon,
                       const Eigen::Ref<const Vector<double>>& lat) {
  detail::check_eigen_shape("lon", lon, "lat", lat);
  for (auto ix = static_cast<Eigen::Index>(0); ix < lon.size(); ++ix) {
    Base::emplace_back(Point{lon(ix), lat(ix)});
  }
}

auto LineString::from_geojson(const pybind11::list& array) -> LineString {
  auto result = LineString{};
  auto* base = dynamic_cast<Base*>(&result);
  for (auto& point : array) {
    base->push_back(Point::from_geojson(point.cast<pybind11::list>()));
  }
  return result;
}

auto LineString::intersection(const LineString& rhs) const
    -> std::optional<Point> {
  std::deque<Point> output;
  boost::geometry::intersection(*this, rhs, output);

  if (output.empty()) {
    // There is no intersection.
    return {};
  }

  if (output.size() != 1) {
    // If there is a merged point between lines #1 and #2 then the method will
    // find this point for each of the segments tested.
    std::set<std::tuple<double, double>> points;
    for (auto& item : output) {
      points.insert(std::make_tuple(item.get<0>(), item.get<1>()));
    }
    if (points.size() != 1) {
      // If the intersection is not a point then an exception is thrown.
      throw std::runtime_error(
          "The geometry of the intersection is not a point");
    }
  }
  return output[0];
}

auto LineString::getstate() const -> pybind11::tuple {
  auto lon = pybind11::array_t<double>(pybind11::array::ShapeContainer{size()});
  auto lat = pybind11::array_t<double>(pybind11::array::ShapeContainer{size()});
  auto _lon = lon.mutable_unchecked<1>();
  auto _lat = lat.mutable_unchecked<1>();
  auto ix = static_cast<int64_t>(0);
  for (const auto& item : *this) {
    _lon[ix] = item.lon();
    _lat[ix] = item.lat();
    ++ix;
  }
  return pybind11::make_tuple(lon, lat);
}

auto LineString::setstate(const pybind11::tuple& state) -> LineString {
  if (state.size() != 2) {
    throw std::runtime_error("invalid state");
  }
  auto lon = state[0].cast<pybind11::array_t<double>>();
  auto lat = state[1].cast<pybind11::array_t<double>>();

  auto x = Eigen::Map<const Vector<double>>(lon.data(), lon.size());
  auto y = Eigen::Map<const Vector<double>>(lat.data(), lat.size());
  return {x, y};
}

auto LineString::to_geojson() const -> pybind11::dict {
  auto result = pybind11::dict();
  result["type"] = "LineString";
  auto coordinates = pybind11::list();
  for (auto& point : *this) {
    coordinates.append(point.coordinates());
  }
  result["coordinates"] = coordinates;
  return result;
}

}  // namespace pyinterp::geodetic
