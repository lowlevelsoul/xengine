# Metal Heap Support Specification

Status: Draft. Edit the choices below before implementation.

## Scope and ownership

- Resources: [ ] buffers only (recommended initially) [ ] buffers and textures
- Device ownership: [ ] keep the existing global Metal device [ ] introduce explicit `gpu_device_t` ownership
- Staging buffers: [ ] direct allocations [ ] heap allocations

The current Metal backend stores one global device. `gpu_device_t` is forward-declared, and buffer operations currently
ignore their device argument. Texture operations are not yet implemented.

## Heap policy

- Use separate heaps for incompatible storage modes, including shared and private storage.
- Heap allocation type: [ ] automatic (recommended initially) [ ] placement
- Heap capacity: [ ] fixed [ ] growable
- Configuration: [ ] API parameters [ ] engine constants
- Capacity and growth sizes: 
    - We'll support a maximum number of active heaps of 256, This will be specified in a define
    - Heaps are allocated in chunks of size that are specified in by the vakue given when crrating a gpu_render_context
- On exhaustion or allocation failure: [ ] create another heap [ ] fall back to direct allocation [ ] report failure
- Handle Metal's required resource sizes and alignments when sizing heaps and allocating resources.
- Unsupported heap configurations or resource options: 
    - We'll simply just assert when this happens. 

## Buffer behavior

- Shared buffers remain CPU-visible and mappable.
- GPU-only buffers remain private and cannot be mapped.
- GPU-only writes continue to use synchronous staging uploads.
- Preserve offset writes, bounds checking, and existing error reporting unless explicitly changed below.
- Changes to the public buffer or device API: 
    - We don't change the public API.
    - We will maintain a thread safe list of buffers and textures that are to be written to
    - The list will just be an array describing the various buffer/texture writes of 4096 entries ( use a defined for the size )
    - At the start of each frame, we'll create the appropriate blit/copy command encoders and and the commands
    - The array is then cleared when the write requests have been added

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
    - Thus shouldn't happen - we shold wait until the GPU has no work in flight before finalisation

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
