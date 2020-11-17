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
class CombinatorSystem {
 public:
  enum class Error { InvalidReference, InconsistentDownstreamUpdate, LeafCountOverflow };

  CombinatorSystem(const std::vector<CombinatorExpression>& initialExpressions, ExpressionID initialRoot);

  int64_t evolve(const CombinatorRules& rules, int64_t eventsCount, const std::function<bool()>& shouldAbort);

  std::vector<uint64_t> leafCounts();
  std::vector<mpz_class> leafCountsMPZ();

 private:
  class Implementation;
  std::shared_ptr<Implementation> implementation_;
};
}  // namespace CombinatorEvolve

#endif  // LIBCOMBINATOREVOLVE_COMBINATORSYSTEM_HPP_
