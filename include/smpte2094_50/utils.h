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

// Utilities to manipulate SMPTE ST 2094-50 metadata.

#ifndef LIBSMPTE2094_50_SRC_UTILS_UTILS_H_
#define LIBSMPTE2094_50_SRC_UTILS_UTILS_H_

#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "smpte2094_50/smpte2094_50.h"

namespace smpte2094_50 {

// Serializes metadata to its SMPTE ST 2094-50 bitstream representation.
absl::StatusOr<std::string> ToSt209450(const DynamicMetadata& metadata);

// Returns a DynamicMetadata from a SMPTE ST 2094-50 bitstream.
absl::StatusOr<DynamicMetadata> FromSt209450(absl::string_view data);

// Populates the metadata with the Reference White Tone Mapping
// (RWTM) parameters, based on the metadata's baseline_hdr_headroom_log2.
// Removes existing rules if any.
void PopulateUsingRwtm(DynamicMetadata& metadata);

// Populates the slope values for rules that have use_pchip_slope = true.
// For those rules, the existing slope values, if any, are overwritten.
absl::Status PopulatePchipSlopes(DynamicMetadata& metadata);

// Calls PopulateUsingRwtm() if use_reference_white_tone_mapping_flag is true,
// and PopulatePchipSlopes().
absl::Status PopulateImplicitParameters(DynamicMetadata& metadata);

// Returns true if the metadata satisfies all the mandatory parameter
// constraints in SMPTE ST 2094-50, including constraints for serialization.
bool IsValid(const DynamicMetadata& metadata);

}  // namespace smpte2094_50

#endif  // LIBSMPTE2094_50_SRC_UTILS_UTILS_H_
