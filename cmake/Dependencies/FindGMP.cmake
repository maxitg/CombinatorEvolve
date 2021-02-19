find_package(PkgConfig REQUIRED)
pkg_check_modules(PKG gmp gmpxx)

find_path(GMP_INCLUDE_DIR gmp.h HINTS ${PKG_gmp_INCLUDEDIR})
find_library(GMP_LIB NAMES gmp HINTS ${PKG_gmp_LIBDIR} C:/Users/circleci/msys64/mingw64/bin)

message(STATUS "GMP_INCLUDE_DIR: ${GMP_INCLUDE_DIR}")
message(STATUS "GMP_LIB: ${GMP_LIB}")
message(STATUS "PKG_gmp_INCLUDEDIR: ${PKG_gmp_INCLUDEDIR}")
message(STATUS "PKG_gmp_LIBDIR: ${PKG_gmp_LIBDIR}")

find_path(GMPXX_INCLUDE_DIR gmpxx.h HINTS ${PKG_gmpxx_INCLUDEDIR})
find_library(GMPXX_LIB NAMES gmpxx HINTS ${PKG_gmpxx_LIBDIR} C:/Users/circleci/msys64/mingw64/bin)

if(GMP_FOUND)
  if(NOT TARGET GMP)
    add_library(GMP UNKNOWN IMPORTED)
    set_target_properties(GMP PROPERTIES
      IMPORTED_LOCATION ${GMPXX_LIB}
      INTERFACE_INCLUDE_DIRECTORIES ${GMPXX_INCLUDE_DIR}
      INTERFACE_LINK_LIBRARIES GMP
    )
  endif()
endif()
