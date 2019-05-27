#ifndef WSDECODE_H_
#define WSDECODE_H_

void get_payload(const unsigned char *msg, unsigned char *payload);

void set_payload(const unsigned char *payload, unsigned char *msg);

#endif
