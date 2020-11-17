#ifndef LIBCOMBINATOREVOLVE_RULE_HPP_
#define LIBCOMBINATOREVOLVE_RULE_HPP_

#include <utility>
#include <vector>

#include "Expression.hpp"

namespace CombinatorEvolve {
// if the rule input is just a head nested to a fixed number of levels, and simple unique patterns at arguments,
// e.g., s[a_][b_][c_] or e[a_][b_], we can represent the input as a head depth (3 and 2 in the examples)
// and the ID of the nested head (IDs of s and e respectively).
struct InputShortcut {
  const int headDepth = -1;
  const ExpressionID nestedHeadID = nullExpression;

  bool isAvailable() const;
};

enum class ExpressionReferenceType {
  Constant,            // constant symbol like s or k
  ExistingExpression,  // reference to an expression matched by a pattern in the input
  CreatedExpression    // new composite expression referencing other expressions
};

struct ExpressionReference {
  const ExpressionReferenceType type;

  // Only one of the following three is used. Memory use is not a concern because rules are small and there is only
  // one copy of them.
  const ExpressionID constantID;
  const std::vector<int> pathToExisting;  // sequence of zeros and ones describing a path to follow from the input root
  const int createdExpressionIndex;       // the number of a newly created expression starting with zero

  static ExpressionReference fromConstant(const ExpressionID constantID);
  static ExpressionReference fromPath(const std::vector<int>& pathToExisting);
  static ExpressionReference fromCreatedExpression(const int createdExpressionIndex);
};

struct OutputBlueprint {
  const std::vector<std::pair<ExpressionReference, ExpressionReference>> newExpressionBlueprints;
  const ExpressionReference newRoot;
};

struct CombinatorRules {
  // The pairs correspond to heads and arguments
  // Negative ExpressionIDs are constants
  const std::vector<CombinatorExpression> expressions;
  const std::vector<std::pair<ExpressionID, ExpressionID>> roots;

  // A number one larger than the max pattern ID used in expressions
  const ExpressionID variablesCount;

  const std::vector<InputShortcut> shortcuts;
  const std::vector<OutputBlueprint> blueprints;

  CombinatorRules(const std::vector<CombinatorExpression>& expressions,
                  const std::vector<std::pair<ExpressionID, ExpressionID>>& roots);
};
}  // namespace CombinatorEvolve

#endif  // LIBCOMBINATOREVOLVE_RULE_HPP_
