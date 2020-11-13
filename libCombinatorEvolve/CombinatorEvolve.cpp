#include "CombinatorEvolve.hpp"

#include <vector>

#include "CombinatorSystem.hpp"
#include "Expression.hpp"
#include "IDTypes.hpp"

namespace CombinatorEvolve {
mint getData(const mint* data, const mint& length, const mint& index) {
  if (index >= length || index < 0) {
    throw LIBRARY_FUNCTION_ERROR;
  } else {
    return data[index];
  }
}

std::vector<CombinatorExpression> getCombinatorExpressions(WolframLibraryData libData, MTensor expressionsTensor) {
  mint readIndex = 0;
  mint tensorLength = libData->MTensor_getFlattenedLength(expressionsTensor);
  mint* tensorData = libData->MTensor_getIntegerData(expressionsTensor);

  const auto getDataFunc = [&tensorData, &tensorLength, &readIndex]() -> mint {
    return getData(tensorData, tensorLength, readIndex++);
  };

  const mint expressionsCount = tensorLength / 2;
  std::vector<CombinatorExpression> expressions(expressionsCount);
  for (mint expressionIndex = 0; expressionIndex < expressionsCount; ++expressionIndex) {
    expressions[expressionIndex].headID = getDataFunc();
    expressions[expressionIndex].argumentID = getDataFunc();
  }
  return expressions;
}

MTensor putVector(const std::vector<int64_t>& data, WolframLibraryData libData) {
  size_t tensorLength = data.size();

  const mint dimensions[1] = {static_cast<mint>(tensorLength)};
  MTensor output;
  libData->MTensor_new(MType_Integer, 1, dimensions, &output);

  mint writeIndex = 0;
  mint position[1];
  for (const auto& number : data) {
    position[0] = ++writeIndex;
    libData->MTensor_setInteger(output, position, number);
  }

  return output;
}

std::function<bool()> shouldAbort(WolframLibraryData libData) {
  return [libData]() { return static_cast<bool>(libData->AbortQ()); };
}

int skCombinatorLeftmostOutermostLeafCounts(WolframLibraryData libData, mint argc, MArgument* argv, MArgument result) {
  if (argc != 3) {  // init expressions + root + events count
    return LIBRARY_FUNCTION_ERROR;
  }
  try {
    std::vector<CombinatorExpression> initialExpressions =
        getCombinatorExpressions(libData, MArgument_getMTensor(argv[0]));
    ExpressionID initialRoot = MArgument_getInteger(argv[1]);
    int64_t eventsCount = MArgument_getInteger(argv[2]);

    CombinatorSystem system(initialExpressions, initialRoot);
    system.evolve(eventsCount, shouldAbort(libData));
    std::vector<int64_t> leafCounts = system.leafCounts();

    MArgument_setMTensor(result, putVector(leafCounts, libData));
  } catch (...) {
    return LIBRARY_FUNCTION_ERROR;
  }

  return LIBRARY_NO_ERROR;
}
}  // namespace CombinatorEvolve

EXTERN_C mint WolframLibrary_getVersion() { return WolframLibraryVersion; }

EXTERN_C int WolframLibrary_initialize([[maybe_unused]] WolframLibraryData libData) { return 0; }

EXTERN_C void WolframLibrary_uninitialize([[maybe_unused]] WolframLibraryData libData) { return; }

EXTERN_C int skCombinatorLeftmostOutermostLeafCounts(WolframLibraryData libData,
                                                     mint argc,
                                                     MArgument* argv,
                                                     MArgument result) {
  return CombinatorEvolve::skCombinatorLeftmostOutermostLeafCounts(libData, argc, argv, result);
}
