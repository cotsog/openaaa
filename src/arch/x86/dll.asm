_asm {
//   Placeholder for the return address
push 0xDEADBEEF

//   Save the flags and registers
pushfd
pushad

//   Placeholder for the string address and LoadLibrary
push 0xDEADBEEF
mov eax, 0xDEADBEEF

//   Call LoadLibrary with the string parameter
call eax

//   Restore the registers and flags
popad
popfd
      
//   Return control to the hijacked thread
ret
}
