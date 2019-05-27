#include "mybase64.h"
#include <stdio.h>
#include <string.h>
#include "libb64/include/b64/cencode.h"
#include "libb64/include/b64/cdecode.h"


void base64EncodeString(const char *textToEncode, char *buffer)
{
    base64_encodestate state;
    base64_init_encodestate(&state);
    int numberOfEncodedBytes = base64_encode_block(textToEncode, strlen(textToEncode), buffer, &state);
    numberOfEncodedBytes += base64_encode_blockend(buffer + numberOfEncodedBytes, &state);
    buffer[numberOfEncodedBytes-1] = '\0';
}
void base64DecodeString(const char *codeToDecode, char *buffer)
{
    base64_decodestate state;
    base64_init_decodestate(&state);
    int numberOfCharacters = base64_decode_block(codeToDecode, strlen(codeToDecode), buffer, &state);
    buffer[numberOfCharacters] = '\0';
}

/*int main(){
    char *a = "hello";
    char *b;
    char aa[20];
    memset(aa, 0, strlen(aa));
    strcat(aa, a);
    char *bb;
    //base64EncodeString(a, b);
    //printf("encode:%s\n", b);
    base64EncodeString(aa, bb);
    printf("encode:%s\n", bb);
    return 0;
}*/
