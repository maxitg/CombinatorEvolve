#!/usr/bin/env wolframscript

Needs["CCompilerDriver`"];
Needs["GeneralUtilities`"];

$success = True;

Check[
  $repoRoot = DirectoryName[$InputFileName];

  With[{
      cppSource = FileNameJoin[{$repoRoot, "libCombinatorEvolve"}]},
    CreateLibrary[
      FileNames["*.cpp", AbsoluteFileName[cppSource]],
      "libCombinatorEvolve",
      "CleanIntermediate" -> True,
      "CompileOptions" -> Switch[$OperatingSystem,
        "Windows",
          {"/std:c++17", "/EHsc"},
        _,
          {"-std=c++17 -lgmp"}],
      "Language" -> "C++",
      "TargetDirectory" -> FileNameJoin[{$repoRoot, "LibraryResources", $SystemID}],
      "TargetSystemID" -> $SystemID];
  ];,
  $success = False;
];

If[$success,
  Print["Build done."];,
  Print["Build failed."];
];
