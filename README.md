# xengine
Small 3d engine written in C

The Visual Studio 2026 solution includes a GLFW static library project. It
builds `glfw3.lib` into the output directory configured by `xe_common.props`.

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
