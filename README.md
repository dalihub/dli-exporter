# dli-exporter

Converts scenes from popular 3D formats to, .dli, a JSON based format, which can
be loaded and displayed using libdli (part of [dali-toolkit](https://github.com/dalihub/dali-toolkit )).

The repository provides two artifacts: dli-exporter-core, a static library that
performs the processing, and dli-exporter, a simple CLI implementation.

## Prequisites

- *either* CMake 2.6.9+ & GCC v6.3.0+
- *or* Visual Studio 2017
- [assimp](https://github.com/assimp/assimp) v5.0.0+

## Building

Visual Studio 2017 and CMake projects are provided in the build/ directory. The
latter is for Ubuntu; refer to build/cmake/build.sh for details.

## Usage

$ dli-exporter path/to/input.dae [other/path/to/output[.dli]]

## Known issues

   * Material entries need to be created (and image files used for textures moved), manually at the moment.
