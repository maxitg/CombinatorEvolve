#include "CombinatorEvolve.hpp"

#include <utility>
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

  const mint expressionsCount = tensorLength / 2;
  std::vector<CombinatorExpression> expressions(expressionsCount);
  for (mint expressionIndex = 0; expressionIndex < expressionsCount; ++expressionIndex) {
    expressions[expressionIndex].headID = getData(tensorData, tensorLength, readIndex++);
    expressions[expressionIndex].argumentID = getData(tensorData, tensorLength, readIndex++);
  }
  return expressions;
}

MTensor putCombinatorExpressions(const std::vector<CombinatorExpression>& expressions,
                                 const ExpressionID root,
                                 WolframLibraryData libData) {
  const mint dimensions[1] = {1 + static_cast<mint>(2 * expressions.size())};
  MTensor output;
  libData->MTensor_new(MType_Integer, 1, dimensions, &output);

  mint writeIndex = 0;
  mint position[1];
  position[0] = ++writeIndex;
  libData->MTensor_setInteger(output, position, root);
  for (const auto& expression : expressions) {
    position[0] = ++writeIndex;
    libData->MTensor_setInteger(output, position, expression.headID);
    position[0] = ++writeIndex;
    libData->MTensor_setInteger(output, position, expression.argumentID);
  }

  return output;
}

CombinatorRules getCombinatorRules(WolframLibraryData libData, MTensor expressionsTensor, MTensor rootsTensor) {
  std::vector<CombinatorExpression> expressions = getCombinatorExpressions(libData, expressionsTensor);
  std::vector<std::pair<ExpressionID, ExpressionID>> roots;

  mint readIndex = 0;
  mint tensorLength = libData->MTensor_getFlattenedLength(rootsTensor);
  mint* tensorData = libData->MTensor_getIntegerData(rootsTensor);

  const mint rulesCount = tensorLength / 2;
  roots.resize(rulesCount);
  for (mint ruleIndex = 0; ruleIndex < rulesCount; ++ruleIndex) {
    roots[ruleIndex].first = getData(tensorData, tensorLength, readIndex++);
    roots[ruleIndex].second = getData(tensorData, tensorLength, readIndex++);
  }

  return CombinatorRules(expressions, roots);
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
    mpz_pow_ui(digitSize.get_mpz_t(), mpz_class(2).get_mpz_t(), 31);
    while (remainingNumber > 0) {
      mpz_class thisDigit = remainingNumber & (digitSize - 1);
      digits.push_back(thisDigit.get_ui());
      remainingNumber >>= 31;
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

CombinatorSystem evolvedSystem(WolframLibraryData libData, mint argc, MArgument* argv) {
  if (argc != 7) {  // rule expressions + rule roots + init expressions + root + events count
    throw LIBRARY_FUNCTION_ERROR;
  }
  CombinatorRules rules = getCombinatorRules(libData, MArgument_getMTensor(argv[0]), MArgument_getMTensor(argv[1]));
  std::vector<CombinatorExpression> initialExpressions =
      getCombinatorExpressions(libData, MArgument_getMTensor(argv[2]));
  ExpressionID initialRoot = MArgument_getInteger(argv[3]);
  int64_t eventsCount = MArgument_getInteger(argv[4]);
  EvaluationOrder evaluationOrder = static_cast<EvaluationOrder>(MArgument_getInteger(argv[5]));
  int64_t maxLeafCount = MArgument_getInteger(argv[6]);

  CombinatorSystem system(initialExpressions, initialRoot, evaluationOrder);
  system.evolve(rules, eventsCount, shouldAbort(libData), maxLeafCount);
  return system;
}

int combinatorLeafCounts(WolframLibraryData libData, mint argc, MArgument* argv, MArgument result) {
  try {
    const auto system = evolvedSystem(libData, argc, argv);
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

int combinatorFinalExpression(WolframLibraryData libData, mint argc, MArgument* argv, MArgument result) {
  try {
    const auto system = evolvedSystem(libData, argc, argv);
    const auto finalExpressions = system.finalExpressionsAndRoot();
    MArgument_setMTensor(result, putCombinatorExpressions(finalExpressions.first, finalExpressions.second, libData));
  } catch (...) {
    return LIBRARY_FUNCTION_ERROR;
  }
  return LIBRARY_NO_ERROR;
}
}  // namespace CombinatorEvolve

EXTERN_C mint WolframLibrary_getVersion() { return WolframLibraryVersion; }

EXTERN_C int WolframLibrary_initialize([[maybe_unused]] WolframLibraryData libData) { return 0; }

EXTERN_C void WolframLibrary_uninitialize([[maybe_unused]] WolframLibraryData libData) { return; }

EXTERN_C int combinatorLeafCounts(WolframLibraryData libData, mint argc, MArgument* argv, MArgument result) {
  return CombinatorEvolve::combinatorLeafCounts(libData, argc, argv, result);
}

EXTERN_C int combinatorFinalExpression(WolframLibraryData libData, mint argc, MArgument* argv, MArgument result) {
  return CombinatorEvolve::combinatorFinalExpression(libData, argc, argv, result);
}
