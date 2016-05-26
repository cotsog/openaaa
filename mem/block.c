/*
 * The MIT License (MIT)                            Variable-size memory blocks
 *                               Copyright (c) 2015 Daniel Kubec <niel@rtfm.cz> 
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy 
 * of this software and associated documentation files (the "Software"),to deal 
 * in the Software without restriction, including without limitation the rights 
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include <sys/compiler.h>
#include <sys/cpu.h>
#include <mem/alloc.h>
#include <mem/page.h>
#include <mem/vm.h>
#include <mem/block.h>
	
void *
mem_vblock_alloc(unsigned int size)
{
	struct mem_vblock *b = vm_page_alloc(size + _align_obj(sizeof(*b))) + size;
	b->size = size;
	b->node.next = NULL;
	return b;
}

void
mem_vblock_free(struct mem_vblock *b)
{
	vm_page_free(b - b->size, b->size + _align_obj(sizeof(*b)));
}

void
mem_vblock_purge(struct mem_vblock *mmv)
{
	struct mem_vblock *it;
	mem_vblock_for_each_safe(mmv, it) {
		mem_vblock_unlink(mmv, it);
		mem_vblock_free(mmv);
	}
}