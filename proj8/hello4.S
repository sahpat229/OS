##############################
#Sharang Phadke
#11/26/2013
#ECE 357: OS
#
#hello4.S
##############################

.global _start

.text
_start:
     movl    $len, %edx         #message length
     movl    $msg, %ecx         #message to write
     movl    $1, %ebx           #file descriptor (stdout)
     movl    $4, %eax           #code for write syscall
     int     $0x80              #syscall

     movl    $0, %ebx           #put return value in ebx
     movl    $400, %eax         #invalid syscall code
     int     $0x80              #syscall -- SIGSEGV

.data
msg:    .ascii "hello\n"
        len = . - msg
