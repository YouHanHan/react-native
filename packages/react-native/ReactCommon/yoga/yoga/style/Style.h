/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <type_traits>

#include <yoga/Yoga.h>
#include <yoga/YGFloatOptional.h>
#include <yoga/Yoga-internal.h>
#include <yoga/BitUtils.h>

#include <yoga/style/CompactValue.h>

namespace facebook::yoga {

class YOGA_EXPORT Style {
  template <typename Enum>
  using Values = detail::Values<enums::count<Enum>()>;

public:
  using Dimensions = Values<YGDimension>;
  using Edges = Values<YGEdge>;
  using Gutters = Values<YGGutter>;

  template <typename T>
  struct BitfieldRef {
    Style& style;
    size_t offset;
    operator T() const {
      return facebook::yoga::detail::getEnumData<T>(style.flags, offset);
    }
    BitfieldRef<T>& operator=(T x) {
      facebook::yoga::detail::setEnumData<T>(style.flags, offset, x);
      return *this;
    }
  };

  template <typename T, T Style::*Prop>
  struct Ref {
    Style& style;
    operator T() const { return style.*Prop; }
    Ref<T, Prop>& operator=(T value) {
      style.*Prop = value;
      return *this;
    }
  };

  template <typename Idx, Values<Idx> Style::*Prop>
  struct IdxRef {
    struct Ref {
      Style& style;
      Idx idx;
      operator CompactValue() const { return (style.*Prop)[idx]; }
      operator YGValue() const { return (style.*Prop)[idx]; }
      Ref& operator=(CompactValue value) {
        (style.*Prop)[idx] = value;
        return *this;
      }
    };

    Style& style;
    IdxRef<Idx, Prop>& operator=(const Values<Idx>& values) {
      style.*Prop = values;
      return *this;
    }
    operator const Values<Idx>&() const { return style.*Prop; }
    Ref operator[](Idx idx) { return {style, idx}; }
    CompactValue operator[](Idx idx) const { return (style.*Prop)[idx]; }
  };

  Style() {
    alignContent() = YGAlignFlexStart;
    alignItems() = YGAlignStretch;
  }
  ~Style() = default;

private:
  static constexpr size_t directionOffset = 0;
  static constexpr size_t flexdirectionOffset =
      directionOffset + facebook::yoga::detail::bitWidthFn<YGDirection>();
  static constexpr size_t justifyContentOffset = flexdirectionOffset +
      facebook::yoga::detail::bitWidthFn<YGFlexDirection>();
  static constexpr size_t alignContentOffset =
      justifyContentOffset + facebook::yoga::detail::bitWidthFn<YGJustify>();
  static constexpr size_t alignItemsOffset =
      alignContentOffset + facebook::yoga::detail::bitWidthFn<YGAlign>();
  static constexpr size_t alignSelfOffset =
      alignItemsOffset + facebook::yoga::detail::bitWidthFn<YGAlign>();
  static constexpr size_t positionTypeOffset =
      alignSelfOffset + facebook::yoga::detail::bitWidthFn<YGAlign>();
  static constexpr size_t flexWrapOffset =
      positionTypeOffset + facebook::yoga::detail::bitWidthFn<YGPositionType>();
  static constexpr size_t overflowOffset =
      flexWrapOffset + facebook::yoga::detail::bitWidthFn<YGWrap>();
  static constexpr size_t displayOffset =
      overflowOffset + facebook::yoga::detail::bitWidthFn<YGOverflow>();

  uint32_t flags = 0;

  YGFloatOptional flex_ = {};
  YGFloatOptional flexGrow_ = {};
  YGFloatOptional flexShrink_ = {};
  CompactValue flexBasis_ = CompactValue::ofAuto();
  Edges margin_ = {};
  Edges position_ = {};
  Edges padding_ = {};
  Edges border_ = {};
  Gutters gap_ = {};
  Dimensions dimensions_{CompactValue::ofAuto()};
  Dimensions minDimensions_ = {};
  Dimensions maxDimensions_ = {};
  // Yoga specific properties, not compatible with flexbox specification
  YGFloatOptional aspectRatio_ = {};

public:
  // for library users needing a type
  using ValueRepr = std::remove_reference<decltype(margin_[0])>::type;

  YGDirection direction() const {
    return facebook::yoga::detail::getEnumData<YGDirection>(
        flags, directionOffset);
  }
  BitfieldRef<YGDirection> direction() { return {*this, directionOffset}; }

  YGFlexDirection flexDirection() const {
    return facebook::yoga::detail::getEnumData<YGFlexDirection>(
        flags, flexdirectionOffset);
  }
  BitfieldRef<YGFlexDirection> flexDirection() {
    return {*this, flexdirectionOffset};
  }

  YGJustify justifyContent() const {
    return facebook::yoga::detail::getEnumData<YGJustify>(
        flags, justifyContentOffset);
  }
  BitfieldRef<YGJustify> justifyContent() {
    return {*this, justifyContentOffset};
  }

  YGAlign alignContent() const {
    return facebook::yoga::detail::getEnumData<YGAlign>(
        flags, alignContentOffset);
  }
  BitfieldRef<YGAlign> alignContent() { return {*this, alignContentOffset}; }

  YGAlign alignItems() const {
    return facebook::yoga::detail::getEnumData<YGAlign>(
        flags, alignItemsOffset);
  }
  BitfieldRef<YGAlign> alignItems() { return {*this, alignItemsOffset}; }

  YGAlign alignSelf() const {
    return facebook::yoga::detail::getEnumData<YGAlign>(flags, alignSelfOffset);
  }
  BitfieldRef<YGAlign> alignSelf() { return {*this, alignSelfOffset}; }

  YGPositionType positionType() const {
    return facebook::yoga::detail::getEnumData<YGPositionType>(
        flags, positionTypeOffset);
  }
  BitfieldRef<YGPositionType> positionType() {
    return {*this, positionTypeOffset};
  }

  YGWrap flexWrap() const {
    return facebook::yoga::detail::getEnumData<YGWrap>(flags, flexWrapOffset);
  }
  BitfieldRef<YGWrap> flexWrap() { return {*this, flexWrapOffset}; }

  YGOverflow overflow() const {
    return facebook::yoga::detail::getEnumData<YGOverflow>(
        flags, overflowOffset);
  }
  BitfieldRef<YGOverflow> overflow() { return {*this, overflowOffset}; }

  YGDisplay display() const {
    return facebook::yoga::detail::getEnumData<YGDisplay>(flags, displayOffset);
  }
  BitfieldRef<YGDisplay> display() { return {*this, displayOffset}; }

  YGFloatOptional flex() const { return flex_; }
  Ref<YGFloatOptional, &Style::flex_> flex() { return {*this}; }

  YGFloatOptional flexGrow() const { return flexGrow_; }
  Ref<YGFloatOptional, &Style::flexGrow_> flexGrow() { return {*this}; }

  YGFloatOptional flexShrink() const { return flexShrink_; }
  Ref<YGFloatOptional, &Style::flexShrink_> flexShrink() { return {*this}; }

  CompactValue flexBasis() const { return flexBasis_; }
  Ref<CompactValue, &Style::flexBasis_> flexBasis() { return {*this}; }

  const Edges& margin() const { return margin_; }
  IdxRef<YGEdge, &Style::margin_> margin() { return {*this}; }

  const Edges& position() const { return position_; }
  IdxRef<YGEdge, &Style::position_> position() { return {*this}; }

  const Edges& padding() const { return padding_; }
  IdxRef<YGEdge, &Style::padding_> padding() { return {*this}; }

  const Edges& border() const { return border_; }
  IdxRef<YGEdge, &Style::border_> border() { return {*this}; }

  const Gutters& gap() const { return gap_; }
  IdxRef<YGGutter, &Style::gap_> gap() { return {*this}; }

  const Dimensions& dimensions() const { return dimensions_; }
  IdxRef<YGDimension, &Style::dimensions_> dimensions() { return {*this}; }

  const Dimensions& minDimensions() const { return minDimensions_; }
  IdxRef<YGDimension, &Style::minDimensions_> minDimensions() {
    return {*this};
  }

  const Dimensions& maxDimensions() const { return maxDimensions_; }
  IdxRef<YGDimension, &Style::maxDimensions_> maxDimensions() {
    return {*this};
  }

  // Yoga specific properties, not compatible with flexbox specification
  YGFloatOptional aspectRatio() const { return aspectRatio_; }
  Ref<YGFloatOptional, &Style::aspectRatio_> aspectRatio() { return {*this}; }
};

YOGA_EXPORT bool operator==(const Style& lhs, const Style& rhs);
YOGA_EXPORT inline bool operator!=(const Style& lhs, const Style& rhs) {
  return !(lhs == rhs);
}
} // namespace facebook::yoga
