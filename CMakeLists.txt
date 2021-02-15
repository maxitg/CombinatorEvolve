cmake_minimum_required(VERSION 3.13)
project(CombinatorEvolve
    VERSION 0.3.0
    LANGUAGES CXX)
message(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION}")

include(GNUInstallDirs) # Define CMAKE_INSTALL_xxx: LIBDIR, INCLUDEDIR
set(CombinatorEvolve_export_file "${PROJECT_BINARY_DIR}/CombinatorEvolveTargets.cmake")

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
        STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY
        STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
    message(STATUS "${PROJECT_NAME} using CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")
endif()

add_library(GMP STATIC IMPORTED)
find_library(GMP_LIBRARY gmp /usr/local/lib)

# Set extra compile options
option(COMBINATOR_EVOLVE_ENABLE_ALLWARNINGS
       "Add all compiler warnings to the C++ targets"
       OFF)
if(MSVC)
  list(APPEND COMBINATOR_EVOLVE_DEFAULT_COMPILE_OPTIONS /EHsc)
  if(COMBINATOR_EVOLVE_ENABLE_ALLWARNINGS)
    list(APPEND COMBINATOR_EVOLVE_DEFAULT_COMPILE_OPTIONS /W4 /WX)
  endif()
else()
  if(COMBINATOR_EVOLVE_ENABLE_ALLWARNINGS)
    list(APPEND COMBINATOR_EVOLVE_DEFAULT_COMPILE_OPTIONS
         -Wall -Wextra -Werror -pedantic -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2
         -Winit-self -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow
         -Wsign-promo -Wswitch-default -Wundef -Wno-unused)
  endif()
endif()

set(COMBINATOR_EVOLVE_EXTRA_COMPILE_OPTIONS CACHE STRING "Add extra compile options to the C++ targets")
set(COMBINATOR_EVOLVE_COMPILE_OPTIONS_OVERRIDE CACHE STRING "Overwrite compile options for the C++ targets")
if(COMBINATOR_EVOLVE_COMPILE_OPTIONS_OVERRIDE)
  set(COMBINATOR_EVOLVE_COMPILE_OPTIONS ${COMBINATOR_EVOLVE_COMPILE_OPTIONS_OVERRIDE})
else()
  set(COMBINATOR_EVOLVE_COMPILE_OPTIONS
      ${COMBINATOR_EVOLVE_DEFAULT_COMPILE_OPTIONS}
      ${COMBINATOR_EVOLVE_EXTRA_COMPILE_OPTIONS})
endif()

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

message(STATUS "COMBINATOR_EVOLVE_COMPILE_OPTIONS: ${COMBINATOR_EVOLVE_COMPILE_OPTIONS}")

set(libCombinatorEvolve_headers
    CombinatorEvolve.hpp
    CombinatorSystem.hpp
    Expression.hpp
    IDTypes.hpp
    Rule.hpp
    )
set(libCombinatorEvolve_sources
    CombinatorEvolve.cpp
    CombinatorSystem.cpp
    Rule.cpp
    )
list(TRANSFORM libCombinatorEvolve_headers PREPEND "libCombinatorEvolve/")
list(TRANSFORM libCombinatorEvolve_sources PREPEND "libCombinatorEvolve/")

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Dependencies ${CMAKE_MODULE_PATH})
find_package(GMP REQUIRED)

add_library(CombinatorEvolve ${libCombinatorEvolve_sources})
target_include_directories(CombinatorEvolve PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libCombinatorEvolve>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  ${GMP_INCLUDE_DIR})
target_compile_options(CombinatorEvolve PRIVATE ${COMBINATOR_EVOLVE_COMPILE_OPTIONS})
target_link_libraries(CombinatorEvolve ${GMP_LIB})

set(COMBINATOR_EVOLVE_LIBRARIES CombinatorEvolve)

install(TARGETS ${COMBINATOR_EVOLVE_LIBRARIES}
        EXPORT CombinatorEvolveTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT runtime
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT runtime
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT development
        )

install(FILES ${libCombinatorEvolve_headers}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/CombinatorEvolve
  COMPONENT development
  )

# export to the build tree
export( TARGETS ${COMBINATOR_EVOLVE_LIBRARIES}
  NAMESPACE CombinatorEvolve::
  APPEND FILE ${CombinatorEvolve_export_file})

# INSTALL
set(install_cmake_dir "${CMAKE_INSTALL_LIBDIR}/cmake/CombinatorEvolve")

install (EXPORT CombinatorEvolveTargets
  NAMESPACE CombinatorEvolve::
  DESTINATION ${install_cmake_dir} )

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CombinatorEvolveConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/CombinatorEvolveConfigVersion.cmake
  DESTINATION ${install_cmake_dir} )

include(CMakePackageConfigHelpers)

write_basic_package_version_file(CombinatorEvolveConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)

# Build tree
set(CombinatorEvolve_TARGETS_FILE ${CombinatorEvolve_export_file})
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/CombinatorEvolveConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/CombinatorEvolveConfig.cmake
  INSTALL_DESTINATION ${install_cmake_dir}
  PATH_VARS CombinatorEvolve_TARGETS_FILE
  NO_CHECK_REQUIRED_COMPONENTS_MACRO # CombinatorEvolve does not provide components
  INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}
  )

# Install tree
set(CombinatorEvolve_TARGETS_FILE ${CMAKE_INSTALL_PREFIX}/${install_cmake_dir}/CombinatorEvolveTargets.cmake)
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/CombinatorEvolveConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CombinatorEvolveConfig.cmake
  INSTALL_DESTINATION ${install_cmake_dir}
  PATH_VARS CombinatorEvolve_TARGETS_FILE
  NO_CHECK_REQUIRED_COMPONENTS_MACRO # CombinatorEvolve does not provide components
  )

# Add custom target to only install component: runtime (libraries)
add_custom_target(set-replace-install-runtime
  ${CMAKE_COMMAND}
  -DCMAKE_INSTALL_COMPONENT=runtime
  -P "${PROJECT_BINARY_DIR}/cmake_install.cmake"
  DEPENDS ${COMBINATOR_EVOLVE_LIBRARIES}
  )
message(STATUS "COMBINATOR_EVOLVE_LIBRARIES: ${COMBINATOR_EVOLVE_LIBRARIES}")
add_dependencies(set-replace-install-runtime ${COMBINATOR_EVOLVE_LIBRARIES})
