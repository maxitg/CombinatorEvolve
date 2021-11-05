# Combinator Systems

Combinator System is a rewriting system based on evaluating functions. A state of such a system is a nested expression,
for example,

```
s[s][s][s[s]][s][k[s]]
```

Note that there are two types of nesting here. That is, in `a[b]` both `a` and `b` can be nested expressions, so nesting
can happen as `a[b][c][d][e]` and also as `a[b[c[d[e]]]]`.

Rewriting rules then define transformations on expressions, for example, s[x_][y_][z_] :> x[z][y[z]].

Using this rule, a part of the expression above can be evaluated as `s[s][s][s[s]] -> s[s[s]][s[s[s]]]`, or, using the
package:

```wl
In[] := CombinatorFinalExpression[{s[x_][y_][z_] :> x[z][y[z]]}, s[s][s][s[s]], 1, "LeftmostOutermost"]
Out[] = s[s[s]][s[s[s]]]
```

# Combinator Evolve

*CombinatorEvolve* is a simple Wolfram Language paclet and C++ library for evaluating combinators.

It defines two functions, `CombinatorFinalExpression` and `CombinatorLeafCounts`.

`CombinatorFinalExpression` returns the final expression after a specified number of rewrites:

```wl
CombinatorFinalExpression[rules, init, eventCount, eventOrder, maxLeafCount]
```

where `rules` is a list of rules, e.g., `{s[x_][y_][z_] :> x[z][y[z]], k[x_][y_] :> x}`, `init` is the initial
expression, `eventCount` is the number of rewriting events, `eventOrder` can be `"LeftmostOutermost"` or
`"LeftmostInnermost"` and defines the order in which to evaluate events. `"LeftmostOutermost"` prefers to rewrite parts
of the expression closer to the root, whereas `"LeftmostInnermost"` starts from leafs. `maxLeafCount` is `Infinity` by
default, but it can be set to a final value, in which case the function will return `$Failed` if the leaf count in the
state is exceeded.

For example,

```wl
In[] := CombinatorFinalExpression[
  {s[x_][y_][z_] :> x[z][y[z]], k[x_][y_] :> x}, s[s][s][s[s]][s][k[s]], 10, "LeftmostOutermost"]
Out[] = s[s[s[s]][s][k[s][s[k[s]]][s[s[s[s]][s]][k[s]]]][k[s][k[s][s[k[s]]][s[s[s[s]][s]][k[s]]]]]]
```

`CombinatorLeafCounts` has the same syntax, except it returns the leaf counts in the state after each event:

```wl
In[] := CombinatorLeafCounts[
  {s[x_][y_][z_] :> x[z][y[z]], k[x_][y_] :> x}, s[s][s][s[s]][s][k[s]], 10, "LeftmostOutermost"]
Out[] = {8, 9, 9, 12, 13, 14, 16, 22, 33, 20, 31}
```

The special feature of this package is that it stores the expression as a DAG and avoids duplicating subexpressions for
rules such as `s[x_][y_][z_] :> x[z][y[z]]`. This allows it to run in O(step count * depth). Care should be taken with
`CombinatorFinalExpression` as it unpacks the (potentially large) expression before returning it.

# Installation Instructions

This package requires [GMP](https://gmplib.org). Once GMP is installed, `./build.wls && ./pack.wls` will create a paclet file. Finally,
evaluate `PacletInstall["CombinatorEvolve-*.*.*.paclet"]` to install the paclet, and ``Get["CombinatorEvolve`"]`` to
import it.
