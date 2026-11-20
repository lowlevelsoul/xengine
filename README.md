# xengine
Small 3d engine written in C

The Visual Studio 2026 solution includes a GLFW static library project. It
builds `glfw3.lib` into the output directory configured by `xe_common.props`.

The D3D12 swapchain accepts 1-16 frames in flight. Its flip-model swapchain
uses at least two buffers, and its DXGI maximum frame latency is set to the
requested value. The renderer waits for frame latency and GPU completion before
reusing frame resources, clears each submitted scene to red, and presents it.
Queued buffer writes are copied before the clear. Other scene contents and
render passes are not yet processed.

Buffer usage `0` creates a CPU-mappable upload buffer; `GPU_ONLY` creates a
non-mappable destination buffer. A queued write copies the entire source to
the specified destination offset. Pair every successful `Render_GpuBuffer_Map`
with `Render_GpuBuffer_Unmap`; nested maps remain valid until the last unmap.
Mapping and unmapping the same buffer require external synchronization.
Destroying a mapped buffer invalidates all its mappings. Keep buffers and
upload data unchanged until the copy completes. With `canDiscardSrc`, ownership
transfers to the renderer after a successful submission and the source is
destroyed after the GPU finishes; the same source can be used by multiple writes
in that submission.
At most 1024 distinct disposable sources may be pending. Destroy all other
buffers before finalising the renderer; destruction waits for submitted GPU
work.

To run the D3D12 swapchain tests from a Visual Studio Developer Command Prompt:

```bat
cl /nologo /std:c++20 /EHsc /I source\engine\inc /I source\3rdparty\D3D12MemoryAllocator-master\include /I source\3rdparty\DirectX-Headers\include\directx tests\RenderSwapchain_d3d12_test.cpp source\engine\src\d3d12\Render_buffer_3d312.cpp source\engine\src\Mem.c source\3rdparty\D3D12MemoryAllocator-master\src\D3D12MemAlloc.cpp /Fe:render_swapchain_test.exe /link d3d12.lib dxgi.lib dxguid.lib user32.lib
render_swapchain_test.exe
```

To run the frame heap tests from a Visual Studio Developer Command Prompt:

```bat
cl /nologo /std:c11 /experimental:c11atomics /I source\engine\inc source\engine\src\FrameHeap.c tests\FrameHeap_test.c /Fe:frameheap_test.exe
frameheap_test.exe
```

To run the bounded MPMC queue tests from the same prompt:

```bat
cl /nologo /std:c11 /experimental:c11atomics /I source\engine\inc tests\MPMCQueue_test.c /Fe:mpmcqueue_test.exe
mpmcqueue_test.exe
```

To run the Metal heap pool test on macOS with a Metal-capable device:

```sh
xcrun clang -fobjc-arc -I source/engine/inc tests/RenderHeapPool_metal_test.m source/engine/src/metal/RenderHeapPool_metal.m -framework Metal -framework Foundation -o /tmp/render_heap_pool_test
/tmp/render_heap_pool_test
```

Define a queue type with inline storage and use its generated functions:

```c
#include "xe/MPMCQueue.h"

MPMC_QUEUE_DEFINE( job_queue, uint32_t, 64 )

void example( void ) {
    job_queue queue;
    job_queue_Init( &queue );
    uint32_t job = 1;

    if ( job_queue_Write( &queue, &job ) ) {
        ( void )job_queue_Read( &queue, &job );
    }
}
```

The element type can also be a struct, pointer, or array typedef. Values are
copied by value; the caller manages the lifetime of anything a pointer refers to.
Initialization and reset require exclusive access. Operations can wait for an
earlier reader or writer to finish; they are not lock-free. See the header for
snapshot semantics and the version counter's ABA limitation.
