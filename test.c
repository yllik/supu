#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(){
    char *s = (char*)malloc(9);
    char *str = "hello";
    strcpy(s, str);
    printf("%s\n", s);
    free(s);
}
