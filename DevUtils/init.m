Package["CombinatorEvolveDevUtils`"]

(* CombinatorEvolveDevUtils is *not* included in paclet builds, so is not visible to users,
but is available for developer workflow purposes, and is used by the build scripts *)

PackageImport["GeneralUtilities`"]

PackageExport["$CombinatorEvolveRoot"]
PackageExport["$DevUtilsRoot"]
PackageExport["$DevUtilsTemporaryDirectory"]

SetUsage @ "
$CombinatorEvolveRoot is the directory from which CombinatorEvolve was loaded.
";

$CombinatorEvolveRoot = FileNameDrop[$InputFileName, -2];

SetUsage @ "
$DevUtilsRoot is the directory from which the DevUtils package was loaded.
";

$DevUtilsRoot = FileNameDrop[$InputFileName, -1];

SetUsage @ "
$DevUtilsTemporaryDirectory is the temprary directory that DevUtils uses.
";

$DevUtilsTemporaryDirectory := EnsureDirectory @ FileNameJoin[{$TemporaryDirectory, "CombinatorEvolve"}];
