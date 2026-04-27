/**
 * Copyright 2026 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Base types, according to SMPTE ST 2094-50.

#ifndef LIBSMPTE2094_50_INCLUDE_SMPTE2094_50_H_
#define LIBSMPTE2094_50_INCLUDE_SMPTE2094_50_H_

#include <array>
#include <vector>

namespace smpte2094_50 {

// A 2D point with optional slope.
struct ControlPoint {
  float x;  // in [0., 64.]
  float y;  // in [-6., 6.]
  // Used if use_pchip_slope is false.
  float m;
};

// All values must be in [0., 1.] and sum to 1.
struct ComponentMix {
  std::array<float, 3> rgb;
  float max;
  float min;
  float component;
};

// Alternative tone mapping rule.
struct ToneMappingRule {
  float alternate_hdr_headroom_log2;  // in [0., 6.]
  std::vector<ControlPoint> curve;    // up to 32 points.
  bool use_pchip_slope;               // if false, curve[i].theta is used.
  ComponentMix mix;
};

// SMPTE 2094-50 metadata.
struct DynamicMetadata {
  static constexpr int kApplicationVersion = 0;
  static constexpr int kMinimumApplicationVersion = 0;
  static constexpr int kDefaultHdrReferenceWhite = 203;
  static constexpr std::array<float, 8> kChromaticitiesSrgb = {
      0.64f, 0.33f, 0.3f,    0.6f,
      0.15f, 0.06f, 0.3127f, 0.329f};  // Rec. 709 (sRGB)
  static constexpr std::array<float, 8> kChromaticitiesDisplayP3 = {
      0.68f, 0.32f, 0.265f,  0.69f,
      0.15f, 0.06f, 0.3127f, 0.329f};  // Display P3
  static constexpr std::array<float, 8> kChromaticitiesRec2020 = {
      0.708f, 0.292f, 0.17f,   0.797f,
      0.131f, 0.046f, 0.3127f, 0.329f};  // Rec. 2020

  // Luminance level in cd/m2 used as an anchor to normalize content when
  // converting to the gain application color space for headroom-adaptive tone
  // mapping and compositing.
  float hdr_reference_white = kDefaultHdrReferenceWhite;
  // If false, the metadata does not contain any adaptive tone mapping
  // information, and fields below are unused.
  bool has_adaptive_tone_map_flag = true;
  // HDR headroom of the baseline image. Valid range is [0., 6.].
  float baseline_hdr_headroom_log2 = 0.0f;
  // If true, the tone mapping method used is the Reference White Tone Mapping
  // operator (RWTM). The fields below should be unset and/or filled with the
  // RWTM values, see PopulateUsingRwtm().
  bool use_reference_white_tone_mapping_flag = false;
  // X and y chromaticity coordinates of the gain application color space:
  // red_x, red_y, green_x, green_y, blue_x, blue_y, white_x, white_y.
  std::array<float, 8> gain_application_space_chromaticities =
      kChromaticitiesRec2020;
  // Maximum valid size: 4 rules.
  std::vector<ToneMappingRule> rules;
};

}  // namespace smpte2094_50

#endif  // LIBSMPTE2094_50_INCLUDE_SMPTE2094_50_H_
