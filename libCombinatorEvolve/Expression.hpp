#ifndef LIBCOMBINATOREVOLVE_EXPRESSION_HPP_
#define LIBCOMBINATOREVOLVE_EXPRESSION_HPP_

#include "IDTypes.hpp"

namespace CombinatorEvolve {
constexpr ExpressionID nullExpression = -1;
constexpr ExpressionID combinatorS = -2;
constexpr ExpressionID combinatorK = -3;

struct CombinatorExpression {
  ExpressionID headID;
  ExpressionID argumentID;

  ExpressionID successor = nullExpression;     // result of the evolution event
  int64_t leafCount = -1;                      // downstream leaf count
  bool isDownstreamEvolutionComplete = false;  // nothing left to evolve downstream
};
}  // namespace CombinatorEvolve

#endif  // LIBCOMBINATOREVOLVE_EXPRESSION_HPP_
