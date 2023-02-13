#pragma once
#define dgEfcFlashBlob_000000 "__OTR__objects/object_efc_flash/gEfcFlashBlob_000000"
#ifdef _WIN32
static const __declspec(align(2)) char gEfcFlashBlob_000000[] = dgEfcFlashBlob_000000;
#else
static const char gEfcFlashBlob_000000[] __attribute__((aligned (2))) = dgEfcFlashBlob_000000;
#endif
            

