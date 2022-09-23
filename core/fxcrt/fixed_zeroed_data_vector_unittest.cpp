// Copyright 2022 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fxcrt/fixed_zeroed_data_vector.h"

#include "core/fxcrt/span_util.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/base/span.h"

TEST(FixedZeroedDataVector, NoData) {
  FixedZeroedDataVector<int> vec;
  EXPECT_EQ(0u, vec.size());
  EXPECT_TRUE(vec.empty());
  EXPECT_TRUE(vec.span().empty());
  EXPECT_TRUE(vec.writable_span().empty());
}

TEST(FixedZeroedDataVector, WithData) {
  FixedZeroedDataVector<int> vec(4);
  EXPECT_FALSE(vec.empty());
  EXPECT_EQ(4u, vec.size());
  EXPECT_EQ(4u, vec.span().size());
  EXPECT_EQ(4u, vec.writable_span().size());
  EXPECT_THAT(vec.span(), testing::ElementsAre(0, 0, 0, 0));

  constexpr int kData[] = {1, 2, 3, 4};
  fxcrt::spancpy(vec.writable_span(), pdfium::make_span(kData));
  EXPECT_THAT(vec.span(), testing::ElementsAre(1, 2, 3, 4));
}

TEST(FixedZeroedDataVector, Move) {
  FixedZeroedDataVector<int> vec(4);
  constexpr int kData[] = {1, 2, 3, 4};
  ASSERT_EQ(4u, vec.writable_span().size());
  fxcrt::spancpy(vec.writable_span(), pdfium::make_span(kData));

  FixedZeroedDataVector<int> vec2(std::move(vec));
  EXPECT_FALSE(vec2.empty());
  EXPECT_EQ(4u, vec2.size());
  EXPECT_EQ(4u, vec2.span().size());
  EXPECT_EQ(4u, vec2.writable_span().size());
  EXPECT_THAT(vec2.span(), testing::ElementsAre(1, 2, 3, 4));

  EXPECT_EQ(0u, vec.size());
  EXPECT_TRUE(vec.empty());
  EXPECT_TRUE(vec.span().empty());
  EXPECT_TRUE(vec.writable_span().empty());

  vec = std::move(vec2);
  EXPECT_FALSE(vec.empty());
  EXPECT_EQ(4u, vec.size());
  EXPECT_EQ(4u, vec.span().size());
  EXPECT_EQ(4u, vec.writable_span().size());
  EXPECT_THAT(vec.span(), testing::ElementsAre(1, 2, 3, 4));

  EXPECT_EQ(0u, vec2.size());
  EXPECT_TRUE(vec2.empty());
  EXPECT_TRUE(vec2.span().empty());
  EXPECT_TRUE(vec2.writable_span().empty());
}
