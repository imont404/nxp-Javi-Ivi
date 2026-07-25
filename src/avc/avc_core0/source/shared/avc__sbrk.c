/*
 * Bounded _sbrk for the AVC firmware.
 *
 * Why this exists
 * ---------------
 * The MCUXpresso-generated linker script grouped "libcr_newlib_nohost.a", an
 * NXP library that ships only with the IDE. Building with the standalone Arm
 * GNU toolchain requires replacing it with the standard "libnosys.a"
 * (see link/avc_core0_Debug_library.ld).
 *
 * Those two libraries do not manage the heap the same way:
 *
 *   libcr_newlib_nohost.a  provided __end_of_heap and __check_heap_overflow,
 *                          so heap growth was bounded.
 *   libnosys.a             provides the classic minimal _sbrk, which grows
 *                          from "end" with NO limit check at all.
 *
 * The linker script declares _HeapSize = 0x1000 with _pvHeapStart and
 * _pvHeapLimit, but libnosys knows nothing about those symbols, so swapping
 * libraries would have silently removed the heap bound.
 *
 * This file restores it. Defining _sbrk here overrides the library version,
 * because an object file in the link beats a member of an archive.
 *
 * Is the heap even used?
 * ----------------------
 * Yes, indirectly. Nothing in AVC calls malloc, but newlib's printf family
 * allocates for some conversions, and the overlay code formats floats
 * ("%.2f" in avc__update_overlay). Expected use is small and bounded; the
 * 4 KB region is ample. The point of this file is that exceeding it fails
 * cleanly rather than quietly corrupting whatever lies above the heap.
 */

#include <errno.h>
#include <stddef.h>

/* Provided by the linker script; see link/avc_core0_Debug.ld. */
extern char _pvHeapStart[];
extern char _pvHeapLimit[];

void *_sbrk(ptrdiff_t incr);

void *_sbrk(ptrdiff_t incr)
{
    static char *heap_current = NULL;
    char *previous;

    if (heap_current == NULL)
    {
        heap_current = _pvHeapStart;
    }

    /* Refuse growth past the linker-defined limit, and refuse shrinking below
     * the start. Returning (void *)-1 with ENOMEM is what newlib expects. */
    if (incr > 0)
    {
        if ((size_t)(_pvHeapLimit - heap_current) < (size_t)incr)
        {
            errno = ENOMEM;
            return (void *)-1;
        }
    }
    else if (incr < 0)
    {
        if ((heap_current + incr) < _pvHeapStart)
        {
            errno = ENOMEM;
            return (void *)-1;
        }
    }

    previous = heap_current;
    heap_current += incr;

    return (void *)previous;
}
