#include <stdio.h>
#include "wsdecode.h"
#include <stdint.h>
#include <string.h>

void get_payload(const unsigned char *recv, unsigned char *payload){
    uint8_t mask = recv[1] & 0x80;
    printf("mask is %d\n", mask);
    uint64_t payload_len = recv[1] & 0x7f;
    printf("payload_len is %d\n", payload_len);
    unsigned char mask_key[4];
    memset(mask_key, 0, 4);
    memcpy(mask_key, recv+2, 4); 
    printf("maks_key is %s\n", mask_key);
    if(mask == 128){
        printf("decode mask payload==\n");
        int i;
        for(i = 0; i<payload_len; i++){
            int j = i % 4;
   //         printf("m%d\n", mask_key[j]);
   //         printf("r%d\n", recv[6+i]);
            payload[i] = (unsigned char)(recv[6+i] ^ mask_key[j]);
        }
    }else{
        printf("decode payload==\n");
        memcpy(payload, recv+2, payload_len);
    }   

}

void set_payload(const unsigned char *payload, unsigned char *msg){
    int len = strlen(payload);
    msg[0] = (unsigned char)0x81;
    msg[1] = (unsigned char)(0x7f & len);
    
    int i;
    for(i = 0; i < len; i++){
        msg[i+2] = payload[i];
    }
    
}
