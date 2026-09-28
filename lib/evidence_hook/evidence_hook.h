#pragma once

// Evidence capture hook — firmware-side.
//
// Called by the native capture apps (Sub-GHz, NFC, LF RFID, iButton, ...)
// immediately after a file is saved. It appends one JSONL line to a shared
// inbox that the Evidence Logger FAP later imports and annotates:
//
//   /ext/apps_data/evidence/_inbox.jsonl
//
// Each line: {"ts":"...","source":"subghz","file":"<path>","fhash":"<sha256>"}
//
// Design goal: keep the firmware diff minimal. This module is self-contained
// (opens its own Storage record, computes its own SHA-256), so integrating it
// into a fork is: (1) build these files, (2) add ONE call per capture app.

#ifdef __cplusplus
extern "C" {
#endif

// Register a freshly-saved capture. `file_path` is the full path on /ext,
// `source` is a short tag ("subghz", "nfc", "rfid", "ibutton", ...).
// Safe to call from an app thread; failures are silent (never blocks a save).
void evidence_hook_register(const char* file_path, const char* source);

#ifdef __cplusplus
}
#endif
