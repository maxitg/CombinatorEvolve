#!/usr/bin/env wolframscript

Check[
  Block[{$ContextPath}, Quiet[Get[FileNameJoin[{DirectoryName[$InputFileName], "DevUtils", "init.m"}]], General::shdw]];
  Off[General::stop];
  If[$VersionNumber < 12.1, Needs["PacletManager`"]];
,
  Print["Message occurred during loading of DevUtils. Build failed."];
  Exit[1];
];

Check[
  InstallGitLink[];

  buildResult = CombinatorEvolveDevUtils`BuildLibCombinatorEvolve["Verbose" -> True];
  If[!AssociationQ[buildResult], Exit[1]];

  packResult = CombinatorEvolveDevUtils`PackLibCombinatorEvolve["Verbose" -> True];
  pacletLocation = packResult["Location"];
  If[!StringQ[pacletLocation], Exit[1]];

  previousPaclets = PacletFind["CombinatorEvolve"];
  If[previousPaclets =!= {},
    Print["Uninstalling previous CombinatorEvolve at ", First[previousPaclets]["Location"]];
    If[FailureQ[PacletUninstall["CombinatorEvolve"]],
      Print["Could not uninstall old paclet."];
      Exit[1];
    ];
  ];

  Print["Installing paclet at ", pacletLocation];
  installedPaclet = PacletInstall[pacletLocation];
  If[FailureQ[installedPaclet],
    Print["Could not install new paclet."];
    Exit[1];
  ];

  Print["Paclet installed to ", installedPaclet["Location"]];
,
  Print["Message occurred during installing. Install failed."];
  Exit[1];
];

Print["Install succeeded."];
Exit[0];
