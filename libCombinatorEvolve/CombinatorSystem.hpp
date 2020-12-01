#ifndef LIBCOMBINATOREVOLVE_COMBINATORSYSTEM_HPP_
#define LIBCOMBINATOREVOLVE_COMBINATORSYSTEM_HPP_

#include <gmpxx.h>

#include <functional>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Expression.hpp"
#include "Rule.hpp"

namespace CombinatorEvolve {
enum class EvaluationOrder { LeftmostOutermost, LeftmostInnermost };

class CombinatorSystem {
 public:
  enum class Error { InvalidReference, InconsistentDownstreamUpdate, LeafCountOverflow, EventsCountExceeded };

  CombinatorSystem(const std::vector<CombinatorExpression>& initialExpressions,
                   ExpressionID initialRoot,
                   EvaluationOrder evaluationOrder);

  int64_t evolve(const CombinatorRules& rules,
                 int64_t eventsCount,
                 const std::function<bool()>& shouldAbort,
                 int64_t maxLeafCount = -1);

  std::pair<std::vector<CombinatorExpression>, ExpressionID> finalExpressionsAndRoot() const;

  std::vector<uint64_t> leafCounts() const;
  std::vector<mpz_class> leafCountsMPZ() const;

 private:
  class Implementation;
  std::shared_ptr<Implementation> implementation_;
};
}  // namespace CombinatorEvolve

#endif  // LIBCOMBINATOREVOLVE_COMBINATORSYSTEM_HPP_
