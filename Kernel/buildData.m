Package["CombinatorEvolve`"]

PackageImport["GeneralUtilities`"]

PackageExport["$CombinatorEvolveLibraryBuildTime"]
PackageExport["$CombinatorEvolveLibraryPath"]
PackageExport["$CombinatorEvolveBuildTime"]
PackageExport["$CombinatorEvolveGitSHA"]

PackageScope["$packageRoot"]

$packageRoot = FileNameDrop[$InputFileName, -2];

CombinatorEvolve::jitbuildfail =
  "Failed to (re)build libCombinatorEvolve. The existing library, if any, will be used instead.";

(* before loading build data, we check if we are running on a developer's machine, indicated by
the presence of the DevUtils sub-package, if so, we load it and do a rebuild, so that we can
get up-to-date versions of the various build properties *)
$devUtilsPath = FileNameJoin[{$packageRoot, "DevUtils", "init.m"}];
If[FileExistsQ[$devUtilsPath],
  Block[{$ContextPath = {"System`"}}, Quiet[Get[$devUtilsPath], General::shdw]];

  (* forwarders for the functions we want from DevUtils. This is done so
  we don't create the CombinatorEvolveDevUtils context for ordinary users (when DevUtils *isn't* available) *)
  buildLibCombinatorEvolve = Symbol["CombinatorEvolveDevUtils`BuildLibCombinatorEvolve"];
  gitSHAWithDirtyStar = Symbol["CombinatorEvolveDevUtils`GitSHAWithDirtyStar"];

  (* try build the C++ code immediately (which will most likely retrieve a cached library) *)
  (* if there is a frontend, then give a temporary progress panel, otherwise just Print *)
  If[TrueQ @ $Notebooks,
    (* WithLocalSettings will run the final 'cleanup' argument even if the evaluation of the second
    argument aborts (due to a Throw, user abort, etc.) *)
    Internal`WithLocalSettings[
      $progCell = None;
    ,
      $buildResult = buildLibCombinatorEvolve["PreBuildCallback" -> Function[
        $progCell = PrintTemporary @ Panel[
          "Building libCombinatorEvolve from sources in " <> #LibrarySourceDirectory,
          Background -> LightOrange]]];
    ,
      NotebookDelete[$progCell];
      $progCell = None;
    ];
  ,
    $buildResult = buildLibCombinatorEvolve["PreBuildCallback" -> "Print"];
  ];

  If[!AssociationQ[$buildResult],
    Message[CombinatorEvolve::jitbuildfail];
  ];
];

readJSONFile[file_] := Quiet @ Check[Developer`ReadRawJSONFile[file], $Failed];

SetUsage @ "
$CombinatorEvolveLibraryBuildTime gives the date object at which this C++ libCombinatorEvolve library was built.
";

SetUsage @ "
$CombinatorEvolveLibraryPath stores the path of the C++ libCombinatorEvolve library.
";

$libraryDirectory = FileNameJoin[{$packageRoot, "LibraryResources", $SystemID}];
$libraryBuildDataPath = FileNameJoin[{$libraryDirectory, "libCombinatorEvolveBuildInfo.json"}];

$buildData = readJSONFile[$libraryBuildDataPath];
If[$buildData === $Failed,
  $CombinatorEvolveLibraryBuildTime = $CombinatorEvolveLibraryPath = Missing["LibraryBuildDataNotFound"];
,
  $CombinatorEvolveLibraryBuildTime = DateObject[$buildData["LibraryBuildTime"], TimeZone -> "UTC"];
  $CombinatorEvolveLibraryPath = FileNameJoin[{$libraryDirectory, $buildData["LibraryFileName"]}];
];

SetUsage @ "
$CombinatorEvolveBuildTime gives the time at which this CombinatorEvolve paclet was built.
* When evaluated for an in-place build, this time is the time at which CombinatorEvolve was loaded.
";

SetUsage @ "
$CombinatorEvolveGitSHA gives the Git SHA of the repository from which this SetRepace paclet was built.
* When evaluated for an in-place build, this is simply the current HEAD of the git repository.
";

$pacletBuildInfoPath = FileNameJoin[{$packageRoot, "PacletBuildInfo.json"}];

If[FileExistsQ[$pacletBuildInfoPath] && AssociationQ[$pacletBuildInfo = readJSONFile[$pacletBuildInfoPath]],
  $CombinatorEvolveBuildTime = DateObject[$pacletBuildInfo["BuildTime"], TimeZone -> "UTC"];
  $CombinatorEvolveGitSHA = $pacletBuildInfo["GitSHA"];
,
  $CombinatorEvolveGitSHA = gitSHAWithDirtyStar[$packageRoot];
  If[!StringQ[$CombinatorEvolveGitSHA], Missing["GitLinkNotAvailable"]];
  $CombinatorEvolveBuildTime = DateObject[TimeZone -> "UTC"];
];
