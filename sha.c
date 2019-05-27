#include "mysha.h"
#include "string.h"
#include <openssl/sha.h>
#include <stdio.h>

#define SHA_DIGEST_LENGTH 20

void mysha(const char *src, char *md){
    unsigned char digest[SHA_DIGEST_LENGTH];
    SHA1(src, strlen(src), (unsigned char*)&digest);
    strcpy(md, digest);
}

/*int main(){
    char b[100];
    mysha("hello", b);
    printf("sha1:%s\n", b);
    return 0;
}*/
