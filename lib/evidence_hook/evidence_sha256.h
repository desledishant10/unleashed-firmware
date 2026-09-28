#pragma once

#include <stddef.h>
#include <stdint.h>

// Self-contained SHA-256 (no SDK / mbedtls dependency, so it links on any
// firmware fork). Public-domain algorithm.

typedef struct {
    uint8_t data[64];
    uint32_t datalen;
    uint64_t bitlen;
    uint32_t state[8];
} EvidenceSha256;

void evidence_sha256_init(EvidenceSha256* ctx);
void evidence_sha256_update(EvidenceSha256* ctx, const uint8_t* data, size_t len);
void evidence_sha256_final(EvidenceSha256* ctx, uint8_t hash[32]);

// Convenience: hash a buffer / string into a 64-char lowercase hex string
// (out must hold 65 bytes, includes the NUL terminator).
void evidence_sha256_hex(const uint8_t* data, size_t len, char out[65]);
void evidence_sha256_str(const char* s, char out[65]);
