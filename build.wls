#!/usr/bin/env wolframscript

Check[
  Block[{$ContextPath}, Quiet[Get[FileNameJoin[{DirectoryName[$InputFileName], "DevUtils", "init.m"}]], General::shdw]];
  Off[General::stop];
,
  Print["Message occurred during loading of DevUtils. Build failed."];
  Exit[1];
];

Check[
  result = CombinatorEvolveDevUtils`BuildLibCombinatorEvolve["Verbose" -> True];
  If[!AssociationQ[result], Exit[1]];
,
  Print["Message occurred during building. Build failed."];
  Exit[1];
];

Print["Build succeeded."];
Exit[0];
