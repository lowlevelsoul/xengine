# Metal Heap Support Specification

Status: Draft. Edit the choices below before implementation.

## Misc
- Fatal errors will be flagged with the Sys_FatalError macro
- Thread synchromisation will be done using sys mutex;s ( sys_mux_t )

## Scope and ownership

- Resources: [x] buffers only (recommended initially) [ ] buffers and textures
- Device ownership: [x] keep the existing global Metal device [ ] introduce explicit `gpu_device_t` ownership
- Staging buffers: [ ] direct allocations [x] heap allocations

The current Metal backend stores one global device. `gpu_device_t` is forward-declared, and buffer operations currently
ignore their device argument. Texture operations are not yet implemented.

## Heap policy

- Use separate heaps for incompatible storage modes, including shared and private storage.
- Heap allocation type: [x] automatic (recommended initially) [ ] placement
- Heap capacity: [ ] fixed [x] growable
- Configuration: [x] API parameters [ ] engine constants
- Capacity and growth sizes: 
    - We'll support a maximum global number of active heaps of 256, This will be specified in a define
        - If we run out of heaps, we'll fatal error
    - Heaps are allocated in chunks of size that are specified in by the value given when ceating a gpu_render_context
- On exhaustion or allocation failure: [x] create another heap [ ] fall back to direct allocation [ ] report failure
- Handle Metal's required resource sizes and alignments when sizing heaps and allocating resources.
- Unsupported heap configurations or resource options: 
    - We'll simply just assert when this happens. 

## Buffer behavior

- We can assume that were will only ever be one render context
- Shared buffers remain CPU-visible and mappable.
- GPU-only buffers remain private and cannot be mapped.
- GPU-only writes are staged to happen at the start of the next frame
- Shared buffer writes can occur imemdiately without queuing 
- Preserve offset writes, bounds checking, and existing error reporting unless explicitly changed below.
- Changes to the public buffer or device API: 
    - We don't change the public AP for buffers.
    - We will maintain a thread safe list of buffers and textures that are to be written to
    - The list will just be an array describing the various buffer/texture writes of 4096 entries ( use a defined for the size )
    - The engine should just fatal error when there are too many entries
    - At the start of each frame, we'll create the appropriate blit/copy command encoders and and the commands
        - This happens in Gpu_RenderContext_BeginFrame
    - The array is then cleared when the write requests have been added
    - When asking for a buffer write, the data should be copied immediately so that the caller can reuse/free 
    - We can assume that the application will not submit additonal writes whilst the write list is being frained
    - If the buffer size is larger than can be accomodated by an empty heap, we'll fatal error
    - We'll use a write counter to flag if a buffer has pending writes which will be an atomic operation ( although buffer write queued from differing threads will be rare )
    - If we attempt to destroy a buffer whilst it is queued, it will result in a fatal error
- Buffer Submission flow
    - Buffer writing happens via Gpu_Buffer_Write
    - The buffer is flagged that it is to be wrritten to
    - The buffer is added to the buffer write list in the current render context, along with any information about the buffer write
    - We'll add the required copy operations to gpu queue when Gpu_RenderContext_BeginFrame is caled
    - Write occur in queue order - we can assume that overkapping writes are intended behaviour that is order dependent 
    - Once the buffer is encoded we can get rid of the staging * book keeping information 
    - The caller may destroy the engine buffer after encoding
    - Context destruction will discard pending write ops
    - Write that are lager than a staging heap should fail with a fatal error
    - Staging buffer can be released without issues as Metal will track its usage
    - The destination buffer will be tracked by the render code so it can stay edicent without being released
    - Reinitialisaing a buffer that has pending writes is an error and should not happen. Flag a fatal error when this happens. 






## Resource lifetime and reuse

- Resource ownership and destruction: 
    - Buffers and textures will remain owned by the the render code
- Reclaiming and reusing heap space after resource destruction: 
    - We'll take a GC approach. We'll evict heaps that no longer have any allocations
    - This means we only have a single active heap where we allocate from for each resource type
- Heap lifetime and cleanup: 
    - Heaps are deallocated as soon as they are empty / have no allocations
    - Any remaining heaps will be cleaned up when the render context is destroyed
- Device finalization with live resources: 
    - It's up to the application to manage this
- Heap aliasing and explicit placement: [ ] out of scope initially [ ] specify behavior: **TBD**

## Synchronization

- Define who ensures that GPU work has completed before resources or heap space are destroyed or reused: 
    - By defintion, the heap should be empty when resources are finally released by the GPU ( no GPU work pending )
- Define device finalization behavior when GPU work is in flight: 
    - This shouldn't happen - we shold wait until the GPU has no work in flight before finalisation
    - The render code itself will wait for the gpu to exaust its inflight work before cleaning up

- Define synchronization between heap allocation, destruction, and concurrent access: **TBD**
- Preserve the current requirement that callers synchronize buffer access with GPU work unless changed above.

## Acceptance tests

- Allocate and destroy heap-backed buffers for each supported storage mode.
- Verify shared-buffer mapping and offset writes, and GPU-only staging uploads and readback.
- Verify heap exhaustion and allocation failures follow the selected policy.
- Verify resource destruction and heap-space reuse follow the selected lifetime and synchronization rules.
- Verify required size and alignment handling and allocations near heap capacity.
- Verify device shutdown behavior with live resources and in-flight work.
- Run existing Metal buffer and texture-format tests to check for regressions.
- Exercise Metal API validation; test both ARC and non-ARC builds where supported.
