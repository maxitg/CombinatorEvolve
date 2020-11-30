Package["CombinatorEvolve`"]

PackageImport["GeneralUtilities`"]

PackageScope["unloadLibrary"]

PackageExport["CombinatorLeafCounts"]
PackageExport["CombinatorFinalExpression"]
PackageExport["CombinatorLeftmostOutermostLeafCounts"]
PackageExport["SKCombinatorLeftmostOutermostLeafCounts"]

$expectedLibName = "libCombinatorEvolve." <> System`Dump`LibraryExtension[];

findLibraryIn[basePath_] := Scope[
  libraryPath = FileNameJoin[{basePath, "LibraryResources", $SystemID, $expectedLibName}];
  If[FileExistsQ[libraryPath], libraryPath, $Failed]
];

$parentDirectory = FileNameDrop[$InputFileName, -2];

CombinatorLeftmostOutermostLeafCounts::nolibcombinatorEvolve = "Could not locate ``.";

$libraryFile = findLibraryIn[$parentDirectory];
If[FailureQ[$libraryFile],
  Message[CombinatorLeftmostOutermostLeafCounts::nolibcombinatorEvolve, $expectedLibName];
];

unloadLibrary[] := If[StringQ[$libraryFile],
  Scan[LibraryFunctionUnload, $libraryFunctions];
  $libraryFunctions = Null;
  Quiet @ LibraryUnload[$libraryFile];
];

evolutionFunctionLoad[name_] := If[$libraryFile =!= $Failed,
  LibraryFunctionLoad[
    $libraryFile,
    name,
    {{Integer, 2},  (* rule expressions *)
     {Integer, 2},  (* rule roots *)
     {Integer, 2},  (* init expressions *)
     Integer,       (* root *)
     Integer,       (* events count *)
     Integer},      (* evaluation order *)
    {Integer, 1}],  (* leaf counts *)
  $Failed]

$libraryFunctions = {
  cpp$combinatorLeafCounts = evolutionFunctionLoad["combinatorLeafCounts"],
  cpp$combinatorFinalExpression = evolutionFunctionLoad["combinatorFinalExpression"];
};

encodeExpressionsAndRoots[exprs_, specialSymbolIDs_, patternVariables_ : {}] := ModuleScope[
  expressions = DeleteCases[
    Union[Level[exprs, {1, Infinity}, Heads -> True]],
    Alternatives @@ Join[Keys[specialSymbolIDs], patternVariables, {List}]];
  expressionsToIDs = Join[
    Thread[expressions -> Range[Length[expressions]] - 1],
    Thread[patternVariables -> Range[Length[expressions], Length[expressions] + Length[patternVariables] - 1]],
    Normal @ specialSymbolIDs
  ];
  expressionInheritance = Association[# -> {#[[0]], #[[1]]} & /@ expressions];
  {
    Values[KeySort[Fold[#2[Replace[expressionsToIDs], #] &, expressionInheritance, {KeyMap, Map[##, {2}] &}]]],
    Replace[exprs, expressionsToIDs, {1}]
  }
]

encodeExpressionsAndRoot[expr_, specialSymbolIDs_, patternVariables_ : {}] :=
  {#, First[#2]} & @@ encodeExpressionsAndRoots[{expr}, specialSymbolIDs, patternVariables]

encodeRules[rules_, patternIDs_, constantIDs_] := ModuleScope[
  inputs = First /@ rules /. Pattern -> (#1 &);
  outputs = Last /@ rules;
  {expressions, roots} = encodeExpressionsAndRoots[Riffle[inputs, outputs], constantIDs, patternIDs];
  {expressions, Partition[roots, 2]}
]

$Int64VectorTypeID = -45735;
$MPZVectorTypeID = -43083;

decodeLeafCounts[{$Int64VectorTypeID, numbers___}] := {numbers}

decodeLeafCounts[{$MPZVectorTypeID, spec___}] := ModuleScope[
  decodedLength = {spec}[[1]];
  digitCounts = {spec}[[2 ;; decodedLength + 1]];
  digitLists = TakeList[{spec}[[decodedLength + 2 ;; ]], digitCounts];
  Fold[2^63 # + #2 &, 0, #] & /@ digitLists
]

SKCombinatorLeftmostOutermostLeafCounts[initExpr_, eventsCount_] /; $libraryFile =!= $Failed :=
  CombinatorLeftmostOutermostLeafCounts[
    {Global`s[a_][b_][c_] :> a[c][b[c]], Global`k[a_][b_] :> a}, initExpr, eventsCount]

CombinatorLeftmostOutermostLeafCounts[rules_, initExpr_, eventsCount_] :=
  CombinatorLeafCounts[rules, initExpr, eventsCount, "LeftmostOutermost"]

$evaluationOrderCodes = <|"LeftmostOutermost" -> 0, "LeftmostInnermost" -> 1|>;

combinatorEvolutionArguments[rules_, initExpr_, eventsCount_, evaluationOrder_] := ModuleScope[
  patternAtoms = First /@ Union[Cases[First /@ rules, _Pattern, All, Heads -> True]];
  ruleConstants = Complement[
    Union @ Cases[rules, _ ? AtomQ, All, Heads -> True], 
    Join[patternAtoms, {Blank, Pattern, List, RuleDelayed}]];
  ruleConstantIDs = Association[Thread[ruleConstants -> Range[-2, -Length[ruleConstants] - 1, -1]]];

  additionalInitConstants = Complement[Cases[initExpr, _ ? AtomQ, All, Heads -> True], ruleConstants];
  initConstantIDs = Join[
    Association[Thread[additionalInitConstants ->
      Range[Min[ruleConstantIDs] - 1, Min[ruleConstantIDs] - Length[additionalInitConstants], -1]]],
    ruleConstantIDs];


  {##, eventsCount, Replace[evaluationOrder, $evaluationOrderCodes], initConstantIDs} & @@
    Join[encodeRules[rules, patternAtoms, ruleConstantIDs], encodeExpressionsAndRoot[initExpr, initConstantIDs]]
]

CombinatorLeafCounts[rules_, initExpr_, eventsCount_, evaluationOrder_] /; $libraryFile =!= $Failed :=
  decodeLeafCounts[
    cpp$combinatorLeafCounts @@ combinatorEvolutionArguments[rules, initExpr, eventsCount, evaluationOrder][[ ;; 6]]]

CombinatorFinalExpression[rules_, initExpr_, eventsCount_, evaluationOrder_] /; $libraryFile =!= $Failed := ModuleScope[
  argsAndConstantIDs = combinatorEvolutionArguments[rules, initExpr, eventsCount, evaluationOrder];
  encodedResult = cpp$combinatorFinalExpression @@ argsAndConstantIDs[[ ;; 6]];
  idsToSymbols = Association[Reverse /@ Normal @ Last @ argsAndConstantIDs];
  root = idx @ First[encodedResult];
  expressions = Partition[idx /@ Rest[encodedResult], 2];
  decodingRules = Join[
    Association[Thread[idx /@ Range[0, Length[expressions] - 1] -> (#[#2] & @@@ expressions)]],
    KeyMap[idx, idsToSymbols]];
  root //. decodingRules
]
