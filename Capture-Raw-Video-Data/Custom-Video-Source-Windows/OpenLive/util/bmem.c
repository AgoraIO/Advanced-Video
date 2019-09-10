/*
 * Copyright (c) 2013 Hugh Bailey <obs.jim@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdlib.h>
#include <string.h>
#include "base.h"
#include "bmem.h"
#include "platform.h"
#include "threading.h"

/*
 * NOTE: totally jacked the mem alignment trick from ffmpeg, credit to them:
 *   http://www.ffmpeg.org/
 */

#define ALIGNMENT 32

/* TODO: use memalign for non-windows systems */

static void *a_malloc(size_t size)
{
	return _aligned_malloc(size, ALIGNMENT);
}

static void *a_realloc(void *ptr, size_t size)
{
	return realloc(ptr, size);
}

static void a_free(void *ptr)
{
	_aligned_free(ptr);
}

static struct base_allocator alloc = {a_malloc, a_realloc, a_free};
static long num_allocs = 0;

void base_set_allocator(struct base_allocator *defs)
{
	memcpy(&alloc, defs, sizeof(struct base_allocator));
}

void *bmalloc(size_t size)
{
	void *ptr = alloc.malloc(size);
	if (!ptr && !size)
		ptr = alloc.malloc(1);
	if (!ptr) {
		os_breakpoint();
		bcrash("Out of memory while trying to allocate %lu bytes",
		       (unsigned long)size);
	}

	os_atomic_inc_long(&num_allocs);
	return ptr;
}

void *brealloc(void *ptr, size_t size)
{
	if (!ptr)
		os_atomic_inc_long(&num_allocs);

	ptr = alloc.realloc(ptr, size);
	if (!ptr && !size)
		ptr = alloc.realloc(ptr, 1);
	if (!ptr) {
		os_breakpoint();
		bcrash("Out of memory while trying to allocate %lu bytes",
		       (unsigned long)size);
	}

	return ptr;
}

void bfree(void *ptr)
{
	if (ptr)
		os_atomic_dec_long(&num_allocs);
	alloc.free(ptr);
}

long bnum_allocs(void)
{
	return num_allocs;
}

int base_get_alignment(void)
{
	return ALIGNMENT;
}

void *bmemdup(const void *ptr, size_t size)
{
	void *out = bmalloc(size);
	if (size)
		memcpy(out, ptr, size);

	return out;
}
