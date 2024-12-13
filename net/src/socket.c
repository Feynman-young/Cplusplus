// Basic linux network programming api

#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>

// Big endian and little endian
void byteorder() {
    uint32_t temp = 1;
    printf("Byte Order: ");
    * (char *) &temp ? printf("Little endian\n") : printf("Big endian\n");
}

union packet {
    unsigned int value;
    uint8_t union_bytes[sizeof(unsigned int)];
};

void test_hton_ntohl() {
    union packet data;
    data.value = 0x12345678;

    printf("Little endian: ");
    for (int i = 0; i < sizeof(unsigned int); ++i) {
        printf("0x%02x ", data.union_bytes[i]);
    }
    printf("\n");

    data.value = htonl(data.value);

    printf("Big endian:    ");
    for (int i = 0; i < sizeof(unsigned int); ++i) {
        printf("0x%02x ", data.union_bytes[i]);
    }
    printf("\n");

    data.value = ntohl(data.value);

    printf("Little endian: ");
    for (int i = 0; i < sizeof(unsigned int); ++i) {
        printf("0x%02x ", data.union_bytes[i]);
    }
    printf("\n");
}

int main() {
    byteorder();
    test_hton_ntohl();
    return 0;
}