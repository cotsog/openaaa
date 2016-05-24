/*                                                                              
 * $elf.h                                           Daniel Kubec <niel@rtfm.cz> 
 *                                                                              
 * This software may be freely distributed and used according to the terms      
 * of the GNU Lesser General Public License.                                    
 */

#include <core/lib.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <dlfcn.h>
#include <link.h>
#include <sys/ptrace.h>

#include <elf.h>
#include <libelf.h>

#ifndef PAGE_SIZE                                                               
#define PAGE_SIZE CPU_PAGE_SIZE                                                 
#endif 

#ifdef __x86_64
    #define Elf_Ehdr Elf64_Ehdr
    #define Elf_Shdr Elf64_Shdr
    #define Elf_Sym Elf64_Sym
    #define Elf_Rel Elf64_Rela
    #define ELF_R_SYM ELF64_R_SYM
    #define REL_DYN ".rela.dyn"
    #define REL_PLT ".rela.plt"
#else
    #define Elf_Ehdr Elf32_Ehdr
    #define Elf_Shdr Elf32_Shdr
    #define Elf_Sym Elf32_Sym
    #define Elf_Rel Elf32_Rel
    #define ELF_R_SYM ELF32_R_SYM
    #define REL_DYN ".rel.dyn"
    #define REL_PLT ".rel.plt"
#endif 

/*
 
Elf file type is EXEC (Executable file)
Entry point 0x804a7a0
There are 9 program headers, starting at offset 52

Program Headers:
Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
PHDR           0x000034 0x08048034 0x08048034 0x00120 0x00120 R E 0x4
INTERP         0x000154 0x08048154 0x08048154 0x00013 0x00013 R   0x1
[Requesting program interpreter: /lib/ld-linux.so.2]
LOAD           0x000000 0x08048000 0x08048000 0x10fc8 0x10fc8 R E 0x1000
LOAD           0x011000 0x08059000 0x08059000 0x0038c 0x01700 RW  0x1000
DYNAMIC        0x01102c 0x0805902c 0x0805902c 0x000f8 0x000f8 RW  0x4
NOTE           0x000168 0x08048168 0x08048168 0x00020 0x00020 R   0x4
TLS            0x011000 0x08059000 0x08059000 0x00000 0x0005c R   0x4
GNU_EH_FRAME   0x00d3c0 0x080553c0 0x080553c0 0x00c5c 0x00c5c R   0x4
GNU_STACK      0x000000 0x00000000 0x00000000 0x00000 0x00000 RW  0x4

The first (lowest) LOAD segment's virtual address is the default load base 
of the file. You can see it's 0x08048000 for this file.

*/

/* read data from location addr */
static void *
read_data(int pid ,unsigned long addr ,void *vptr ,int len)
{
	int i , count;
	long word;
	unsigned long *ptr = (unsigned long *) vptr;
	count = i = 0;

	while (count < len) {
		word = ptrace(PTRACE_PEEKTEXT ,pid ,addr+count, NULL);
		count += 4;
		ptr[i++] = word;
	}
}

/* write data to location addr */	
static void
write_data(int pid ,unsigned long addr ,void *vptr,int len)
{
	int i , count;
	long word;
	i = count = 0;
	while (count < len) {
		memcpy(&word , vptr+count , sizeof(word));
		word = ptrace(PTRACE_POKETEXT, pid, addr+count , word);
		count +=4;
	}
}

struct link_map *
locate_linkmap(int pid)
{
	Elf32_Ehdr *ehdr = malloc(sizeof(Elf32_Ehdr));
	Elf32_Phdr *phdr = malloc(sizeof(Elf32_Phdr));
	Elf32_Dyn  *dyn  = malloc(sizeof(Elf32_Dyn));
	Elf32_Word got;

	struct link_map *l = malloc(sizeof(struct link_map));
	unsigned long phdr_addr, dyn_addr, map_addr;

	/*
	 * first we check from elf header, mapped at 0x08048000, the offset
	 * to the program header table from where we try to locate
	 * PT_DYNAMIC section.
	 */

	read_data(pid, 0x08048000 , ehdr , sizeof(Elf32_Ehdr));
	phdr_addr = 0x08048000 + ehdr->e_phoff;

	printf("program header at %p\n", phdr_addr);
	read_data(pid , phdr_addr, phdr , sizeof(Elf32_Phdr));

	while (phdr->p_type != PT_DYNAMIC ) {
		read_data(pid, phdr_addr += sizeof(Elf32_Phdr), phdr,
		          sizeof(Elf32_Phdr));
	}

	/*
	 * now go through dynamic section until we find address of the GOT
	 */

	read_data(pid, phdr->p_vaddr, dyn, sizeof(Elf32_Dyn));
	dyn_addr = phdr->p_vaddr;
	while ( dyn->d_tag != DT_PLTGOT ) {
		read_data(pid, dyn_addr += sizeof(Elf32_Dyn), dyn, sizeof(Elf32_Dyn));
	}
	got = (Elf32_Word) dyn->d_un.d_ptr;
	got += 4;		/* second GOT entry, remember? */
	/* now just read first link_map item and return it */
	read_data(pid, (unsigned long)got, &map_addr , 4);
	read_data(pid, map_addr, l , sizeof(struct link_map));

	free(phdr);
	free(ehdr);
	free(dyn);

	return l;
}

/*
 *
uint32_t
vaddr32_foff(char *filepath, uint32_t vaddr)
{
	int fd = open(filepath, O_RDONLY);
	if (fd == -1)
		return (uint32_t)~0U;

	Elf *e = elf_begin(fd, ELF_C_READ, NULL);
	Elf_Scn *scn = NULL;

	uint32_t offset = 0;
	while((scn = elf_nextscn(e, scn)) != NULL) {
		Elf32_Shdr * shdr = elf32_getshdr(scn);
		if(vaddr >= shdr->sh_addr && (vaddr <= (shdr->sh_addr + shdr->sh_size))) {
			offset = shdr->sh_offset + (vaddr - shdr->sh_addr);
			break;
		}
	}

	elf_end(e);
	close(fd);
	return offset;
}

uint64_t
vaddr64_foff(char *filepath, uint64_t vaddr)
{
	int fd = open(filepath, O_RDONLY);
	if (fd == -1)                                                           
		return (uint32_t)~0U; 

	Elf *e = elf_begin(fd, ELF_C_READ, NULL);
	Elf_Scn *scn = NULL;

	uint64_t offset = 0;
	while((scn = elf_nextscn(e, scn)) != NULL) {
		Elf64_Shdr * shdr = elf64_getshdr(scn);
		if(vaddr >= shdr->sh_addr && (vaddr <= (shdr->sh_addr + shdr->sh_size))) {
			offset = shdr->sh_offset + (vaddr - shdr->sh_addr);
			break;
		}
	}

	elf_end(e);
	close(fd);
	return offset;
}

*/

static int
read_header(int d, Elf_Ehdr **header)
{
	*header = (Elf_Ehdr *)malloc(sizeof(Elf_Ehdr));

	if (lseek(d, 0, SEEK_SET) < 0) {
		free(*header);
		return errno;
	}

	if (read(d, *header, sizeof(Elf_Ehdr)) <= 0) {
		free(*header);
		return errno = EINVAL;
	}

	return 0;
}

static int
read_section_table(int d, Elf_Ehdr const *header, Elf_Shdr **table)
{
	if (NULL == header)
		return EINVAL;

	size_t size = header->e_shnum * sizeof(Elf_Shdr);
	*table = (Elf_Shdr *)malloc(size);

	if (lseek(d, header->e_shoff, SEEK_SET) < 0) {
		free(*table);

		return errno;
	}

	if (read(d, *table, size) <= 0) {
		free(*table);

		return errno = EINVAL;
	}

	return 0;
}

static int
read_string_table(int d, Elf_Shdr const *section, char const **strings)
{
	if (NULL == section)
		return EINVAL;

	*strings = (char const *)malloc(section->sh_size);

	if (lseek(d, section->sh_offset, SEEK_SET) < 0) {
		free((void *)*strings);

		return errno;
	}

	if (read(d, (char *)*strings, section->sh_size) <= 0) {
		free((void *)*strings);

		return errno = EINVAL;
	}

	return 0;
}

static int
read_symbol_table(int d, Elf_Shdr const *section, Elf_Sym **table)
{
	if (NULL == section)
		return EINVAL;

	*table = (Elf_Sym *)malloc(section->sh_size);

	if (lseek(d, section->sh_offset, SEEK_SET) < 0) {
		free(*table);

		return errno;
	}

	if (read(d, *table, section->sh_size) <= 0) {
		free(*table);

		return errno = EINVAL;
	}

	return 0;
}

static int
read_relocation_table(int d, Elf_Shdr const *section, Elf_Rel **table)
{
	if (NULL == section)
		return EINVAL;

	*table = (Elf_Rel *)malloc(section->sh_size);

	if (lseek(d, section->sh_offset, SEEK_SET) < 0)	{
		free(*table);

		return errno;
	}

	if (read(d, *table, section->sh_size) <= 0) {
		free(*table);

		return errno = EINVAL;
	}

	return 0;
}

static int
section_by_index(int d, size_t const index, Elf_Shdr **section)
{
	Elf_Ehdr *header = NULL;
	Elf_Shdr *sections = NULL;
	size_t i;

	*section = NULL;

	if (read_header(d, &header) || read_section_table(d, header, &sections))
		return errno;

	if (index < header->e_shnum) {
		*section = (Elf_Shdr *)malloc(sizeof(Elf_Shdr));

		if (NULL == *section) {
			free(header);
			free(sections);

			return errno;
		}

	memcpy(*section, sections + index, sizeof(Elf_Shdr));

	} else
		return EINVAL;

	free(header);
	free(sections);

	return 0;
}

static int
section_by_type(int d, size_t const section_type, Elf_Shdr **section)
{
	Elf_Ehdr *header = NULL;
	Elf_Shdr *sections = NULL;
	size_t i;

	*section = NULL;

	if (read_header(d, &header) ||read_section_table(d, header, &sections))
		return errno;

	for (i = 0; i < header->e_shnum; ++i)
		if (section_type == sections[i].sh_type) {
			*section = (Elf_Shdr *)malloc(sizeof(Elf_Shdr));

			if (NULL == *section) {
				free(header);
				free(sections);

			return errno;
			}

		memcpy(*section, sections + i, sizeof(Elf_Shdr));

		break;
		}

	free(header);
	free(sections);

	return 0;
}

static int
section_by_name(int d, char const *section_name, Elf_Shdr **section)
{
	Elf_Ehdr *header = NULL;
	Elf_Shdr *sections = NULL;
	char const *strings = NULL;
	size_t i;

	*section = NULL;

	if (read_header(d, &header) ||
	    read_section_table(d, header, &sections) ||
	    read_string_table(d, &sections[header->e_shstrndx], &strings))
		return errno;

	for (i = 0; i < header->e_shnum; ++i)
		if (!strcmp(section_name, &strings[sections[i].sh_name])) {
			*section = (Elf_Shdr *)malloc(sizeof(Elf_Shdr));

			if (NULL == *section) {
				free(header);
				free(sections);
				free((void *)strings);

				return errno;
			}

		memcpy(*section, sections + i, sizeof(Elf_Shdr));

		break;
		}

	free(header);
	free(sections);
	free((void *)strings);

	return 0;
}

static int
symbol_by_name(int d, Elf_Shdr *section, char const *name, Elf_Sym **symbol, size_t *index)
{
	Elf_Shdr *strings_section = NULL;
	char const *strings = NULL;
	Elf_Sym *symbols = NULL;
	size_t i, amount;

	*symbol = NULL;
	*index = 0;

	if (section_by_index(d, section->sh_link, &strings_section) ||
	    read_string_table(d, strings_section, &strings) ||
	    read_symbol_table(d, section, &symbols))
		return errno;

	amount = section->sh_size / sizeof(Elf_Sym);

	for (i = 0; i < amount; ++i)
		if (!strcmp(name, &strings[symbols[i].st_name])) {
			*symbol = (Elf_Sym *)malloc(sizeof(Elf_Sym));

			if (NULL == *symbol) {
				free(strings_section);
				free((void *)strings);
				free(symbols);

				return errno;
			}

			memcpy(*symbol, symbols + i, sizeof(Elf_Sym));
			*index = i;

			break;
		}

	free(strings_section);
	free((void *)strings);
	free(symbols);

	return 0;
} 

void *
elf_dyn_sym_wrap(char const *file, void const *addr,
                 char const *name, void const *sub)
{
	/* section headers .dynsym .rel.plt .rel.dyn */
	/* array with .rel.plt and .rel.dyn entries */
	Elf_Shdr *dynsym = NULL, *rel_plt = NULL,*rel_dyn = NULL;
	Elf_Sym	*symbol = NULL;
	Elf_Rel *rel_plt_table = NULL, *rel_dyn_table = NULL;

	size_t index = 0, rel_plt_amount = 0, rel_dyn_amount = 0, *name_address = NULL;
	void *original = NULL;  //address of the symbol being substituted

	int fd;
	if ((fd = open(file, O_RDONLY)) < 0)
		return NULL;

	if (section_by_type(fd, SHT_DYNSYM, &dynsym))
		goto failed;

	if (symbol_by_name(fd, dynsym, name, &symbol, &index))
		goto failed;

	if (section_by_name(fd, REL_PLT, &rel_plt))
		goto failed;

	if (section_by_name(fd, REL_DYN, &rel_dyn))
		goto failed;

	free(dynsym);
	free(symbol);

	/* .rel.plt array */
	rel_plt_table = (Elf_Rel *)(((size_t)addr) + rel_plt->sh_addr);
	rel_plt_amount = rel_plt->sh_size / sizeof(Elf_Rel);
	/* .rel.dyn array */
	rel_dyn_table = (Elf_Rel *)(((size_t)addr) + rel_dyn->sh_addr);
	rel_dyn_amount = rel_dyn->sh_size / sizeof(Elf_Rel);

	free(rel_plt);
	free(rel_dyn);
	close(fd);

	/*
	 * got ".rel.plt" (needed for PIC) table
	 * and ".rel.dyn" (for non-PIC) table and the symbol's index
	 */

	for (size_t i = 0; i < rel_plt_amount; ++i) { 
		if (ELF_R_SYM(rel_plt_table[i].r_info) != index) 
			continue;

		/* if we found the symbol to substitute in ".rel.plt"*/
		original = (void *)*(size_t *)(((size_t)addr) + rel_plt_table[i].r_offset);  
		*(size_t *)(((size_t)addr) + rel_plt_table[i].r_offset) = (size_t)sub;
		break;  
	}

	if (original)
		return original;

	/* we will get here only with 32-bit non-PIC modul */
	/* .rel.dyn table */    
	for (size_t i = 0; i < rel_dyn_amount; ++i) { 

		if (ELF_R_SYM(rel_dyn_table[i].r_info) == index) {
        
			name_address = (size_t *)(((size_t)addr) + rel_dyn_table[i].r_offset);
			//get the relocation address (address of a relative CALL (0xE8) instruction's argument)

			if (!original)
				original = (void *)(*name_address + (size_t)name_address + sizeof(size_t));
			//calculate an address of the original function by a relative CALL (0xE8) instruction's argument

			mprotect((void *)(((size_t)name_address) & (((size_t)-1) ^ (PAGE_SIZE - 1))), 
			         PAGE_SIZE, PROT_READ | PROT_WRITE);  //mark a memory page that contains the relocation as writable

			if (errno)
				return NULL;

			*name_address = (size_t)sub - (size_t)name_address - sizeof(size_t);
			//calculate a new relative CALL (0xE8) instruction's argument for the substitutional function and write it down

			mprotect((void *)(((size_t)name_address) & (((size_t)-1) ^ (PAGE_SIZE - 1))), 
			        PAGE_SIZE, PROT_READ | PROT_EXEC);  //mark a memory page that contains the relocation back as executable

			if (errno) {
				*name_address = (size_t)original - 
				(size_t)name_address - sizeof(size_t);  //then restore the original function address

				return NULL;
			}
		}
	}


    return original;

failed:

	if (dynsym)
		free(dynsym);
	if (rel_plt)
		free(rel_plt);
	if (rel_dyn)
		free(rel_dyn);
	if (symbol)
		free(symbol);

	close(fd);

        return NULL;	
}
