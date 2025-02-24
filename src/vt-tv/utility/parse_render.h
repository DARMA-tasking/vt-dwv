/*
//@HEADER
// *****************************************************************************
//
//                                parse_render.h
//             DARMA/vt-tv => Virtual Transport -- Task Visualizer
//
// Copyright 2019-2024 National Technology & Engineering Solutions of Sandia, LLC
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

#if !defined INCLUDED_VT_TV_UTILITY_PARSE_RENDER_H
#define INCLUDED_VT_TV_UTILITY_PARSE_RENDER_H

#include "vt-tv/api/info.h"

#include <yaml-cpp/yaml.h>

#include <limits>
#include <memory>

#if VT_TV_OPENMP_ENABLED
#include <omp.h>
#endif
namespace vt::tv::utility {

/**
 * \struct ParseRender
 *
 * \brief Parse YAML file and render based on configuration
 */
struct ParseRender {
  /**
   * \brief Construct the class
   *
   * \param[in] in_filename the yaml file name to read
   */
  ParseRender(std::string const& in_filename) : filename_(in_filename) { }

  /**
   * \brief Parse yaml file and render
   *
   * \param[in] phase_id the phase ID
   * \param[in] info the data to render
   *
   * \note If \c phase_id is max then all phases will be rendered
   */
  void parseAndRender(
    PhaseType phase_id = std::numeric_limits<PhaseType>::max(),
    std::unique_ptr<Info> info = nullptr);

private:
  std::string filename_;
};

} /* end namespace vt::tv::utility */

#endif /*INCLUDED_VT_TV_UTILITY_PARSE_RENDER_H*/
