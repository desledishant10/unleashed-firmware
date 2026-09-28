#include "evidence_hook.h"
#include "evidence_sha256.h"

#include <furi.h>
#include <furi_hal_rtc.h>
#include <datetime/datetime.h>
#include <storage/storage.h>
#include <string.h>

#define EVIDENCE_DIR   EXT_PATH("apps_data/evidence")
#define EVIDENCE_INBOX EXT_PATH("apps_data/evidence/_inbox.jsonl")

static bool evidence_hook_file_sha(Storage* storage, const char* path, char out[65]) {
    out[0] = 0;
    File* f = storage_file_alloc(storage);
    bool ok = storage_file_open(f, path, FSAM_READ, FSOM_OPEN_EXISTING);
    if(ok) {
        EvidenceSha256 ctx;
        evidence_sha256_init(&ctx);
        uint8_t buf[256];
        size_t n;
        while((n = storage_file_read(f, buf, sizeof(buf))) > 0) {
            evidence_sha256_update(&ctx, buf, n);
        }
        uint8_t h[32];
        evidence_sha256_final(&ctx, h);
        static const char hx[] = "0123456789abcdef";
        for(int i = 0; i < 32; ++i) {
            out[i * 2] = hx[h[i] >> 4];
            out[i * 2 + 1] = hx[h[i] & 0x0f];
        }
        out[64] = 0;
    }
    storage_file_close(f);
    storage_file_free(f);
    return ok;
}

void evidence_hook_register(const char* file_path, const char* source) {
    if(!file_path || !source) return;

    Storage* storage = furi_record_open(RECORD_STORAGE);

    // ensure the evidence dir exists (harmless if it already does)
    storage_common_mkdir(storage, EXT_PATH("apps_data"));
    storage_common_mkdir(storage, EVIDENCE_DIR);

    char fhash[65];
    if(!evidence_hook_file_sha(storage, file_path, fhash)) fhash[0] = 0;

    DateTime dt;
    furi_hal_rtc_get_datetime(&dt);

    FuriString* line = furi_string_alloc();
    furi_string_printf(
        line,
        "{\"ts\":\"%04d-%02d-%02dT%02d:%02d:%02d\",\"source\":\"%s\",\"file\":\"%s\","
        "\"fhash\":\"%s\"}\n",
        dt.year,
        dt.month,
        dt.day,
        dt.hour,
        dt.minute,
        dt.second,
        source,
        file_path,
        fhash);

    File* f = storage_file_alloc(storage);
    if(storage_file_open(f, EVIDENCE_INBOX, FSAM_WRITE, FSOM_OPEN_APPEND)) {
        storage_file_write(f, furi_string_get_cstr(line), furi_string_size(line));
    }
    storage_file_close(f);
    storage_file_free(f);

    furi_string_free(line);
    furi_record_close(RECORD_STORAGE);
}
