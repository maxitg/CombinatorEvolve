#ifndef LIBCOMBINATOREVOLVE_EXPRESSION_HPP_
#define LIBCOMBINATOREVOLVE_EXPRESSION_HPP_

#include "IDTypes.hpp"

namespace CombinatorEvolve {
constexpr ExpressionID nullExpression = -1;

struct CombinatorExpression {
  ExpressionID headID;
  ExpressionID argumentID;
};
}  // namespace CombinatorEvolve

#endif  // LIBCOMBINATOREVOLVE_EXPRESSION_HPP_
