#include "shake.h"
#include "hashmap.h"
#include "mysha.h"
#include "mybase64.h"
#include <string.h>
#include <stdio.h>
#include <regex.h>
#include <stdlib.h>

void wsshake_res(const char* http_req, char *http_res){
    regex_t reg;
    regmatch_t pmatch[2];
    char match[100];
    char *pattern = "Sec-WebSocket-Key:\\s([+/0-9a-zA-Z=]*?)\r\n";
    int err;
    regcomp(&reg, pattern, REG_EXTENDED);
    err = regexec(&reg,http_req,2,pmatch,0);

    if(err == REG_NOMATCH){
        printf("no match\n");
        exit(-1);
    }

    int len = pmatch[1].rm_eo-pmatch[1].rm_so;
    if(len){
        memset(match,0,sizeof(match));
        memcpy(match,http_req+pmatch[1].rm_so,len);
        printf("key is:%s\n",match);
        printf("key len is:%d\n", strlen(match));
        //计算返回key
        char mask[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
        strcat(match, mask);
        char res_key[60];
        char md[100];
        memset(md, 0, sizeof(md));
        mysha(match, md);
        memset(res_key,0,sizeof(res_key));
        base64EncodeString(md, res_key);
        strcat(http_res, "HTTP/1.1 101 Switching Protocols\r\n");
        strcat(http_res, "Upgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: ");
        strcat(http_res, res_key); 
        strcat(http_res,"\r\n\r\n");
    }

}

/*int main(){
    char *aa = "Host: 120.79.83.128:4500\r\nAccept-Encoding: gzip, deflate\r\nAccept-Language: zh-CN,zh;q=0.9,en;q=0.8\r\nSec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\nSec-WebSocket-Extensions: permessage-deflate; client_max_window_bits";
    printf("ori aa is:%s\n", aa);
    printf("ori aa len is:%d\n", strlen(aa));
    char bb[1024];
    char *cc;
    memset(bb, 0, sizeof(bb));
    wsshake_res(aa, bb);
    printf("%s\n", bb);
    cc = malloc(strlen(bb)+1);
    strncpy(cc, bb, strlen(bb)+1); 
    printf("%s\n", cc);
    return 0;
}*/
