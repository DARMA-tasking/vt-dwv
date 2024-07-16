/*
//@HEADER
// *****************************************************************************
//
//                           test_info.cc
//             DARMA/vt-tv => Virtual Transport -- Task Visualizer
//
// Copyright 2019 National Technology & Engineering Solutions of Sandia, LLC
// (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
// Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact darma@sandia.gov
//
// *****************************************************************************
//@HEADER
*/

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vt-tv/api/info.h>
#include <vt-tv/api/phase_work.h>

#include <fmt-vt/format.h>

#include <string>
#include <filesystem>
#include <iostream>
#include <variant>
#include <set>

#include "../generator.h"


namespace vt::tv::tests::unit::api {

struct InfoTestParam
{
  public:

  InfoTestParam(size_t num_objects, size_t num_ranks, size_t num_phases):
    num_objects(num_objects), num_ranks(num_ranks), num_phases(num_phases) {

  }

  friend std::ostream& operator<< (std::ostream& stream, const InfoTestParam& param) {
    stream << "{num_objects: " << std::to_string(param.num_objects) << ", num_ranks: " << std::to_string(param.num_ranks) << ", num_phases: " << std::to_string(param.num_phases) << "}";
    return stream;
  }

  const size_t num_objects;
  const size_t num_ranks;
  const size_t num_phases;
};

/**
 * Provides unit tests for the vt::tv::api::Info class
 */
class InfoTest :public ::testing::TestWithParam<InfoTestParam> { };

/**
 * Test Info:getNumRanks returns same number of ranks as defined in the sample
 */
TEST_P(InfoTest, test_get_num_ranks) {
  InfoTestParam const & param = GetParam();
  Info info = Generator::makeInfo(param.num_objects, param.num_ranks, param.num_phases);
  EXPECT_EQ(info.getNumRanks(), param.num_ranks);
}

/**
 * Test Info:getNumPhases
 */
TEST_P(InfoTest, test_get_num_phases) {
  InfoTestParam const & param = GetParam();
  Info info = Generator::makeInfo(param.num_objects, param.num_ranks, param.num_phases);
  fmt::print("{}={}", param.num_phases, info.getNumPhases());

  EXPECT_EQ(info.getNumPhases(), param.num_phases);
}

/**
 * Test Info:getRankIDs
 */
TEST_P(InfoTest, test_get_rank_ids) {
  InfoTestParam const & param = GetParam();
  Info info = Generator::makeInfo(param.num_objects, param.num_ranks, param.num_phases);
  fmt::print("{}={}", param.num_phases, info.getNumPhases());

  auto rank_ids = std::vector<NodeType>();
  for (NodeType rank_id = 0; rank_id< param.num_ranks; rank_id++) {
    rank_ids.push_back(rank_id);
  }

  ASSERT_THAT(info.getRankIDs(), ::testing::UnorderedElementsAreArray(rank_ids));
}

/**
 * Test Info:getAllObjectIDs returns same items as defined in the sample
 */
TEST_P(InfoTest, test_get_all_object_ids) {
  InfoTestParam const & param = GetParam();

  auto objects = Generator::makeObjects(param.num_objects);
  auto ranks = Generator::makeRanks(objects, param.num_ranks, param.num_phases);
  auto object_info_map = Generator::makeObjectInfoMap(objects);
  auto info = Info(object_info_map, ranks);

  auto const& expected = object_info_map;
  auto const& actual = info.getAllObjectIDs();

  EXPECT_EQ(expected.size(), actual.size());
  for (auto& pair: expected) {
    auto object_id = pair.first;
    EXPECT_NE(actual.find(pair.first), actual.end()) << "Cannot find element with id " << object_id;
  }
}

/* Run Unit tests using different data sets as Tests params */
INSTANTIATE_TEST_SUITE_P(
    InfoTests,
    InfoTest,
    ::testing::Values<InfoTestParam>(
        // num_objects, num_ranks, num_phases
        InfoTestParam(0,0,0), // empty case
        InfoTestParam(2,5,1),
        InfoTestParam(6,1,1)
    ),
    [](const testing::TestParamInfo<InfoTest::ParamType>& info) {
      // test suffix
      return std::to_string(info.param.num_objects) + "_" +
             std::to_string(info.param.num_ranks) + "_" +
             std::to_string(info.param.num_phases);
    }
);

/**
 * Test Info::addInfo
 */
TEST_F(InfoTest, test_add_info) {

  Info info = Info();

  std::vector<size_t> idx;

  // Create object info and add to a map
  ObjectInfo oInfo = ObjectInfo(0, 0, true, idx);
  auto object_info_map = std::unordered_map<ElementIDType, ObjectInfo>();
  object_info_map.insert(std::make_pair(oInfo.getID(), oInfo));

  // Create PhaseWork including one object work
  auto object_work_objects = std::unordered_map<PhaseType, ObjectWork>();
  PhaseWork phase = PhaseWork(0, object_work_objects);

  // Create PhaseWork map
  auto phase_object_map = std::unordered_map<PhaseType, PhaseWork>();
  phase_object_map.insert(std::make_pair(0, phase));

  // Create a rank with id 0 and the PhaseWork map instance
  Rank rank = Rank(0, phase_object_map);

  EXPECT_EQ(info.getNumRanks(), 0);
  info.addInfo(object_info_map, rank);

  EXPECT_EQ(info.getNumRanks(), 1);
  EXPECT_EQ(info.getRank(0).getPhaseWork().size(), 1);

  // Rank already added. Expected assertion error (DEBUG).
  ASSERT_DEBUG_DEATH({ info.addInfo(object_info_map, rank); }, "Rank must not exist");
}

/**
 * Test Info:getNumPhases with inconstent rank phases.
 */
TEST_F(InfoTest, test_inconsistent_number_of_phases_across_ranks_throws_error) {
  Rank rank_0 = Rank(0, {{0, {}}}, {}); // Rank with 1 phase
  Rank rank_1 = Rank(1, {{0, {}}, {1, {}}}, {}); // Rank with 2 phases
  Info info = Info(Generator::makeObjectInfoMap(Generator::makeObjects(10)), { {0, rank_0}, {1, rank_1} } );
  EXPECT_THROW(info.getNumPhases(), std::runtime_error);
}

// TEST_F(InfoTest, test_must_fail) {
//   EXPECT_EQ(true, false);
// }

} // end namespace vt::tv::tests::unit
