#include "CombinatorSystem.hpp"

#include <algorithm>
#include <limits>
#include <unordered_set>

namespace CombinatorEvolve {
struct PairHash {
  template <class T1, class T2>
  size_t operator()(const std::pair<T1, T2>& pair) const {
    auto hash1 = std::hash<T1>{}(pair.first);
    auto hash2 = std::hash<T2>{}(pair.second);
    return hash1 ^ hash2;
  }
};

struct ExpressionWithMetadata {
  CombinatorExpression expression;
  ExpressionID successor;
  ExpressionID isFullyEvolved;
  int64_t leafCount = -1;

  explicit ExpressionWithMetadata(const CombinatorExpression& rawExpression)
      : expression(rawExpression), successor(nullExpression), isFullyEvolved(false) {}
};

class CombinatorSystem::Implementation {
 private:
  std::vector<ExpressionWithMetadata> expressions_;
  std::unordered_map<std::pair<ExpressionID, ExpressionID>, ExpressionID, PairHash> contentsToExpressions_;
  std::vector<ExpressionID> evolutionRoots_;
  EvaluationOrder evaluationOrder_;

 public:
  Implementation(const std::vector<CombinatorExpression>& initialExpressions,
                 const ExpressionID initialRoot,
                 const EvaluationOrder evaluationOrder)
      : evolutionRoots_({initialRoot}), evaluationOrder_(evaluationOrder) {
    for (int i = 0; i < initialExpressions.size(); ++i) {
      expressions_.push_back(ExpressionWithMetadata(initialExpressions[i]));
      contentsToExpressions_[{expressions_[i].expression.headID, expressions_[i].expression.argumentID}] = i;
    }
  }

  int64_t evolve(const CombinatorRules& rules, const int64_t eventsCount, const std::function<bool()>& shouldAbort, const int64_t maxLeafCount) {
    int64_t eventsDone = 0;

    // This is actually local to the matching algorithm. However, constantly reallocating memory for this takes a lot
    // of time. So, it is created here. For parallelization, there will need to be one copy of these per each thread.
    std::vector<ExpressionID> inputPatternsToExpressions(rules.variablesCount - rules.expressions.size(),
                                                         nullExpression);

    while (eventsDone < eventsCount) {
      if (shouldAbort()) return eventsDone;
      if (evolveOnce(rules, evolutionRoots_.back(), &inputPatternsToExpressions)) {
        evolutionRoots_.push_back(successor(evolutionRoots_.back()));
        expressions_[evolutionRoots_[evolutionRoots_.size() - 2]].successor = nullExpression;
        ++eventsDone;
        if (maxLeafCount >= 0 && leafCount(evolutionRoots_.back()) > maxLeafCount) {
          throw Error::EventsCountExceeded;
        }
      } else {
        return eventsDone;
      }
    }
    return eventsDone;
  }

  std::pair<std::vector<CombinatorExpression>, ExpressionID> finalExpressionsAndRoot() const {
    std::vector<CombinatorExpression> result;
    std::unordered_map<ExpressionID, int> expressionIDToIndex;
    collectExpressions(evolutionRoots_.back(), &result, &expressionIDToIndex);
    for (auto& expression : result) {
      expression.headID = remapExpressionIDToIndex(expression.headID, expressionIDToIndex);
      expression.argumentID = remapExpressionIDToIndex(expression.argumentID, expressionIDToIndex);
    }
    return {result, remapExpressionIDToIndex(evolutionRoots_.back(), expressionIDToIndex)};
  }

  template <typename Number>
  std::vector<Number> leafCounts() {
    std::vector<Number> result;
    std::vector<Number> subexpressionLeafCounts(expressions_.size(), -1);
    for (auto root : evolutionRoots_) {
      result.push_back(leafCount<Number>(root, &subexpressionLeafCounts));
    }
    return result;
  }

 private:
  ExpressionID head(const ExpressionID root) const {
    if (root < 0) {
      return nullExpression;
    } else {
      return expressions_[root].expression.headID;
    }
  }

  ExpressionID argument(const ExpressionID root) const {
    if (root < 0) {
      return nullExpression;
    } else {
      return expressions_[root].expression.argumentID;
    }
  }

  ExpressionID successor(const ExpressionID root) {
    if (root < 0) {
      return nullExpression;
    } else {
      return expressions_[root].successor;
    }
  }

  int evolveOnce(const CombinatorRules& rules,
                 const ExpressionID root,
                 std::vector<ExpressionID>* inputPatternsToExpressions) {
    if (root < 0) return 0;
    if (expressions_[root].isFullyEvolved) return 0;

    bool didEvolve = false;

    if (evaluationOrder_ == EvaluationOrder::LeftmostOutermost) {
      didEvolve = tryEvolvingUsingRules(rules, root, inputPatternsToExpressions) ||
                  tryEvolvingDownstream(rules, root, inputPatternsToExpressions);
    } else {
      didEvolve = tryEvolvingDownstream(rules, root, inputPatternsToExpressions) ||
                  tryEvolvingUsingRules(rules, root, inputPatternsToExpressions);
    }

    if (didEvolve) {
      return 1;
    } else {
      if (isDownstreamEvolutionComplete(root)) expressions_[root].isFullyEvolved = true;
      return 0;
    }
  }

  bool tryEvolvingUsingRules(const CombinatorRules& rules,
                             const ExpressionID root,
                             std::vector<ExpressionID>* inputPatternsToExpressions) {
    for (int i = 0; i < rules.roots.size(); ++i) {
      if (tryEvolvingUsingRule(rules.expressions,
                               rules.roots[i].first,
                               rules.shortcuts[i],
                               rules.blueprints[i],
                               root,
                               inputPatternsToExpressions)) {
        return true;
      }
    }
    return false;
  }

  bool tryEvolvingDownstream(const CombinatorRules& rules,
                             const ExpressionID root,
                             std::vector<ExpressionID>* inputPatternsToExpressions) {
    if (evolveOnce(rules, head(root), inputPatternsToExpressions) ||
        evolveOnce(rules, argument(root), inputPatternsToExpressions)) {
      updateFromDownstream(root);
      return true;
    }
    return false;
  }

  int tryEvolvingUsingRule(const std::vector<CombinatorExpression>& ruleExpressions,
                           const ExpressionID inputRoot,
                           const InputShortcut& shortcut,
                           const OutputBlueprint& blueprint,
                           const ExpressionID root,
                           std::vector<ExpressionID>* inputPatternsToExpressions) {
    if (!matchExpression(root, inputRoot, shortcut, ruleExpressions, inputPatternsToExpressions)) return 0;
    expressions_[root].successor = createExpressions(blueprint.newRoot, blueprint.newExpressionBlueprints, root);
    return 1;
  }

  bool matchExpression(ExpressionID root,
                       ExpressionID patternRoot,
                       const InputShortcut& shortcut,
                       const std::vector<CombinatorExpression>& patternExpressions,
                       std::vector<ExpressionID>* inputPatternsToExpressions) {
    if (shortcut.isAvailable()) {
      return isMatchableUsingShortcut(root, shortcut);
    } else {
      std::fill(inputPatternsToExpressions->begin(), inputPatternsToExpressions->end(), nullExpression);
      return matchWithoutShortcut(root, patternRoot, patternExpressions, inputPatternsToExpressions);
    }
  }

  bool isMatchableUsingShortcut(ExpressionID root, const InputShortcut& shortcut) {
    ExpressionID expressionToCheck = root;
    for (int i = 0; i < shortcut.headDepth; ++i) {
      expressionToCheck = head(expressionToCheck);
    }
    return expressionToCheck == shortcut.nestedHeadID;
  }

  bool matchWithoutShortcut(ExpressionID root,
                            ExpressionID patternRoot,
                            const std::vector<CombinatorExpression>& patternExpressions,
                            std::vector<ExpressionID>* inputPatternsToExpressions) {
    if (patternRoot < 0) {
      return root == patternRoot;
    } else if (patternRoot >= patternExpressions.size()) {  // pattern
      if ((*inputPatternsToExpressions)[patternRoot - patternExpressions.size()] != nullExpression) {
        return (*inputPatternsToExpressions)[patternRoot - patternExpressions.size()] == root;
      } else {
        (*inputPatternsToExpressions)[patternRoot - patternExpressions.size()] = root;
        return true;
      }
    } else {
      return matchWithoutShortcut(
                 head(root), patternExpressions[patternRoot].headID, patternExpressions, inputPatternsToExpressions) &&
             matchWithoutShortcut(argument(root),
                                  patternExpressions[patternRoot].argumentID,
                                  patternExpressions,
                                  inputPatternsToExpressions);
    }
  }

  ExpressionID createExpressions(
      const ExpressionReference currentRoot,
      const std::vector<std::pair<ExpressionReference, ExpressionReference>>& otherExpressionBlueprints,
      const ExpressionID fullDAGRoot) {
    if (currentRoot.type == ExpressionReferenceType::Constant) {
      return currentRoot.constantID;
    } else if (currentRoot.type == ExpressionReferenceType::ExistingExpression) {
      return followPath(fullDAGRoot, currentRoot.pathToExisting);
    } else {
      // we need to create a new expression
      return newExpression(createExpressions(otherExpressionBlueprints[currentRoot.createdExpressionIndex].first,
                                             otherExpressionBlueprints,
                                             fullDAGRoot),
                           createExpressions(otherExpressionBlueprints[currentRoot.createdExpressionIndex].second,
                                             otherExpressionBlueprints,
                                             fullDAGRoot));
    }
  }

  ExpressionID followPath(const ExpressionID root, const std::vector<int>& path) {
    ExpressionID result = root;
    for (const auto direction : path) {
      if (direction == 0) {
        result = head(result);
      } else {
        result = argument(result);
      }
    }
    return result;
  }

  // Will use existing one if possible
  ExpressionID newExpression(const ExpressionID head, const ExpressionID argument) {
    if (contentsToExpressions_.count({head, argument})) {
      return contentsToExpressions_[{head, argument}];
    } else {
      expressions_.push_back(ExpressionWithMetadata({head, argument}));
      return expressions_.size() - 1;
    }
  }

  int64_t leafCount(const ExpressionID expr) {
    if (expr < 0) {
      return 1;
    } else if (expressions_[expr].leafCount > 0) {
      return expressions_[expr].leafCount;
    } else {
      const auto result = leafCount(head(expr)) + leafCount(argument(expr));
      expressions_[expr].leafCount = result;
      return result;
    }
  }

  void updateFromDownstream(const ExpressionID root) {
    ExpressionID headSuccessor = successor(head(root));
    ExpressionID argumentSuccessor = successor(argument(root));

    CombinatorExpression newContents;
    if (headSuccessor != nullExpression) {
      newContents = {headSuccessor, argument(root)};
      expressions_[head(root)].successor = nullExpression;
    } else if (argumentSuccessor != nullExpression) {
      newContents = {head(root), argumentSuccessor};
      expressions_[argument(root)].successor = nullExpression;
    } else {
      throw Error::InconsistentDownstreamUpdate;
    }

    expressions_[root].successor = newExpression(newContents.headID, newContents.argumentID);
  }

  bool isDownstreamEvolutionComplete(const ExpressionID root) {
    return (head(root) < 0 || expressions_[head(root)].isFullyEvolved) &&
           (argument(root) < 0 || expressions_[argument(root)].isFullyEvolved);
  }

  uint64_t checkForPossibleOverflow(uint64_t count) {
    if (count > std::numeric_limits<uint64_t>::max() / 4) throw Error::LeafCountOverflow;
    return count;
  }

  mpz_class checkForPossibleOverflow(mpz_class count) { return count; }

  template <typename Number>
  Number leafCount(const ExpressionID root, std::vector<Number>* subexpressionLeafCounts) {
    if (root < 0) return 1;
    if (root >= expressions_.size()) throw Error::InvalidReference;
    if (subexpressionLeafCounts->at(root) == -1) {
      (*subexpressionLeafCounts)[root] =
          checkForPossibleOverflow(leafCount<Number>(head(root), subexpressionLeafCounts)) +
          checkForPossibleOverflow(leafCount<Number>(argument(root), subexpressionLeafCounts));
    }
    return subexpressionLeafCounts->at(root);
  }

  void collectExpressions(const ExpressionID root,
                          std::vector<CombinatorExpression>* result,
                          std::unordered_map<ExpressionID, int>* expressionIDToIndex) const {
    if (root < 0) return;
    if (expressionIDToIndex->count(root)) return;
    expressionIDToIndex->insert({root, result->size()});
    result->push_back(expressions_[root].expression);
    collectExpressions(head(root), result, expressionIDToIndex);
    collectExpressions(argument(root), result, expressionIDToIndex);
  }

  // assumes positive expression IDs exist in expressionIDToIndex
  ExpressionID remapExpressionIDToIndex(const ExpressionID expressionID,
                                        const std::unordered_map<ExpressionID, int>& expressionIDToIndex) const {
    if (expressionID < 0) {
      return expressionID;
    } else {
      return expressionIDToIndex.at(expressionID);
    }
  }
};

CombinatorSystem::CombinatorSystem(const std::vector<CombinatorExpression>& initialExpressions,
                                   const ExpressionID initialRoot,
                                   const EvaluationOrder evaluationOrder)
    : implementation_(std::make_shared<Implementation>(initialExpressions, initialRoot, evaluationOrder)) {}

int64_t CombinatorSystem::evolve(const CombinatorRules& rules,
                                 const int64_t eventsCount,
                                 const std::function<bool()>& shouldAbort,
                                 const int64_t maxLeafCount) {
  return implementation_->evolve(rules, eventsCount, shouldAbort, maxLeafCount);
}

std::pair<std::vector<CombinatorExpression>, ExpressionID> CombinatorSystem::finalExpressionsAndRoot() const {
  return implementation_->finalExpressionsAndRoot();
}

std::vector<uint64_t> CombinatorSystem::leafCounts() const { return implementation_->leafCounts<uint64_t>(); }

std::vector<mpz_class> CombinatorSystem::leafCountsMPZ() const { return implementation_->leafCounts<mpz_class>(); }
}  // namespace CombinatorEvolve
