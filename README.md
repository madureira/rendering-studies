# CPP Project Template

## Dependencies

- [Visual Studio Code - CPP Extesion](https://code.visualstudio.com/docs/languages/cpp)
- [CMake](https://cmake.org/download/)
- [Conan2](https://conan.io/)

## Configure

Generate the conan profile

```sh
conan profile detect
```

The path to the generated file is:

```sh
.conan2/profiles/default
```

Add these lines to the end of the generated file:

```sh
[platform_tool_requires]
cmake/0.00.0
```

Replace `0.00.0` by your CMake insalled version.

To know what's your CMake version, enter this command on your terminal:

```sh
cmake --version
```

## Build and Run

### MacOS / Linux

```sh
./bash.sh
```

### Windows

```sh
./bash.bat
```

## VSCODE

### Add dependencies to the include path

Edit the file `.vscode/c_cpp_properties.json` and add `conan2` to the `includePath` property:

```json
"includePath": [
    "${workspaceFolder}/**",
    "~/.conan2/**"
]
```
