# Copyright 2026 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.

enable_testing()

macro(add_gtest TEST_NAME)
    add_executable(${TEST_NAME} ${TEST_NAME}.cc)
    target_link_libraries(${TEST_NAME} PRIVATE GTest::GTest GTest::Main smpte2094-50 smpte2094-50_utils)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endmacro()

find_package(GTest)

add_gtest(base_test)
add_gtest(pchip_test)
add_gtest(utils_test)
