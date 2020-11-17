#include "Rule.hpp"

#include <algorithm>
#include <unordered_map>
#include <unordered_set>

namespace CombinatorEvolve {
bool InputShortcut::isAvailable() const { return nestedHeadID != nullExpression; }

ExpressionReference ExpressionReference::fromConstant(const ExpressionID constantID) {
  return {ExpressionReferenceType::Constant, constantID, {}, -1};
}

ExpressionReference ExpressionReference::fromPath(const std::vector<int>& pathToExisting) {
  return {ExpressionReferenceType::ExistingExpression, nullExpression, pathToExisting, -1};
}

ExpressionReference ExpressionReference::fromCreatedExpression(const int createdExpressionIndex) {
  return {ExpressionReferenceType::CreatedExpression, nullExpression, {}, createdExpressionIndex};
}

namespace {
ExpressionID getVariablesCount(const std::vector<CombinatorExpression>& expressions) {
  ExpressionID result = expressions.size();
  for (const CombinatorExpression& expression : expressions) {
    result = std::max(result, std::max(expression.argumentID, expression.headID) + 1);
  }
  return result;
}

InputShortcut getInputShortcut(const std::vector<CombinatorExpression>& expressions,
                               const ExpressionID root,
                               const int currentDepth,
                               std::unordered_set<ExpressionID>* encounteredPatterns) {
  if (root < 0) {
    return {currentDepth, root};
  } else if (root < expressions.size() && expressions[root].argumentID >= expressions.size() &&
             !encounteredPatterns->count(expressions[root].argumentID)) {
    encounteredPatterns->insert(expressions[root].argumentID);
    return getInputShortcut(expressions, expressions[root].headID, currentDepth + 1, encounteredPatterns);
  } else {
    return {-1, nullExpression};
  }
}

std::vector<InputShortcut> getInputShortcuts(const std::vector<CombinatorExpression>& expressions,
                                             const std::vector<std::pair<ExpressionID, ExpressionID>>& roots) {
  std::vector<InputShortcut> result;
  for (const auto& inputOutputRoots : roots) {
    std::unordered_set<ExpressionID> encounteredPatterns;
    result.push_back(getInputShortcut(expressions, inputOutputRoots.first, 0, &encounteredPatterns));
  }
  return result;
}

void findPatterns(const ExpressionID inputRoot,
                  const std::vector<CombinatorExpression>& expressions,
                  std::vector<int>* currentPath,
                  std::unordered_map<ExpressionID, std::vector<int>>* inputPatternsToPaths) {
  if (inputRoot >= 0 && inputRoot < expressions.size()) {
    currentPath->push_back(0);
    findPatterns(expressions[inputRoot].headID, expressions, currentPath, inputPatternsToPaths);
    currentPath->pop_back();
    currentPath->push_back(1);
    findPatterns(expressions[inputRoot].argumentID, expressions, currentPath, inputPatternsToPaths);
    currentPath->pop_back();
  } else if (inputRoot >= expressions.size()) {
    inputPatternsToPaths->insert({inputRoot, *currentPath});
  }
}

ExpressionReference makeBlueprints(
    const ExpressionID outputRoot,
    const std::vector<CombinatorExpression>& expressions,
    const std::unordered_map<ExpressionID, std::vector<int>>& inputPatternsToPaths,
    std::vector<std::pair<ExpressionReference, ExpressionReference>>* newExpressionReferences) {
  if (outputRoot < 0) {
    return ExpressionReference::fromConstant(outputRoot);
  } else if (outputRoot >= expressions.size()) {
    return ExpressionReference::fromPath(inputPatternsToPaths.at(outputRoot));
  } else {
    newExpressionReferences->push_back(
        {makeBlueprints(expressions[outputRoot].headID, expressions, inputPatternsToPaths, newExpressionReferences),
         makeBlueprints(
             expressions[outputRoot].argumentID, expressions, inputPatternsToPaths, newExpressionReferences)});
    return ExpressionReference::fromCreatedExpression(static_cast<int>(newExpressionReferences->size()) - 1);
  }
}

OutputBlueprint getOutputBlueprint(const std::vector<CombinatorExpression>& expressions,
                                   const ExpressionID inputRoot,
                                   const ExpressionID outputRoot) {
  std::unordered_map<ExpressionID, std::vector<int>> inputPatternsToPaths;
  std::vector<int> currentPath = {};
  findPatterns(inputRoot, expressions, &currentPath, &inputPatternsToPaths);

  std::vector<std::pair<ExpressionReference, ExpressionReference>> newExpressionReferences;
  ExpressionReference rootReference =
      makeBlueprints(outputRoot, expressions, inputPatternsToPaths, &newExpressionReferences);

  return {newExpressionReferences, rootReference};
}

std::vector<OutputBlueprint> getOutputBlueprints(const std::vector<CombinatorExpression>& expressions,
                                                 const std::vector<std::pair<ExpressionID, ExpressionID>>& roots) {
  std::vector<OutputBlueprint> result;
  for (const auto& inputOutputRoots : roots) {
    result.push_back(getOutputBlueprint(expressions, inputOutputRoots.first, inputOutputRoots.second));
  }
  return result;
}
}  // namespace

CombinatorRules::CombinatorRules(const std::vector<CombinatorExpression>& expressions,
                                 const std::vector<std::pair<ExpressionID, ExpressionID>>& roots)
    : expressions(expressions),
      roots(roots),
      variablesCount(getVariablesCount(expressions)),
      shortcuts(getInputShortcuts(expressions, roots)),
      blueprints(getOutputBlueprints(expressions, roots)) {}

}  // namespace CombinatorEvolve
