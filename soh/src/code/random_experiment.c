#include "random_experiment.h"

unsigned long (*randomUint32)(unsigned long) = &standardRandomUint32;
bool CSPRNG_INITIALIZED = false;

#if defined(_WIN32)
#include <medparam.h>
#include <wincrypt.h>
static HCRYPTPROV cryptProvider;

bool cryptoRandomInit()
{
    bool success = CryptAcquireContext(&cryptProvider, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
    if (success) {
        randomUint32 = &cryptoRandomUint32;
        CSPRNG_INITIALIZED = true;
    }
    return success;
}
void cryptoRandomTearDown()
{
    if (cryptProvider) {
        CryptReleaseContext(cryptProvider, 0);
    }
}

unsigned long cryptoRandomUint32(unsigned long seed)
{
    (void)seed;
    unsigned long randomBits = 0;
    CryptGenRandom(cryptProvider, 4, &randomBits);
    return randomBits;
}
#endif // _WIN32

unsigned long standardRandomUint32(unsigned long seed)
{
    return (seed * 1664525) + 1013904223;
}
