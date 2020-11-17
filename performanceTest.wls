#!/usr/bin/env wolframscript

Needs["GeneralUtilities`"];
Needs["GitLink`"];

<< CombinatorEvolve`;

$repoRoot = DirectoryName[$InputFileName];

tests = Hold @ <|
  "s[s][s][s[s]][s][s] (no overflow)" -> SKCombinatorLeftmostOutermostLeafCounts[s[s][s][s[s]][s][s], 3438],
  "s[s][s][s[s]][s][s] (overflow)" -> SKCombinatorLeftmostOutermostLeafCounts[s[s][s][s[s]][s][s], 3554],
  "s[s[s]][s][s[s]][s][k]" -> SKCombinatorLeftmostOutermostLeafCounts[s[s[s]][s][s[s]][s][k], 100000],
  "s[s][s][s[s]][s][k[s]]" -> SKCombinatorLeftmostOutermostLeafCounts[s[s][s][s[s]][s][k[s]], 10000]
|>;

$defaultMeasurementsCount = 5;

$measurementsCount = If[Length @ $ScriptCommandLine >= 4,
  ToExpression[$ScriptCommandLine[[4]]],
  $defaultMeasurementsCount];
If[!IntegerQ[$measurementsCount] || $measurementsCount < 2,
  Print["The third argument should be an integer measurements count of at least 2."];
  Exit[1];
];

Attributes[meanAroundTiming] = {HoldAll};
meanAroundTiming[expr_] := MeanAround @ Table[First @ AbsoluteTiming[expr], $measurementsCount]

runTests[repo_, sha_, tests_] := ModuleScope[
  Print["Testing ", sha];
  GitCheckoutReference[repo, sha];
  Run["./build.wls"];
  CloseKernels[];
  {kernel} = LaunchKernels[1];
  result = ParallelEvaluate[
    Get[FileNameJoin[{$repoRoot, "Kernel", "init.m"}]];
    meanAroundTiming @@@ KeyMap[ReleaseHold, ReleaseHold @ Map[Hold, tests, {3}]],
    kernel
  ];
  Print[""];
  result
]

speedupDelta[old_, new_] := (old - new) / old

$gitRepo = GitOpen[$repoRoot];
$currentSHA = GitSHA[$gitRepo, $gitRepo["HEAD"]];
$cleanQ = AllTrue[# === {} &] @ GitStatus[$gitRepo];

If[!$cleanQ,
  Print["Current git tree must be clean."];
  Exit[1];
];

$oldSHA = If[Length @ $ScriptCommandLine >= 2, $ScriptCommandLine[[2]], "master"];
$newSHA = If[Length @ $ScriptCommandLine >= 3, $ScriptCommandLine[[3]], $currentSHA];

$symbolicRef = RunProcess[{"git", "symbolic-ref", "--short", "HEAD"}];
$originalRef = If[$symbolicRef["ExitCode"] === 0, StringExtract[$symbolicRef["StandardOutput"], 1], $currentSHA];

{$oldResults, $newResults} = runTests[$gitRepo, #, tests] & /@ {$oldSHA, $newSHA};

RunProcess[{"git", "checkout", "-q", $originalRef}];

$speedup = speedupDelta[$oldResults, $newResults];

$redColor = "\033[0;31m";
$greenColor = "\033[0;32m";
$whiteColor = "\033[0;37m";
$endColor = "\033[0m";

differenceString[meanAround_] := With[{
    magnitude = QuantityMagnitude[meanAround, "Percent"]},
  If[5 * magnitude[[2]] < Abs[magnitude[[1]]], If[magnitude[[1]] > 0, $greenColor, $redColor], $whiteColor] <>
  FirstCase[
    MakeBoxes[magnitude, StandardForm],
    TemplateBox[{value_, error_}, "Around", ___] :> value <> " \[PlusMinus] " <> error <> " %"] <>
  $endColor
]

KeyValueMap[
  Print[
    #1,
    StringJoin[ConstantArray[" ", Max[40 - StringLength[#1], 1]]],
    differenceString[#2]] &,
  $speedup];
