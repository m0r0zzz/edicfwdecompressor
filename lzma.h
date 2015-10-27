#ifndef LZMA_H
#define LZMA_H

#include <stdint.h>
typedef uint32_t size_t;

int unlzma(uint8_t *buf, uint8_t *output, uint32_t out_len) __attribute__((section(".ARM.unlzma")));;

#endif
