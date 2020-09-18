# libdli

Provides functionality for loading scenes created in the JSON-based DLI format. DLI is most similar to glTF, but less verbose / granular, and it has a few extra features:

- customizations: switch between alternative definitions of parts of the scene;
- environment maps;
- text, arc, and image elements;
- etc. (TODO)

There are three main modules in the package:

- model-converter, an assimp-based conversion tool that exports DLI; (TODO)
- libdli: main loader functionality;
- scene-launcher: demonstrates scene loading using libdli;

Building:

- Windows: use the VS2017 solution in build/windows;
- Linux / Tizen: use the CMake script in build/tizen; (TODO)
