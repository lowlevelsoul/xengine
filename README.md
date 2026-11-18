# xengine
Small 3d engine written in C

The Visual Studio 2026 solution includes a GLFW static library project. It
builds `glfw3.lib` into the output directory configured by `xe_common.props`.

To run the frame heap tests from a Visual Studio Developer Command Prompt:

```bat
cl /nologo /std:c11 /experimental:c11atomics /I source\engine\inc source\engine\src\FrameHeap.c tests\FrameHeap_test.c /Fe:frameheap_test.exe
frameheap_test.exe
```
