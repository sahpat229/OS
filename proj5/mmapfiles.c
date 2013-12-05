/******************************
 * 
 * Sharang Phadke
 * 11/06/2013
 * ECE 357: OS
 * Project 5 - memory-mapped files
 * 
 *****************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/mman.h>


void handle_error(const char *e){
    perror(e);
    exit(EXIT_FAILURE);
}


void setup(int *fd, char *fname, struct stat *s){
    *fd = open(fname, O_RDWR | O_APPEND);
    if(*fd < 0)
        handle_error("open");
    
    if(fstat(*fd, s) < 0)
        handle_error("fstat");
}


void a(){
    int fd;
    FILE * fp;
    struct stat s;
    char *m, *fname = "a.txt";

    setup(&fd, fname, &s);

    m = mmap(0, s.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if(m == MAP_FAILED)
        handle_error("mmap error in a");

    fp = fopen(m, "w");
    fprintf(stderr, "You should now see a SIGSEV\n");
    fwrite("hello", 1, strlen("hello"), fp);
}


void bc(int shared){
    int fd, rb;
    FILE * fp;
    struct stat s;
    char *m, *fname = "b.txt", *str = "hello";
    char *buffer = (char*)malloc(1024);

    setup(&fd, fname, &s);

    m = mmap(0, s.st_size, PROT_READ | PROT_WRITE,
            (shared) ? MAP_SHARED : MAP_PRIVATE, fd, 0);
    if(m == MAP_FAILED)
        handle_error("mmap in bc");

    fprintf(stderr, "Contents of file:\n", m);
    while((rb = read(fd, buffer, sizeof(buffer))) > 0){
        fprintf(stderr, buffer);
    }
    sprintf(m, str);
    fprintf(stderr,
            (shared)
            ? "Contents of file should be different:\n"
            : "Contents of file should not be different:\n"
            , m);
    while((rb = read(fd, buffer, sizeof(buffer))) > 0){
        fprintf(stderr, buffer);
    }
}


void d(){
    int fd;
    FILE * fp;
    struct stat s;
    char *m, *fname = "b.txt", *str = "goodbye";

    setup(&fd, fname, &s);

    fprintf(stderr, "size of file before write: %d\n", (int)s.st_size);

    m = mmap(0, s.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(m == MAP_FAILED)
        handle_error("mmap in d");

    fprintf(stderr, "Contents of file:\n%s\n", m);
    sprintf(m + strlen(m), str);

    if(fstat(fd, &s) < 0)
        handle_error("fstat in d");

    fprintf(stderr, "size of file after write: %d\n", (int)s.st_size);
}


void e(){
    int fd, rb;
    FILE * fp;
    struct stat s;
    char *m, *fname = "b.txt", str1 = "hello", str2 = "goodbye";
    char *buffer = (char*)malloc(1024);

    setup(&fd, fname, &s);

    fprintf(stderr, "size of file before write: %d\n", (int)s.st_size);

    m = mmap(0, s.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(m == MAP_FAILED)
        handle_error("mmap in e");

    fprintf(stderr, "Contents of file:\n%s\n", m);
    sprintf(m + strlen(m), str1);

    if(lseek(fd, 100, SEEK_END) < 0)
        handle_error("lseek in e");
    if(write(fd, str2, strlen(str2)) != strlen(str2))
        handle_error("write in e");

    if(fstat(fd, &s) < 0)
        handle_error("fstat in e");
    fprintf(stderr, "size of file after write (should increase): %d\n", (int)s.st_size);

    if(lseek(fd, 0, SEEK_SET) < 0)
        handle_error("lseek in e");
    fprintf(stderr, "New contents of file (holes will not print out):\n");
    errno = 0;
    while((rb = read(fd, buffer, sizeof(buffer))) > 0){
        fprintf(stderr, buffer);
    }
    if(errno != 0)
        handle_error("read error in e");
}


void f(){
    int fd, rb;
    FILE * fp;
    struct stat s;
    char *m, *fname = "small.txt";

    setup(&fd, fname, &s);

    m = mmap(0, 8192, PROT_READ, MAP_SHARED, fd, 0);
    if(m == MAP_FAILED)
        handle_error("mmap in f");

    fprintf(stderr, m[4000]);
    fprintf(stderr, "should have been able to read from first page\n");

    fprintf(stderr, "should not be able to read from second page (enjoy the following SIGBUS)\n");
    fprintf(stderr, m[6000]);
}


int main(int argc, char *argv[]){
    if(argc != 2)
        handle_error("this problem has 6 parts (a-f). pass in the part you want to test as argv[1]\n");
    char *c = argv[1];

    switch(c[0]){
        case('a'):
            printf("part a:\n");
            a();
            break;
        case('b'):
            printf("part b:\n");
            bc(1);
            break;
        case('c'):
            printf("part c:\n");
            bc(0);
            break;
        case('d'):
            printf("part d:\n");
            d();
            break;
        case('e'):
            printf("part e:\n");
            e();
            break;
        case('f'):
            printf("part f:\n");
            f();
            break;
        default:
            printf("this problem has 6 parts (a-f). pass in the part you want to test as argv[1]\n");
            return 0;
    }
    return 0;
}
