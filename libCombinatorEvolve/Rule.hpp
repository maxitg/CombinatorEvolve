#ifndef LIBCOMBINATOREVOLVE_RULE_HPP_
#define LIBCOMBINATOREVOLVE_RULE_HPP_

#include <utility>
#include <vector>

namespace CombinatorEvolve {
struct CombinatorRules {
  // the pairs correspond to heads and arguments
  // negative ExpressionIDs are constants
  std::vector<CombinatorExpression> expressions;
  std::vector<std::pair<ExpressionID, ExpressionID>> roots;
};
}  // namespace CombinatorEvolve

#endif  // LIBCOMBINATOREVOLVE_RULE_HPP_
