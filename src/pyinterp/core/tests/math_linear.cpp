// Copyright (c) 2023 CNES
//
// All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.
#include <gtest/gtest.h>

#include "pyinterp/detail/math/linear.hpp"

namespace math = pyinterp::detail::math;

TEST(math_linear, linear) {
  /// https://en.wikipedia.org/wiki/Bilinear_interpolation
  EXPECT_DOUBLE_EQ(math::linear(14.5, 14.0, 15.0, 91.0, 210.0), 150.5);
  EXPECT_DOUBLE_EQ(math::linear(14.5, 14.0, 15.0, 162.0, 95.0), 128.5);

  auto y = math::linear<int64_t, double>(14, 13, 15, 91.0, 210.0);
  EXPECT_DOUBLE_EQ(y, 150.5);

  y = math::linear<int64_t, double>(14, 13, 15, 162.0, 95.0);
  EXPECT_DOUBLE_EQ(y, 128.5);
}

TEST(math_linear, k_nearest_neighbors) {
  Eigen::MatrixXd coordinates(3, 4);
  Eigen::VectorXd values(4);
  Eigen::VectorXd query(3);

  coordinates(0, 0) = 0;
  coordinates(1, 0) = 0;
  coordinates(2, 0) = 0;
  coordinates(0, 1) = 1;
  coordinates(1, 1) = 1;
  coordinates(2, 1) = 1;
  coordinates(0, 2) = 2;
  coordinates(1, 2) = 1;
  coordinates(2, 2) = 2;
  coordinates(0, 3) = 3;
  coordinates(1, 3) = 0;
  coordinates(2, 3) = 1;
  values << 0, 1, 2, 1;
  query << 1.5, 0.5, 1;
  auto x = math::linear(coordinates, values, query);
  EXPECT_NEAR(x, 1.0829899872778919, 1e-8);
}
