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
  enum class Error { InvalidReference };

 private:
  std::vector<CombinatorExpression> expressions_;
  std::unordered_map<std::pair<ExpressionID, ExpressionID>, ExpressionID, PairHash> contentsToExpressions_;
  ExpressionID initialRoot_;
  ExpressionID currentRoot_;

 public:
  Implementation(const std::vector<CombinatorExpression>& initialExpressions, const ExpressionID initialRoot)
      : expressions_(initialExpressions), initialRoot_(initialRoot), currentRoot_(initialRoot) {
    for (int i = 0; i < expressions_.size(); ++i) {
      contentsToExpressions_[{expressions_[i].headID, expressions_[i].argumentID}] = i;
    }
  }

  int64_t evolve(const int64_t eventsCount, const std::function<bool()>& shouldAbort) {
    int64_t eventsDone = 0;
    while (eventsDone < eventsCount) {
      if (shouldAbort()) return eventsDone;
      eventsDone += evolveOnce();
    }
    return eventsDone;
  }

  std::vector<int64_t> leafCounts() {
    // TODO: implement
    return {leafCount(initialRoot_), 0};
  }

 private:
  int evolveOnce() {
    // TODO: implement
    return 0;
  }

  int64_t leafCount(ExpressionID root) {
    if (root < 0) return 1;
    if (root >= expressions_.size()) throw Error::InvalidReference;
    if (expressions_[root].leafCount >= 0) return expressions_[root].leafCount;
    return leafCount(expressions_[root].headID) + leafCount(expressions_[root].argumentID);
  }
};

CombinatorSystem::CombinatorSystem(const std::vector<CombinatorExpression>& initialExpressions,
                                   const ExpressionID initialRoot)
    : implementation_(std::make_shared<Implementation>(initialExpressions, initialRoot)) {}

int64_t CombinatorSystem::evolve(const int64_t eventsCount, const std::function<bool()>& shouldAbort) {
  return implementation_->evolve(eventsCount, shouldAbort);
}

std::vector<int64_t> CombinatorSystem::leafCounts() { return implementation_->leafCounts(); }
}  // namespace CombinatorEvolve
