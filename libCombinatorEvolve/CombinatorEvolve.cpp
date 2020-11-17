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

constexpr mint Int64VectorTypeID = -45735;
constexpr mint MPZVectorTypeID = -43083;

MTensor putVector(const std::vector<uint64_t>& data, WolframLibraryData libData) {
  size_t tensorLength = data.size();
  const mint dimensions[1] = {1 + static_cast<mint>(tensorLength)};
  MTensor output;
  libData->MTensor_new(MType_Integer, 1, dimensions, &output);

  mint writeIndex = 0;
  mint position[1];
  position[0] = ++writeIndex;
  libData->MTensor_setInteger(output, position, Int64VectorTypeID);
  for (const auto& number : data) {
    position[0] = ++writeIndex;
    libData->MTensor_setInteger(output, position, number);
  }
  return output;
}

MTensor putMPZVector(const std::vector<mpz_class>& data, WolframLibraryData libData) {
  std::vector<uint64_t> lengths;
  std::vector<uint64_t> allDigits;
  for (const auto& number : data) {
    std::vector<uint64_t> digits;
    mpz_class remainingNumber = number;
    mpz_class digitSize;
    mpz_pow_ui(digitSize.get_mpz_t(), mpz_class(2).get_mpz_t(), 63);
    while (remainingNumber > 0) {
      mpz_class thisDigit = remainingNumber & (digitSize - 1);
      digits.push_back(thisDigit.get_ui());
      remainingNumber >>= 63;
    }
    lengths.push_back(digits.size());
    allDigits.insert(allDigits.end(), digits.rbegin(), digits.rend());
  }

  const mint dimensions[1] = {static_cast<mint>(1 + 1 + lengths.size() + allDigits.size())};
  MTensor output;
  libData->MTensor_new(MType_Integer, 1, dimensions, &output);

  mint writeIndex = 0;
  mint position[1];
  position[0] = ++writeIndex;
  libData->MTensor_setInteger(output, position, MPZVectorTypeID);
  position[0] = ++writeIndex;
  libData->MTensor_setInteger(output, position, lengths.size());
  for (const auto& length : lengths) {
    position[0] = ++writeIndex;
    libData->MTensor_setInteger(output, position, length);
  }
  for (const auto& digit : allDigits) {
    position[0] = ++writeIndex;
    libData->MTensor_setInteger(output, position, static_cast<umint>(digit));
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
    try {
      std::vector<uint64_t> leafCounts = system.leafCounts();
      MArgument_setMTensor(result, putVector(leafCounts, libData));
    } catch (const CombinatorSystem::Error& error) {
      if (error != CombinatorSystem::Error::LeafCountOverflow) throw error;
      std::vector<mpz_class> leafCounts = system.leafCountsMPZ();
      MArgument_setMTensor(result, putMPZVector(leafCounts, libData));
    }
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
