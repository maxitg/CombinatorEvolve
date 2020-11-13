# Combinator Evolve

This is a simpler paclet for running SK-combinators.
I created it per a user's request, and it currently only has a single function,
`SKCombinatorLeftmostOutermostLeafCounts`, which allows one to compute leaf counts as a function of the events count
in leftmost-outermost evaluation order.

For example,

```wl
In[] := SKCombinatorLeftmostOutermostLeafCounts[s[s][s][s[s]][s][k[s]], 10]

Out[] = {8, 9, 9, 12, 13, 14, 16, 22, 33, 20, 31}
```

This code is quite fast and runs in O(step count * depth), which allows computing millions of steps in some cases.

# Contributing

This code could do a lot more than it currently does.
However, it really should not be a separate paclet, and should be a part of [*SetReplace*](https://setreplace.org).
If you are interested in this functionality, and would like to contribute, please get in contact with
[@maxitg](https://github.com/maxitg) on [Discord](https://discord.setreplace.org) to discuss integration into
*SetReplace*.
