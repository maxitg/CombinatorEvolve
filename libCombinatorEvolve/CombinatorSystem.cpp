#include "CombinatorSystem.hpp"

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

class CombinatorSystem::Implementation {
 private:
  std::vector<CombinatorExpression> expressions_;
  std::unordered_map<std::pair<ExpressionID, ExpressionID>, ExpressionID, PairHash> contentsToExpressions_;
  std::vector<ExpressionID> evolutionRoots_;

  std::unordered_map<ExpressionID, ExpressionID> successors_;
  std::unordered_set<ExpressionID> fullyEvolvedExpressions_;

 public:
  Implementation(const std::vector<CombinatorExpression>& initialExpressions, const ExpressionID initialRoot)
      : expressions_(initialExpressions), evolutionRoots_({initialRoot}) {
    for (int i = 0; i < expressions_.size(); ++i) {
      contentsToExpressions_[{expressions_[i].headID, expressions_[i].argumentID}] = i;
    }
  }

  int64_t evolve(const CombinatorRules& rules, const int64_t eventsCount, const std::function<bool()>& shouldAbort) {
    int64_t eventsDone = 0;
    while (eventsDone < eventsCount) {
      if (shouldAbort()) return eventsDone;
      if (evolveOnce(rules, evolutionRoots_.back())) {
        evolutionRoots_.push_back(successor(evolutionRoots_.back()));
        successors_.erase(evolutionRoots_[evolutionRoots_.size() - 2]);
        ++eventsDone;
      } else {
        return eventsCount;
      }
    }
    return eventsDone;
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
  ExpressionID head(const ExpressionID root) {
    if (root < 0) {
      return nullExpression;
    } else {
      return expressions_[root].headID;
    }
  }

  ExpressionID argument(const ExpressionID root) {
    if (root < 0) {
      return nullExpression;
    } else {
      return expressions_[root].argumentID;
    }
  }

  ExpressionID successor(const ExpressionID root) {
    if (root < 0 || !successors_.count(root)) {
      return nullExpression;
    } else {
      return successors_[root];
    }
  }

  int evolveOnce(const CombinatorRules& rules, const ExpressionID root) {
    if (root < 0) return 0;
    if (fullyEvolvedExpressions_.count(root)) return 0;

    bool didEvolve = false;

    for (const auto& inputOutputRoots : rules.roots) {
      if (tryEvolvingUsingRule(rules.expressions, inputOutputRoots.first, inputOutputRoots.second, root)) {
        didEvolve = true;
        break;
      }
    }

    if (!didEvolve && (evolveOnce(rules, head(root)) || evolveOnce(rules, argument(root)))) {
      updateFromDownstream(root);
      didEvolve = true;
    }

    if (didEvolve) {
      return 1;
    } else {
      if (isDownstreamEvolutionComplete(root)) fullyEvolvedExpressions_.insert(root);
      return 0;
    }
  }

  using ExpressionsMap = std::unordered_map<ExpressionID, ExpressionID>;

  int tryEvolvingUsingRule(const std::vector<CombinatorExpression>& ruleExpressions,
                           const ExpressionID inputRoot,
                           const ExpressionID outputRoot,
                           const ExpressionID root) {
    ExpressionsMap inputPatternsToExpressions;
    if (!matchExpression(root, inputRoot, ruleExpressions, &inputPatternsToExpressions)) return 0;
    successors_[root] = createExpressions(outputRoot, ruleExpressions, inputPatternsToExpressions);
    return 1;
  }

  bool matchExpression(ExpressionID root,
                       ExpressionID patternRoot,
                       const std::vector<CombinatorExpression>& patternExpressions,
                       ExpressionsMap* inputPatternsToExpressions) {
    if (patternRoot < 0) {
      return root == patternRoot;
    } else if (patternRoot >= patternExpressions.size()) {  // pattern
      if (inputPatternsToExpressions->count(patternRoot)) {
        return inputPatternsToExpressions->at(patternRoot) == root;
      } else {
        (*inputPatternsToExpressions)[patternRoot] = root;
        return true;
      }
    } else {
      return matchExpression(
                 head(root), patternExpressions[patternRoot].headID, patternExpressions, inputPatternsToExpressions) &&
             matchExpression(argument(root),
                             patternExpressions[patternRoot].argumentID,
                             patternExpressions,
                             inputPatternsToExpressions);
    }
  }

  ExpressionID createExpressions(ExpressionID patternRoot,
                                 const std::vector<CombinatorExpression>& patternExpressions,
                                 const ExpressionsMap& inputPatternsToExpressions) {
    if (inputPatternsToExpressions.count(patternRoot)) {
      return inputPatternsToExpressions.at(patternRoot);
    } else {
      return newExpression(
          createExpressions(patternExpressions[patternRoot].headID, patternExpressions, inputPatternsToExpressions),
          createExpressions(
              patternExpressions[patternRoot].argumentID, patternExpressions, inputPatternsToExpressions));
    }
  }

  // Will use existing one if possible
  ExpressionID newExpression(const ExpressionID head, const ExpressionID argument) {
    if (contentsToExpressions_.count({head, argument})) {
      return contentsToExpressions_[{head, argument}];
    } else {
      expressions_.push_back({head, argument});
      return expressions_.size() - 1;
    }
  }

  void updateFromDownstream(const ExpressionID root) {
    ExpressionID headSuccessor = successor(head(root));
    ExpressionID argumentSuccessor = successor(argument(root));

    CombinatorExpression newContents;
    if (headSuccessor != nullExpression) {
      newContents = {headSuccessor, argument(root)};
      successors_.erase(head(root));
    } else if (argumentSuccessor != nullExpression) {
      newContents = {head(root), argumentSuccessor};
      successors_.erase(argument(root));
    } else {
      throw Error::InconsistentDownstreamUpdate;
    }

    successors_[root] = newExpression(newContents.headID, newContents.argumentID);
  }

  bool isDownstreamEvolutionComplete(const ExpressionID root) {
    return ((head(root) < 0 || fullyEvolvedExpressions_.count(head(root))) &&
            (argument(root) < 0 || fullyEvolvedExpressions_.count(argument(root))));
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
};

CombinatorSystem::CombinatorSystem(const std::vector<CombinatorExpression>& initialExpressions,
                                   const ExpressionID initialRoot)
    : implementation_(std::make_shared<Implementation>(initialExpressions, initialRoot)) {}

int64_t CombinatorSystem::evolve(const CombinatorRules& rules,
                                 const int64_t eventsCount,
                                 const std::function<bool()>& shouldAbort) {
  return implementation_->evolve(rules, eventsCount, shouldAbort);
}

std::vector<uint64_t> CombinatorSystem::leafCounts() { return implementation_->leafCounts<uint64_t>(); }

std::vector<mpz_class> CombinatorSystem::leafCountsMPZ() { return implementation_->leafCounts<mpz_class>(); }
}  // namespace CombinatorEvolve
