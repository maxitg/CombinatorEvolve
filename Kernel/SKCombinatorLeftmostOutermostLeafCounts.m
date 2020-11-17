Package["CombinatorEvolve`"]

PackageImport["GeneralUtilities`"]

PackageScope["unloadLibrary"]

PackageExport["SKCombinatorLeftmostOutermostLeafCounts"]

$expectedLibName = "libCombinatorEvolve." <> System`Dump`LibraryExtension[];

findLibraryIn[basePath_] := Scope[
  libraryPath = FileNameJoin[{basePath, "LibraryResources", $SystemID, $expectedLibName}];
  If[FileExistsQ[libraryPath], libraryPath, $Failed]
];

$parentDirectory = FileNameDrop[$InputFileName, -2];

SKCombinatorLeftmostOutermostLeafCounts::nolibcombinatorEvolve = "Could not locate ``.";

$libraryFile = findLibraryIn[$parentDirectory];
If[FailureQ[$libraryFile],
  Message[SetReplace::nolibcombinatorEvolve, $expectedLibName];
];

unloadLibrary[] := If[StringQ[$libraryFile],
  Scan[LibraryFunctionUnload, $libraryFunctions];
  $libraryFunctions = Null;
  Quiet @ LibraryUnload[$libraryFile];
];

cpp$skCombinatorLeftmostOutermostLeafCounts = If[$libraryFile =!= $Failed,
  LibraryFunctionLoad[
    $libraryFile,
    "skCombinatorLeftmostOutermostLeafCounts",
    {{Integer, 2},  (* expressions *)
     Integer,  (* root *)
     Integer},  (* events count *)
    {Integer, 1}],  (* leaf counts *)
  $Failed];

$libraryFunctions = {cpp$skCombinatorLeftmostOutermostLeafCounts};

$combinatorIDs = <|Global`s -> -2, Global`k -> -3|>;

encodeExpressionsAndRoot[expr_] := ModuleScope[
  expressions = DeleteCases[Union[Level[expr, All, Heads -> True]], Alternatives @@ Keys[$combinatorIDs]];
  expressionsToIDs = Join[Thread[expressions -> Range[Length[expressions]] - 1], Normal @ $combinatorIDs];
  expressionInheritance = Association[# -> {#[[0]], #[[1]]} & /@ expressions];
  {
    Values[KeySort[Fold[#2[Replace[expressionsToIDs], #] &, expressionInheritance, {KeyMap, Map[##, {2}] &}]]],
    Replace[expressionsToIDs][expr]
  }
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

SKCombinatorLeftmostOutermostLeafCounts[initExpr_, eventsCount_] /; $cpp$setReplace =!= $Failed := ModuleScope[
  decodeLeafCounts[cpp$skCombinatorLeftmostOutermostLeafCounts[##, eventsCount] & @@ encodeExpressionsAndRoot[initExpr]]
]
