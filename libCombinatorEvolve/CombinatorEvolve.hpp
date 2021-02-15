#ifndef LIBCOMBINATOREVOLVE_COMBINATOREVOLVE_HPP_
#define LIBCOMBINATOREVOLVE_COMBINATOREVOLVE_HPP_

#include "WolframHeaders/WolframLibrary.h"

EXTERN_C DLLEXPORT mint WolframLibrary_getVersion();

EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData);

EXTERN_C DLLEXPORT void WolframLibrary_uninitialize(WolframLibraryData libData);

EXTERN_C DLLEXPORT int combinatorLeafCounts(WolframLibraryData libData, mint argc, MArgument* argv, MArgument result);

EXTERN_C DLLEXPORT int combinatorFinalExpression(WolframLibraryData libData,
                                                 mint argc,
                                                 MArgument* argv,
                                                 MArgument result);

#endif  // LIBCOMBINATOREVOLVE_COMBINATOREVOLVE_HPP_
