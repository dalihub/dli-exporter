# libdli

Provides functionality for loading scenes created in the JSON-based DLI format. DLI is most similar to glTF, but less verbose / granular, and it has a few extra features:

- customizations: switch between alternative definitions of parts of the scene;
- environment maps;
- text, arc, and image elements;
- etc. (TODO)

There are two main modules in the package:

- dli-exporter, an assimp-based conversion tool which converts Collada and glTF files to DLI, comprised of dli-exporter/core static library and dli-exporter/cli command line tool;
- libdli: main loader functionality;

## Prequisites

- Windows:

  - Microsoft Visual Studio 2017 (or later, with 2017 build tools);
  - [windows-dependencies](https://github.com/dalihub/windows-dependencies ) - please follow the steps in its README.md *as well as* its vcpkg-script/ReadMe.md, to install VCPKG and all dependencies;
 
- Linux:

  - GCC v9.3+;
  - CMake v3.14+;
  - [dali-core](https://github.com/dalihub/dali-core );
  - [dali-adaptor](https://github.com/dalihub/dali-adaptor );
  - [dali-toolkit](https://github.com/dalihub/dali-toolkit );
  - configure DALi environment by following the instructions in README.md, in dali-core;

- All platforms:
 
  - [dali-demo](https://github.com/dalihub/dali-demo );
  - [assimp](https://github.com/assimp/assimp ) - for dli-exporter;
 
## Build instructions

1, build the DALi libraries and dali-demo (either using VCPKG or manually);
2,

  - Windows: refer to the VS2017 solution in build/windows;
  - Linux: run the build script relating to the project you want to build, in build/tizen/;
  - Tizen:

    $ gbs build -A ${target_arch}

    - for debug, add: --define "%enable_debug 1"
    - for SMACK-enabled targets, add: --define "%enable_dali_smack_rules 1"
