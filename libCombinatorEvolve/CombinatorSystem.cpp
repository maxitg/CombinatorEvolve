#include "CombinatorSystem.hpp"

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
  enum class Error { InvalidReference, InconsistentDownstreamUpdate };

 private:
  std::vector<CombinatorExpression> expressions_;
  std::unordered_map<std::pair<ExpressionID, ExpressionID>, ExpressionID, PairHash> contentsToExpressions_;
  std::vector<ExpressionID> evolutionRoots_;

 public:
  Implementation(const std::vector<CombinatorExpression>& initialExpressions, const ExpressionID initialRoot)
      : expressions_(initialExpressions), evolutionRoots_({initialRoot}) {
    for (int i = 0; i < expressions_.size(); ++i) {
      contentsToExpressions_[{expressions_[i].headID, expressions_[i].argumentID}] = i;
    }
  }

  int64_t evolve(const int64_t eventsCount, const std::function<bool()>& shouldAbort) {
    int64_t eventsDone = 0;
    while (eventsDone < eventsCount) {
      if (shouldAbort()) return eventsDone;
      if (evolveOnce(evolutionRoots_.back())) {
        evolutionRoots_.push_back(successor(evolutionRoots_.back()));
        expressions_[evolutionRoots_[evolutionRoots_.size() - 2]].successor = nullExpression;
        ++eventsDone;
      } else {
        return eventsCount;
      }
    }
    return eventsDone;
  }

  std::vector<mpz_class> leafCounts() {
    std::vector<mpz_class> result;
    for (auto root : evolutionRoots_) {
      result.push_back(leafCount(root));
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
    if (root < 0) {
      return nullExpression;
    } else {
      return expressions_[root].successor;
    }
  }

  int evolveOnce(const ExpressionID root) {
    if (root < 0) return 0;
    if (expressions_[root].isDownstreamEvolutionComplete) return 0;

    bool didEvolve = false;

    if (trySEvolve(root) || tryKEvolve(root)) {
      didEvolve = true;
    }

    if (!didEvolve && (evolveOnce(head(root)) || evolveOnce(argument(root)))) {
      updateFromDownstream(root);
      didEvolve = true;
    }

    if (didEvolve) {
      return 1;
    } else {
      if (isDownstreamEvolutionComplete(root)) expressions_[root].isDownstreamEvolutionComplete = true;
      return 0;
    }
  }

  int trySEvolve(const ExpressionID root) {
    if (head(head(head(root))) == combinatorS) {
      const ExpressionID newHead = newExpression(argument(head(head(root))), argument(root));
      const ExpressionID newArgument = newExpression(argument(head(root)), argument(root));
      const ExpressionID successorExpression = newExpression(newHead, newArgument);
      expressions_[root].successor = successorExpression;
      return 1;
    } else {
      return 0;
    }
  }

  int tryKEvolve(const ExpressionID root) {
    if (head(head(root)) == combinatorK) {
      expressions_[root].successor = argument(head(root));
      return 1;
    } else {
      return 0;
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

    std::pair<ExpressionID, ExpressionID> newContents;
    if (headSuccessor != nullExpression) {
      newContents = {headSuccessor, argument(root)};
      expressions_[head(root)].successor = nullExpression;
    } else if (argumentSuccessor != nullExpression) {
      newContents = {head(root), argumentSuccessor};
      expressions_[argument(root)].successor = nullExpression;
    } else {
      throw Error::InconsistentDownstreamUpdate;
    }

    expressions_[root].successor = newExpression(newContents.first, newContents.second);
  }

  bool isDownstreamEvolutionComplete(const ExpressionID root) {
    return ((head(root) < 0 || expressions_[head(root)].isDownstreamEvolutionComplete) &&
            (argument(root) < 0 || expressions_[argument(root)].isDownstreamEvolutionComplete));
  }

  mpz_class leafCount(const ExpressionID root) {
    if (root < 0) return 1;
    if (root >= expressions_.size()) throw Error::InvalidReference;
    if (expressions_[root].leafCount == -1) {
      expressions_[root].leafCount = leafCount(head(root)) + leafCount(argument(root));
    }
    return expressions_[root].leafCount;
  }
};

CombinatorSystem::CombinatorSystem(const std::vector<CombinatorExpression>& initialExpressions,
                                   const ExpressionID initialRoot)
    : implementation_(std::make_shared<Implementation>(initialExpressions, initialRoot)) {}

int64_t CombinatorSystem::evolve(const int64_t eventsCount, const std::function<bool()>& shouldAbort) {
  return implementation_->evolve(eventsCount, shouldAbort);
}

std::vector<mpz_class> CombinatorSystem::leafCounts() { return implementation_->leafCounts(); }
}  // namespace CombinatorEvolve
