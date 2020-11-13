#!/usr/bin/env wolframscript

Needs["PacletManager`"];

$success = True;

Check[
  $repoRoot = DirectoryName[$InputFileName];
  Print[CreatePacletArchive[
    FileNameJoin[{$repoRoot, #}] & /@ {"Kernel", "LibraryResources", "PacletInfo.m"}, $repoRoot]];,
  $success = False;
];

If[$success,
  Print["Build done."];,
  Print["Build failed."];
];
