ARG BASE_IMAGE=lifflander1/vt:ubuntu_22.04-gcc_11-vtk_9.2.2-py_3.8
ARG VT_TV_TESTS_ENABLED=OFF
ARG VT_TV_COVERAGE_ENABLED=OFF
ARG VT_TV_TEST_PYTHON_BINDINGS=OFF

FROM ${BASE_IMAGE} AS base

# setup requirements for rendering tests (xvfb) + coverage report (lcov)
RUN apt-get update && apt-get install -y \
    xvfb \
    lcov

COPY . /opt/src/vt-tv
RUN mkdir -p /opt/build/vt-tv

# Build
FROM base AS build
ARG VT_TV_COVERAGE_ENABLED=OFF
ARG VT_TV_TESTS_ENABLED=OFF
RUN VT_TV_COVERAGE_ENABLED=$VT_TV_COVERAGE_ENABLED bash /opt/src/vt-tv/ci/build.sh

# Unit tests
FROM build AS test-cpp
ARG VT_TV_COVERAGE_ENABLED=OFF
ARG VT_TV_TESTS_ENABLED=OFF
RUN VT_TV_COVERAGE_ENABLED=$VT_TV_COVERAGE_ENABLED bash /opt/src/vt-tv/ci/test_cpp.sh
RUN bash /opt/src/vt-tv/ci/test_python.sh

# Artifacts
FROM scratch AS artifacts
COPY --from=test-cpp /tmp/artifacts /tmp/artifacts
