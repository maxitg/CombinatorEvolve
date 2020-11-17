Unprotect["CombinatorEvolve`*"];

SetReplace`PackageScope`unloadLibrary[];

ClearAll @@ (# <> "*" & /@ Contexts["CombinatorEvolve`*"]);

Block[
  {GeneralUtilities`Control`PackagePrivate`$DesugaringRules = {}},
  Get[FileNameJoin[{FileNameDrop[$InputFileName], "CombinatorLeftmostOutermostLeafCounts.m"}]];
];

SetAttributes[#, {Protected, ReadProtected}] & /@ Evaluate @ Names @ "CombinatorEvolve`*";
