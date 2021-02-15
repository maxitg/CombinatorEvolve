Unprotect["CombinatorEvolve`*"];

SetReplace`PackageScope`unloadLibrary[];

ClearAll @@ (# <> "*" & /@ Contexts["CombinatorEvolve`*"]);

Block[
  {GeneralUtilities`Control`PackagePrivate`$DesugaringRules = {}},
  Get[FileNameJoin[{FileNameDrop[$InputFileName], "buildData.m"}]];
];

SetAttributes[#, {Protected, ReadProtected}] & /@ Evaluate @ Names @ "CombinatorEvolve`*";
