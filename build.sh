#/bin/bash -x

# SPDX-FileCopyrightText: 2023 Ujwal Vujjini
# SPDX-License-Identifier: MIT

clear && clear

unameOut="$(uname -s)"
if [[ "$*" == *"clean"* ]]; then
  rm -rf build/
fi

build_folder="."
project_name="NumberGame"
top_level_dir=$PWD
asset_command="ln -sf"
asset_directory="Engine"
asset_folders=("Fonts" "Shaders" "Images" "Levels" "Audio")
use_clang_tidy="OFF"
asset_folder="."
build_path="$build_folder/$project_name/Assets"
is_copy_asset=true
use_sanitize_thread="OFF"
use_sanitize_address="OFF"
run_args=""
dll="OFF"

if [[ "$*" == *"dll"* ]]; then
  dll="ON"
fi

if [[ "$*" == *"clang-tidy"* ]]; then
  use_clang_tidy="ON"
fi

if [[ "$*" == *"sanitize-thread"* || "$*" == *"tsan"* ]]; then
  use_sanitize_thread="ON"
  export TSAN_OPTIONS="suppressions=$PWD/sanitizer-thread-suppressions.supp second_deadlock_stack=1"
elif [[ "$*" == *"sanitize-address"* || "$*" == *"asan"* ]]; then
  use_sanitize_address="ON"
  #export TSAN_OPTIONS="suppressions=$PWD/sanitizer-thread-suppressions.supp second_deadlock_stack=1"
elif [[ "$*" == *"mangohud"* ]]; then
  run_args="mangohud --dlsym"
fi

mk_dir() {
  #rm -rf $build_path

  for folder in ${asset_folders[@]}; do
    mkdir -p $build_path/$folder
  done
}

windows() { [[ -n "$WINDIR" ]]; }

create_asset_directory() {
  for folder in ${asset_folders[@]}; do
    $asset_command $top_level_dir/src/$asset_directory/Assets/$folder/* $build_path/$folder 2>/dev/null || :
  done
}

create_asset() {
  mk_dir
  asset_directory="Engine"
  create_asset_directory

  asset_directory=$project_name
  create_asset_directory
}

create_asset_ln() {
  asset_command="ln -sf"
  create_asset
}

copy_asset() {
  asset_command="cp -rf"
  create_asset
}

build_project_linux() {
  if [ "$dll" == "OFF" ]; then
    mkdir $build_folder
    for dir in $top_level_dir/src/*/; do
      project_name=$(basename "$dir")
      build_path="$build_folder/$project_name/Assets"
      if [ "$project_name" != "Engine" ]; then
        if [ "$is_copy_asset" = true ]; then
          copy_asset
        else
          create_asset_ln
        fi
      fi
    done
  fi
  (
    cd $build_folder
    if [ "$dll" == "OFF" ]; then
      cmake -DCMAKE_BUILD_TYPE=$build_folder -DUSE_CLANG_TIDY=$use_clang_tidy -DSANITIZE_THREAD=$use_sanitize_thread -DSANITIZE_ADDRESS=$use_sanitize_address -DDLL=$dll -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -G Ninja ../..
      cp -rf compile_commands.json ../..
    fi
    ninja

    if [ "$dll" == "OFF" ]; then
      time ./UT_Engine/UT_Engine
      for dir in $top_level_dir/src/*/; do
        project_name=$(basename "$dir")
        if [ "$project_name" != "UT_Engine" ] && [ -e $project_name/"$project_name"UT ]; then
          ./$project_name/"$project_name"UT
        fi
      done
    fi
  )
}

build_project_mac() {
  for dir in $top_level_dir/src/*/; do
    project_name=$(basename "$dir")
    build_path="$project_name/$build_folder/Assets"
    if [ "$project_name" != "Engine" ]; then
      if [ "$is_copy_asset" = true ]; then
        copy_asset
      else
        create_asset_ln
      fi
    fi
  done
  cmake -DCMAKE_BUILD_TYPE=$build_folder -DUSE_CLANG_TIDY=$use_clang_tidy -DSANITIZE_THREAD=$use_sanitize_thread -DSANITIZE_ADDRESS=$use_sanitize_address -G Xcode ..
}

build_project_win() {
  for dir in $top_level_dir/src/*/; do
    project_name=$(basename "$dir")
    build_path="$project_name/$build_folder/Assets"
    if [ "$project_name" != "Engine" ]; then
      if [ "$is_copy_asset" = true ]; then
        copy_asset
      else
        create_asset_ln
      fi
    fi
  done
  cmake -DCMAKE_BUILD_TYPE=$build_folder -DCMAKE_GENERATOR_PLATFORM=x64 -DCMAKE_PREFIX_PATH=C:/libsndfile -B . -G "Visual Studio 17 2022" -A x64 ..
}

(
  export MSYS=winsymlinks:nativestrict
  if [ "$dll" == "OFF" ]; then
    if [[ "$*" == *"clean"* ]] || [[ "$*" == *"initialize"* ]]; then
      git submodule update --init --remote --merge
      (
        cd src/Engine/Dependencies/imgui
        git checkout master
      )
    fi
    mkdir build
  fi

  cd build
  unameOut="$(uname -s)"
  case "${unameOut}" in
  Linux*)
    if [[ "$*" == *"reldeb"* ]] || [[ "$*" == *"RelWithDebInfo"* ]]; then
      build_folder="RelWithDebInfo"
      is_copy_asset=false
      build_project_linux
    elif [[ "$*" == *"release"* ]] || [[ "$*" == *"Release"* ]]; then
      build_folder="Release"
      is_copy_asset=true
      build_project_linux
    else
      build_folder="Debug"
      is_copy_asset=false
      build_project_linux
    fi
    ;;
  Darwin*)
    if [[ "$*" == *"reldeb"* ]] || [[ "$*" == *"RelWithDebInfo"* ]]; then
      build_folder="RelWithDebInfo"
      is_copy_asset=false
      build_project_mac
    elif [[ "$*" == *"release"* ]] || [[ "$*" == *"Release"* ]]; then
      build_folder="Release"
      is_copy_asset=true
      build_project_mac
    else
      build_folder="Debug"
      is_copy_asset=false
      build_project_mac
    fi
    xcodebuild -scheme Nunu build -configuration $build_folder
    xcodebuild -scheme UT_Engine build -configuration $build_folder
    ;;
  CYGWIN*)
    if [[ "$*" == *"reldeb"* ]] || [[ "$*" == *"RelWithDebInfo"* ]]; then
      build_folder="RelWithDebInfo"
      is_copy_asset=false
      build_project_win
    elif [[ "$*" == *"release"* ]] || [[ "$*" == *"Release"* ]]; then
      build_folder="Release"
      is_copy_asset=true
      build_project_win
    else
      build_folder="Debug"
      is_copy_asset=false
      build_project_win
    fi
    ;;
  MINGW*)
    if [[ "$*" == *"reldeb"* ]] || [[ "$*" == *"RelWithDebInfo"* ]]; then
      build_folder="RelWithDebInfo"
      is_copy_asset=false
      build_project_win
    elif [[ "$*" == *"release"* ]] || [[ "$*" == *"Release"* ]]; then
      build_folder="Release"
      is_copy_asset=true
      build_project_win
    else
      build_folder="Debug"
      is_copy_asset=false
      build_project_win
    fi
    ;;
  *) cmake .. ;;
  esac

  if [[ "$*" == *"run"* ]]; then
    unameOut="$(uname -s)"
    if [[ "$*" == *"Editor"* ]] || [[ "$*" == *"-e"* ]]; then
      Editor="Editor"
    else
      Editor=""
    fi
    case "${unameOut}" in
    Linux*)
      $run_args ./$build_folder/${@: -1}/${@: -1}$Editor
      ;;
    Darwin*)
      $run_args ./$build_folder/${@: -1}/${@: -1}$Editor
      ;;
    CYGWIN*) ;;
    MINGW*) ;;
    *) ;;
    esac
  fi
)
