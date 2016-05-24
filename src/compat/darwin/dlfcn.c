
static bool
ptr_is_in_exe(const void *ptr, const struct mach_header *header, intptr_t *offset, uintptr_t *vmaddr, char *image_name)
{
    uint32_t i, count = _dyld_image_count();

    for (i = 0; i < count; i++) {
        header = _dyld_get_image_header(i);
        offset = _dyld_get_image_vmaddr_slide(i);

        uint32_t j = 0;
        struct load_command* cmd = (struct load_command*)((char *)header + sizeof(struct mach_header));
        if(header->magic == MH_MAGIC_64)
            cmd = (struct load_command*)((char *)header + sizeof(struct mach_header_64));

        while (j < header->ncmds) {
            if (cmd->cmd == LC_SEGMENT) {
                struct segment_command* seg = (struct segment_command*)cmd;
                if (((intptr_t)ptr >= (seg->vmaddr + offset)) && ((intptr_t)ptr < (seg->vmaddr + offset + seg->vmsize))) {
                    vmaddr = seg->vmaddr;
                    image_name = _dyld_get_image_name(i);
                    return true;
                }
            }
            if (cmd->cmd == LC_SEGMENT_64) {
                struct segment_command_64* seg = (struct segment_command_64*)cmd;
                if (((uintptr_t)ptr >= (seg->vmaddr + offset)) && ((uintptr_t)ptr < (seg->vmaddr + offset + seg->vmsize))) {
                    vmaddr = seg->vmaddr;
                    image_name = _dyld_get_image_name(i);
                    return true;
                }
            }

            j++;
            cmd = (struct load_command*)((char*)cmd + cmd->cmdsize);
        }
    }

    return false;
}
