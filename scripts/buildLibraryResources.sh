#!/usr/bin/env bash
set -eo pipefail

combinatorEvolveRoot=$(dirname "$(cd "$(dirname "$0")" && pwd)")
cd "$combinatorEvolveRoot"

# Compute the source hash

sourceFiles="$(
  ls libCombinatorEvolve/*pp CMakeLists.txt cmake/*.in cmake/Dependencies/* scripts/buildLibraryResources.sh
)"

if command -v shasum &>/dev/null; then
  echo "Using SHA tool: $(which shasum)"
  sha="$(
    echo "$(
      for fileToHash in $sourceFiles; do
        shasum -a 256 "$fileToHash"
      done
      uname -sm
    )" | shasum -a 256 | cut -d\  -f1
  )"
elif [[ "$OSTYPE" == "msys" && $(command -v certutil) ]]; then # there is another certutil in macOS
  echo "Using SHA tool: $(which certutil)"
  echo "$(
    for fileToHash in $sourceFiles; do
      echo "$(certutil -hashfile "$fileToHash" SHA256 | findstr -v "hash")" "$fileToHash"
    done
    uname -sm
  )" >"$TEMP"/libCombinatorEvolveFilesToHash
  sha=$(certutil -hashfile "$TEMP"/libCombinatorEvolveFilesToHash SHA256 | findstr -v "hash")
else
  echo "Could not find SHA utility"
  exit 1
fi

shortSHA=$(echo "$sha" | cut -c 1-13)
echo "libCombinatorEvolve sources hash: $shortSHA"

# Build the library

mkdir -p build
cd build
cmake .. -DCOMBINATOR_EVOLVE_ENABLE_ALLWARNINGS=OFF -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release # Needed for multi-config generators
cd ..

# Set the platform-specific names

if [ "$(uname -sm)" = "Darwin x86_64" ]; then
  libraryResourcesDirName=MacOSX-x86-64
  libraryExtension=dylib
elif [ "$(uname -sm)" = "Linux x86_64" ]; then
  libraryResourcesDirName=Linux-x86-64
  libraryExtension=so
elif [[ "$OSTYPE" == "msys" && "$(uname -m)" == "x86_64" ]]; then # Windows
  libraryResourcesDirName=Windows-x86-64
  libraryExtension=dll
else
  echo "Operating system unsupported"
  exit 1
fi

libraryDir=LibraryResources/$libraryResourcesDirName
echo "LibraryResources directory: $combinatorEvolveRoot/$libraryDir"
echo "Library extension: $libraryExtension"

# Find the compiled library

compiledLibrary=build/libCombinatorEvolve.$libraryExtension
if [ ! -f $compiledLibrary ]; then
  compiledLibrary=build/Release/CombinatorEvolve.$libraryExtension
fi

if [ ! -f $compiledLibrary ]; then
  echo "Could not find compiled library"
  exit 1
fi

echo "Found compiled library at $combinatorEvolveRoot/$compiledLibrary"

# Add rpath for libgmp

if [ "$(uname -sm)" = "Darwin x86_64" ]; then
  install_name_tool -change "/usr/local/opt/gmp/lib/libgmp.10.dylib" \
                             "@executable_path/../SystemFiles/Libraries/MacOSX-x86-64/libgmp.dylib" \
                             "$compiledLibrary"
elif [ "$(uname -sm)" = "Linux x86_64" ]; then
  echo "Warning: ./buildLibraryResources.sh currently builds a library that will try to load an external libGMP, so it"
  echo "  might not work on other systems."
elif [[ "$OSTYPE" == "msys" && "$(uname -m)" == "x86_64" ]]; then # Windows
  echo "Operating system unsupported"
else
  echo "Operating system unsupported"
  exit 1
fi

# Copy the library to LibraryResources

mkdir -p $libraryDir
libraryDestination=$libraryDir/libCombinatorEvolve-$shortSHA.$libraryExtension
echo "Copying the library to $combinatorEvolveRoot/$libraryDestination"
cp $compiledLibrary "$libraryDestination"

metadataDestination=$libraryDir/libCombinatorEvolveBuildInfo.json
echo "Writing metadata to $combinatorEvolveRoot/$metadataDestination"
echo "\
{
  \"LibraryFileName\": \"libCombinatorEvolve-$shortSHA.$libraryExtension\",
  \"LibraryBuildTime\": $(date -u "+[%-Y, %-m, %-d, %-H, %-M, %-S]"),
  \"LibrarySourceHash\": \"$shortSHA\"
}" >$metadataDestination

cat $metadataDestination
echo "Build done"
