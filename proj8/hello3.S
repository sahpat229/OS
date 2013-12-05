##############################
#Sharang Phadke
#11/26/2013
#ECE 357: OS
#
#hello3.S
##############################

.global _start

.text
_start:
     movl    $len, %edx         #message length
     movl    $msg, %ecx         #message to write
     movl    $1, %ebx           #file descriptor (stdout)
     movl    $4, %eax           #code for write syscall
     int     $0x80              #syscall

     movl    $1, %ebx           #non-zero return value in ebx
     movl    $1, %eax           #code for exit syscall
     int     $0x80              #syscall
                                #valus of $ is ?

.data
msg:    .ascii "hello\n"
        len = . - msg
