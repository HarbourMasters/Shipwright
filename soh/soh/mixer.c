//! This file is always optimized by a rule in the CMakeList. This is done because the SIMD functions are very large
//! when unoptimized and clang does not allow optimizing a single function.
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "mixer.h"
#ifndef __clang__
#pragma GCC optimize("unroll-loops")
#endif

#define ROUND_UP_64(v) (((v) + 63) & ~63)
#define ROUND_UP_32(v) (((v) + 31) & ~31)
#define ROUND_UP_16(v) (((v) + 15) & ~15)
#define ROUND_UP_8(v) (((v) + 7) & ~7)
#define ROUND_DOWN_16(v) ((v) & ~0xf)

#define DMEM_BUF_SIZE (0x1000 - 0x3C0 - 0x40)
#define BUF_U8(a) (rspa.buf.as_u8 + ((a)-0x3C0))
#define BUF_S16(a) (rspa.buf.as_s16 + ((a)-0x3C0) / sizeof(int16_t))

static struct {
    uint16_t in;
    uint16_t out;
    uint16_t nbytes;

    uint16_t vol[2];
    uint16_t rate[2];
    uint16_t vol_wet;
    uint16_t rate_wet;

    ADPCM_STATE* adpcm_loop_state;

    int16_t adpcm_table[8][2][8];

    uint16_t filter_count;
    int16_t filter[8];

    union {
        int16_t as_s16[DMEM_BUF_SIZE / sizeof(int16_t)];
        uint8_t as_u8[DMEM_BUF_SIZE];
    } buf;
} rspa;

static int16_t resample_table[64][4] = {
    { 0x0c39, 0x66ad, 0x0d46, 0xffdf }, { 0x0b39, 0x6696, 0x0e5f, 0xffd8 }, { 0x0a44, 0x6669, 0x0f83, 0xffd0 },
    { 0x095a, 0x6626, 0x10b4, 0xffc8 }, { 0x087d, 0x65cd, 0x11f0, 0xffbf }, { 0x07ab, 0x655e, 0x1338, 0xffb6 },
    { 0x06e4, 0x64d9, 0x148c, 0xffac }, { 0x0628, 0x643f, 0x15eb, 0xffa1 }, { 0x0577, 0x638f, 0x1756, 0xff96 },
    { 0x04d1, 0x62cb, 0x18cb, 0xff8a }, { 0x0435, 0x61f3, 0x1a4c, 0xff7e }, { 0x03a4, 0x6106, 0x1bd7, 0xff71 },
    { 0x031c, 0x6007, 0x1d6c, 0xff64 }, { 0x029f, 0x5ef5, 0x1f0b, 0xff56 }, { 0x022a, 0x5dd0, 0x20b3, 0xff48 },
    { 0x01be, 0x5c9a, 0x2264, 0xff3a }, { 0x015b, 0x5b53, 0x241e, 0xff2c }, { 0x0101, 0x59fc, 0x25e0, 0xff1e },
    { 0x00ae, 0x5896, 0x27a9, 0xff10 }, { 0x0063, 0x5720, 0x297a, 0xff02 }, { 0x001f, 0x559d, 0x2b50, 0xfef4 },
    { 0xffe2, 0x540d, 0x2d2c, 0xfee8 }, { 0xffac, 0x5270, 0x2f0d, 0xfedb }, { 0xff7c, 0x50c7, 0x30f3, 0xfed0 },
    { 0xff53, 0x4f14, 0x32dc, 0xfec6 }, { 0xff2e, 0x4d57, 0x34c8, 0xfebd }, { 0xff0f, 0x4b91, 0x36b6, 0xfeb6 },
    { 0xfef5, 0x49c2, 0x38a5, 0xfeb0 }, { 0xfedf, 0x47ed, 0x3a95, 0xfeac }, { 0xfece, 0x4611, 0x3c85, 0xfeab },
    { 0xfec0, 0x4430, 0x3e74, 0xfeac }, { 0xfeb6, 0x424a, 0x4060, 0xfeaf }, { 0xfeaf, 0x4060, 0x424a, 0xfeb6 },
    { 0xfeac, 0x3e74, 0x4430, 0xfec0 }, { 0xfeab, 0x3c85, 0x4611, 0xfece }, { 0xfeac, 0x3a95, 0x47ed, 0xfedf },
    { 0xfeb0, 0x38a5, 0x49c2, 0xfef5 }, { 0xfeb6, 0x36b6, 0x4b91, 0xff0f }, { 0xfebd, 0x34c8, 0x4d57, 0xff2e },
    { 0xfec6, 0x32dc, 0x4f14, 0xff53 }, { 0xfed0, 0x30f3, 0x50c7, 0xff7c }, { 0xfedb, 0x2f0d, 0x5270, 0xffac },
    { 0xfee8, 0x2d2c, 0x540d, 0xffe2 }, { 0xfef4, 0x2b50, 0x559d, 0x001f }, { 0xff02, 0x297a, 0x5720, 0x0063 },
    { 0xff10, 0x27a9, 0x5896, 0x00ae }, { 0xff1e, 0x25e0, 0x59fc, 0x0101 }, { 0xff2c, 0x241e, 0x5b53, 0x015b },
    { 0xff3a, 0x2264, 0x5c9a, 0x01be }, { 0xff48, 0x20b3, 0x5dd0, 0x022a }, { 0xff56, 0x1f0b, 0x5ef5, 0x029f },
    { 0xff64, 0x1d6c, 0x6007, 0x031c }, { 0xff71, 0x1bd7, 0x6106, 0x03a4 }, { 0xff7e, 0x1a4c, 0x61f3, 0x0435 },
    { 0xff8a, 0x18cb, 0x62cb, 0x04d1 }, { 0xff96, 0x1756, 0x638f, 0x0577 }, { 0xffa1, 0x15eb, 0x643f, 0x0628 },
    { 0xffac, 0x148c, 0x64d9, 0x06e4 }, { 0xffb6, 0x1338, 0x655e, 0x07ab }, { 0xffbf, 0x11f0, 0x65cd, 0x087d },
    { 0xffc8, 0x10b4, 0x6626, 0x095a }, { 0xffd0, 0x0f83, 0x6669, 0x0a44 }, { 0xffd8, 0x0e5f, 0x6696, 0x0b39 },
    { 0xffdf, 0x0d46, 0x66ad, 0x0c39 }
};

static void aMixImplSSE2(uint16_t count, int16_t gain, uint16_t in_addr, uint16_t out_addr);
static void aMixImplNEON(uint16_t count, int16_t gain, uint16_t in_addr, uint16_t out_addr);

static inline int16_t clamp16(int32_t v) {
    if (v < -0x8000) {
        return -0x8000;
    } else if (v > 0x7fff) {
        return 0x7fff;
    }
    return (int16_t)v;
}

static inline int32_t clamp32(int64_t v) {
    if (v < -0x7fffffff - 1) {
        return -0x7fffffff - 1;
    } else if (v > 0x7fffffff) {
        return 0x7fffffff;
    }
    return (int32_t)v;
}

void aClearBufferImpl(uint16_t addr, int nbytes) {
    nbytes = ROUND_UP_16(nbytes);
    memset(BUF_U8(addr), 0, nbytes);
}

void aLoadBufferImpl(const void* source_addr, uint16_t dest_addr, uint16_t nbytes) {
#if __SANITIZE_ADDRESS__
    for (size_t i = 0; i < ROUND_DOWN_16(nbytes); i++) {
        BUF_U8(dest_addr)[i] = ((const unsigned char*)source_addr)[i];
    }
#else
    memcpy(BUF_U8(dest_addr), source_addr, ROUND_DOWN_16(nbytes));
#endif
}

#include <opus/opus.h>
#include <opusfile.h>

void aOPUSdecImpl(void* source_addr, uint16_t dest_addr, uint16_t nbytes, struct OggOpusFile** decState, int32_t pos,
                  uint32_t size) {
    int readSamples = 0;
    if (*decState == NULL) {
        *decState = op_open_memory(source_addr, size, NULL);
    }
    op_pcm_seek(*decState, pos);
    int ret = op_read(*decState, BUF_S16(dest_addr), nbytes / 2, NULL);
    if (ret < 0) {
        return;
    }
    readSamples += ret;
    while (readSamples < nbytes / 2) {
        ret = op_read(*decState, BUF_S16(dest_addr + readSamples * 2), (nbytes - readSamples * 2) / 2, NULL);
        if (ret == 0)
            break;
        readSamples += ret;
    }
}

void aOPUSFree(struct OggOpusFile* opusFile) {
    op_free(opusFile);
}

void aSaveBufferImpl(uint16_t source_addr, int16_t* dest_addr, uint16_t nbytes) {
    memcpy(dest_addr, BUF_S16(source_addr), ROUND_DOWN_16(nbytes));
}

void aLoadADPCMImpl(int num_entries_times_16, const int16_t* book_source_addr) {
    memcpy(rspa.adpcm_table, book_source_addr, num_entries_times_16);
}

void aSetBufferImpl(uint8_t flags, uint16_t in, uint16_t out, uint16_t nbytes) {
    rspa.in = in;
    rspa.out = out;
    rspa.nbytes = nbytes;
}

void aInterleaveImpl(uint16_t dest, uint16_t left, uint16_t right, uint16_t c) {
    int count = ROUND_UP_8(c) / sizeof(int16_t) / 4;
    int16_t* l = BUF_S16(left);
    int16_t* r = BUF_S16(right);
    int16_t* d = BUF_S16(dest);
    while (count > 0) {
        int16_t l0 = *l++;
        int16_t l1 = *l++;
        int16_t l2 = *l++;
        int16_t l3 = *l++;
        int16_t r0 = *r++;
        int16_t r1 = *r++;
        int16_t r2 = *r++;
        int16_t r3 = *r++;
        *d++ = l0;
        *d++ = r0;
        *d++ = l1;
        *d++ = r1;
        *d++ = l2;
        *d++ = r2;
        *d++ = l3;
        *d++ = r3;
        --count;
    }
}

void aDMEMMoveImpl(uint16_t in_addr, uint16_t out_addr, int nbytes) {
    nbytes = ROUND_UP_16(nbytes);
    memmove(BUF_U8(out_addr), BUF_U8(in_addr), nbytes);
}

void aSetLoopImpl(ADPCM_STATE* adpcm_loop_state) {
    rspa.adpcm_loop_state = adpcm_loop_state;
}

void aADPCMdecImpl(uint8_t flags, ADPCM_STATE state) {
    uint8_t* in = BUF_U8(rspa.in);
    int16_t* out = BUF_S16(rspa.out);
    int nbytes = ROUND_UP_32(rspa.nbytes);
    if (flags & A_INIT) {
        memset(out, 0, 16 * sizeof(int16_t));
    } else if (flags & A_LOOP) {
        memcpy(out, rspa.adpcm_loop_state, 16 * sizeof(int16_t));
    } else {
        memcpy(out, state, 16 * sizeof(int16_t));
    }
    out += 16;

    while (nbytes > 0) {
        int shift = *in >> 4;          // should be in 0..12 or 0..14
        int table_index = *in++ & 0xf; // should be in 0..7
        int16_t(*tbl)[8] = rspa.adpcm_table[table_index];
        int i;

        for (i = 0; i < 2; i++) {
            int16_t ins[8];
            int16_t prev1 = out[-1];
            int16_t prev2 = out[-2];
            int j, k;
            if (flags & 4) {
                for (j = 0; j < 2; j++) {
                    ins[j * 4] = (((*in >> 6) << 30) >> 30) << shift;
                    ins[j * 4 + 1] = ((((*in >> 4) & 0x3) << 30) >> 30) << shift;
                    ins[j * 4 + 2] = ((((*in >> 2) & 0x3) << 30) >> 30) << shift;
                    ins[j * 4 + 3] = (((*in++ & 0x3) << 30) >> 30) << shift;
                }
            } else {
                for (j = 0; j < 4; j++) {
                    ins[j * 2] = (((*in >> 4) << 28) >> 28) << shift;
                    ins[j * 2 + 1] = (((*in++ & 0xf) << 28) >> 28) << shift;
                }
            }
            for (j = 0; j < 8; j++) {
                int32_t acc = tbl[0][j] * prev2 + tbl[1][j] * prev1 + (ins[j] << 11);
                for (k = 0; k < j; k++) {
                    acc += tbl[1][((j - k) - 1)] * ins[k];
                }
                acc >>= 11;
                *out++ = clamp16(acc);
            }
        }
        nbytes -= 16 * sizeof(int16_t);
    }
    memcpy(state, out - 16, 16 * sizeof(int16_t));
}

void aResampleImpl(uint8_t flags, uint16_t pitch, RESAMPLE_STATE state) {
    int16_t tmp[16];
    int16_t* in_initial = BUF_S16(rspa.in);
    int16_t* in = in_initial;
    int16_t* out = BUF_S16(rspa.out);
    int nbytes = ROUND_UP_16(rspa.nbytes);
    uint32_t pitch_accumulator;
    int i;
    int16_t* tbl;
    int32_t sample;

    if (flags & A_INIT) {
        memset(tmp, 0, 5 * sizeof(int16_t));
    } else {
        memcpy(tmp, state, 16 * sizeof(int16_t));
    }
    if (flags & 2) {
        memcpy(in - 8, tmp + 8, 8 * sizeof(int16_t));
        in -= tmp[5] / sizeof(int16_t);
    }
    in -= 4;
    pitch_accumulator = (uint16_t)tmp[4];
    memcpy(in, tmp, 4 * sizeof(int16_t));

    do {
        for (i = 0; i < 8; i++) {
            tbl = resample_table[pitch_accumulator * 64 >> 16];
            sample = ((in[0] * tbl[0] + 0x4000) >> 15) + ((in[1] * tbl[1] + 0x4000) >> 15) +
                     ((in[2] * tbl[2] + 0x4000) >> 15) + ((in[3] * tbl[3] + 0x4000) >> 15);
            *out++ = clamp16(sample);

            pitch_accumulator += (pitch << 1);
            in += pitch_accumulator >> 16;
            pitch_accumulator %= 0x10000;
        }
        nbytes -= 8 * sizeof(int16_t);
    } while (nbytes > 0);

    state[4] = (int16_t)pitch_accumulator;
    memcpy(state, in, 4 * sizeof(int16_t));
    i = (in - in_initial + 4) & 7;
    in -= i;
    if (i != 0) {
        i = -8 - i;
    }
    state[5] = i;
    memcpy(state + 8, in, 8 * sizeof(int16_t));
}

void aEnvSetup1Impl(uint8_t initial_vol_wet, uint16_t rate_wet, uint16_t rate_left, uint16_t rate_right) {
    rspa.vol_wet = (uint16_t)(initial_vol_wet << 8);
    rspa.rate_wet = rate_wet;
    rspa.rate[0] = rate_left;
    rspa.rate[1] = rate_right;
}

void aEnvSetup2Impl(uint16_t initial_vol_left, uint16_t initial_vol_right) {
    rspa.vol[0] = initial_vol_left;
    rspa.vol[1] = initial_vol_right;
}

void aEnvMixerImpl(uint16_t in_addr, uint16_t n_samples, bool swap_reverb, bool neg_3, bool neg_2, bool neg_left,
                   bool neg_right, int32_t wet_dry_addr, u32 unk) {
    int16_t* in = BUF_S16(in_addr);
    int16_t* dry[2] = { BUF_S16(((wet_dry_addr >> 24) & 0xFF) << 4), BUF_S16(((wet_dry_addr >> 16) & 0xFF) << 4) };
    int16_t* wet[2] = { BUF_S16(((wet_dry_addr >> 8) & 0xFF) << 4), BUF_S16(((wet_dry_addr)&0xFF) << 4) };
    int16_t negs[4] = { neg_left ? -1 : 0, neg_right ? -1 : 0, neg_3 ? -4 : 0, neg_2 ? -2 : 0 };
    int swapped[2] = { swap_reverb ? 1 : 0, swap_reverb ? 0 : 1 };
    int n = ROUND_UP_16(n_samples);

    uint16_t vols[2] = { rspa.vol[0], rspa.vol[1] };
    uint16_t rates[2] = { rspa.rate[0], rspa.rate[1] };
    uint16_t vol_wet = rspa.vol_wet;
    uint16_t rate_wet = rspa.rate_wet;

    do {
        for (int i = 0; i < 8; i++) {
            int16_t samples[2] = { *in, *in };
            in++;
            for (int j = 0; j < 2; j++) {
                samples[j] = (samples[j] * vols[j] >> 16) ^ negs[j];
            }
            for (int j = 0; j < 2; j++) {
                *dry[j] = clamp16(*dry[j] + samples[j]);
                dry[j]++;
                *wet[j] = clamp16(*wet[j] + ((samples[swapped[j]] * vol_wet >> 16) ^ negs[2 + j]));
                wet[j]++;
            }
        }
        vols[0] += rates[0];
        vols[1] += rates[1];
        vol_wet += rate_wet;

        n -= 8;
    } while (n > 0);
}

static void aMixImplRef(uint16_t count, int16_t gain, uint16_t in_addr, uint16_t out_addr) {
    int nbytes = ROUND_UP_32(ROUND_DOWN_16(count << 4));
    int16_t* in = BUF_S16(in_addr);
    int16_t* out = BUF_S16(out_addr);
    int i;
    int32_t sample;

    if (gain == -0x8000) {
        while (nbytes > 0) {
            for (i = 0; i < 16; i++) {
                sample = *out - *in++;
                *out++ = clamp16(sample);
            }
            nbytes -= 16 * sizeof(int16_t);
        }
    }

    while (nbytes > 0) {
        for (i = 0; i < 16; i++) {
            sample = ((*out * 0x7fff + *in++ * gain) + 0x4000) >> 15;
            *out++ = clamp16(sample);
        }

        nbytes -= 16 * sizeof(int16_t);
    }
}

void aMixImpl(uint16_t count, int16_t gain, uint16_t in_addr, uint16_t out_addr) {
#if defined(__SSE2__) || defined(_M_AMD64)
    aMixImplSSE2(count, gain, in_addr, out_addr);
#elif defined(__ARM_NEON)
    aMixImplNEON(count, gain, in_addr, out_addr);
#else
    aMixImplRef(count, gain, in_addr, out_addr);
#endif
}

void aS8DecImpl(uint8_t flags, ADPCM_STATE state) {
    uint8_t* in = BUF_U8(rspa.in);
    int16_t* out = BUF_S16(rspa.out);
    int nbytes = ROUND_UP_32(rspa.nbytes);
    if (flags & A_INIT) {
        memset(out, 0, 16 * sizeof(int16_t));
    } else if (flags & A_LOOP) {
        memcpy(out, rspa.adpcm_loop_state, 16 * sizeof(int16_t));
    } else {
        memcpy(out, state, 16 * sizeof(int16_t));
    }
    out += 16;

    while (nbytes > 0) {
        *out++ = (int16_t)(*in++ << 8);
        *out++ = (int16_t)(*in++ << 8);
        *out++ = (int16_t)(*in++ << 8);
        *out++ = (int16_t)(*in++ << 8);
        *out++ = (int16_t)(*in++ << 8);
        *out++ = (int16_t)(*in++ << 8);
        *out++ = (int16_t)(*in++ << 8);
        *out++ = (int16_t)(*in++ << 8);
        *out++ = (int16_t)(*in++ << 8);
        *out++ = (int16_t)(*in++ << 8);
        *out++ = (int16_t)(*in++ << 8);
        *out++ = (int16_t)(*in++ << 8);
        *out++ = (int16_t)(*in++ << 8);
        *out++ = (int16_t)(*in++ << 8);
        *out++ = (int16_t)(*in++ << 8);
        *out++ = (int16_t)(*in++ << 8);

        nbytes -= 16 * sizeof(int16_t);
    }

    memcpy(state, out - 16, 16 * sizeof(int16_t));
}

void aAddMixerImpl(uint16_t count, uint16_t in_addr, uint16_t out_addr) {
    int16_t* in = BUF_S16(in_addr);
    int16_t* out = BUF_S16(out_addr);
    int nbytes = ROUND_UP_64(ROUND_DOWN_16(count));

    do {
        *out = clamp16(*out + *in++);
        out++;
        *out = clamp16(*out + *in++);
        out++;
        *out = clamp16(*out + *in++);
        out++;
        *out = clamp16(*out + *in++);
        out++;
        *out = clamp16(*out + *in++);
        out++;
        *out = clamp16(*out + *in++);
        out++;
        *out = clamp16(*out + *in++);
        out++;
        *out = clamp16(*out + *in++);
        out++;
        *out = clamp16(*out + *in++);
        out++;
        *out = clamp16(*out + *in++);
        out++;
        *out = clamp16(*out + *in++);
        out++;
        *out = clamp16(*out + *in++);
        out++;
        *out = clamp16(*out + *in++);
        out++;
        *out = clamp16(*out + *in++);
        out++;
        *out = clamp16(*out + *in++);
        out++;
        *out = clamp16(*out + *in++);
        out++;

        nbytes -= 16 * sizeof(int16_t);
    } while (nbytes > 0);
}

void aDuplicateImpl(uint16_t count, uint16_t in_addr, uint16_t out_addr) {
    uint8_t* in = BUF_U8(in_addr);
    uint8_t* out = BUF_U8(out_addr);

    uint8_t tmp[128];
    memcpy(tmp, in, 128);
    do {
        memcpy(out, tmp, 128);
        out += 128;
    } while (count-- > 0);
}

void aResampleZohImpl(uint16_t pitch, uint16_t start_fract) {
    int16_t* in = BUF_S16(rspa.in);
    int16_t* out = BUF_S16(rspa.out);
    int nbytes = ROUND_UP_8(rspa.nbytes);
    uint32_t pos = start_fract;
    uint32_t pitch_add = pitch << 2;

    do {
        *out++ = in[pos >> 17];
        pos += pitch_add;
        *out++ = in[pos >> 17];
        pos += pitch_add;
        *out++ = in[pos >> 17];
        pos += pitch_add;
        *out++ = in[pos >> 17];
        pos += pitch_add;

        nbytes -= 4 * sizeof(int16_t);
    } while (nbytes > 0);
}

void aInterlImpl(uint16_t in_addr, uint16_t out_addr, uint16_t n_samples) {
    int16_t* in = BUF_S16(in_addr);
    int16_t* out = BUF_S16(out_addr);
    int n = ROUND_UP_8(n_samples);

    do {
        *out++ = *in++;
        in++;
        *out++ = *in++;
        in++;
        *out++ = *in++;
        in++;
        *out++ = *in++;
        in++;
        *out++ = *in++;
        in++;
        *out++ = *in++;
        in++;
        *out++ = *in++;
        in++;
        *out++ = *in++;
        in++;

        n -= 8;
    } while (n > 0);
}

void aFilterImpl(uint8_t flags, uint16_t count_or_buf, int16_t* state_or_filter) {
    if (flags > A_INIT) {
        rspa.filter_count = ROUND_UP_16(count_or_buf);
        memcpy(rspa.filter, state_or_filter, sizeof(rspa.filter));
    } else {
        int16_t tmp[16], tmp2[8];
        int count = rspa.filter_count;
        int16_t* buf = BUF_S16(count_or_buf);

        if (flags == A_INIT) {
#ifndef __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmemset-elt-size"
#endif
            memset(tmp, 0, 8 * sizeof(int16_t));
#ifndef __clang__
#pragma GCC diagnostic pop
#endif
            memset(tmp2, 0, 8 * sizeof(int16_t));
        } else {
            memcpy(tmp, state_or_filter, 8 * sizeof(int16_t));
            memcpy(tmp2, state_or_filter + 8, 8 * sizeof(int16_t));
        }

        for (int i = 0; i < 8; i++) {
            rspa.filter[i] = (tmp2[i] + rspa.filter[i]) / 2;
        }

        do {
            memcpy(tmp + 8, buf, 8 * sizeof(int16_t));
            for (int i = 0; i < 8; i++) {
                int64_t sample = 0x4000; // round term
                for (int j = 0; j < 8; j++) {
                    sample += tmp[i + j] * rspa.filter[7 - j];
                }
                buf[i] = clamp16((int32_t)(sample >> 15));
            }
            memcpy(tmp, tmp + 8, 8 * sizeof(int16_t));

            buf += 8;
            count -= 8 * sizeof(int16_t);
        } while (count > 0);

        memcpy(state_or_filter, tmp, 8 * sizeof(int16_t));
        memcpy(state_or_filter + 8, rspa.filter, 8 * sizeof(int16_t));
    }
}

void aHiLoGainImpl(uint8_t g, uint16_t count, uint16_t addr) {
    int16_t* samples = BUF_S16(addr);
    int nbytes = ROUND_UP_32(count);

    do {
        *samples = clamp16((*samples * g) >> 4);
        samples++;
        *samples = clamp16((*samples * g) >> 4);
        samples++;
        *samples = clamp16((*samples * g) >> 4);
        samples++;
        *samples = clamp16((*samples * g) >> 4);
        samples++;
        *samples = clamp16((*samples * g) >> 4);
        samples++;
        *samples = clamp16((*samples * g) >> 4);
        samples++;
        *samples = clamp16((*samples * g) >> 4);
        samples++;
        *samples = clamp16((*samples * g) >> 4);
        samples++;

        nbytes -= 8;
    } while (nbytes > 0);
}

void aUnkCmd3Impl(uint16_t a, uint16_t b, uint16_t c) {
}

void aUnkCmd19Impl(uint8_t f, uint16_t count, uint16_t out_addr, uint16_t in_addr) {
    int nbytes = ROUND_UP_64(count);
    int16_t* in = BUF_S16(in_addr + f);
    int16_t* out = BUF_S16(out_addr);
    int16_t tbl[32];

    memcpy(tbl, in, 32 * sizeof(int16_t));
    do {
        for (int i = 0; i < 32; i++) {
            out[i] = clamp16(out[i] * tbl[i]);
        }
        out += 32;
        nbytes -= 32 * sizeof(int16_t);
    } while (nbytes > 0);
}

// From here on there are SIMD implementations of the various mixer functions.
// A note about FORCE_OPTIMIZE...
// Compilers don't handle SIMD code well when not optimizing. It is unlikely that this code will need to be debugged
// outside of specific audio issues. We can assume it should always be optimized.

// SIMD operations expect aligned data
#include "align_asset_macro.h"

#if defined(__SSE2__) || defined(_M_AMD64)
#include <immintrin.h>

static const ALIGN_ASSET(16) int16_t x7fff[8] = {
    0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF,
};
static const ALIGN_ASSET(16) int32_t x4000[4] = {
    0x4000,
    0x4000,
    0x4000,
    0x4000,
};

static void aMixImplSSE2(uint16_t count, int16_t gain, uint16_t in_addr, uint16_t out_addr) {
    int nbytes = ROUND_UP_32(ROUND_DOWN_16(count << 4));
    int16_t* in = BUF_S16(in_addr);
    int16_t* out = BUF_S16(out_addr);
    int i;
    int32_t sample;
    if (gain == -0x8000) {
        while (nbytes > 0) {
            for (unsigned int i = 0; i < 2; i++) {
                __m128i outVec = _mm_loadu_si128((__m128i*)out);
                __m128i inVec = _mm_loadu_si128((__m128i*)in);
                __m128i subsVec = _mm_subs_epi16(outVec, inVec);
                _mm_storeu_si128(out, subsVec);
                nbytes -= 8 * sizeof(int16_t);
                in += 8;
                out += 8;
            }
        }
    }
    // Load constants into vectors from aligned memory.
    __m128i x7fffVec = _mm_load_si128((__m128i*)x7fff);
    __m128i x4000Vec = _mm_load_si128((__m128i*)x4000);
    __m128i gainVec = _mm_set1_epi16(gain);
    while (nbytes > 0) {
        for (unsigned int i = 0; i < 2; i++) {
            // Load input and output data into vectors
            __m128i outVec = _mm_loadu_si128((__m128i*)out);
            __m128i inVec = _mm_loadu_si128((__m128i*)in);
            // Multiply `out` by `0x7FFF` producing 32 bit results, and store the upper and lower bits in each vector.
            // Equivalent to `out[0..8] * 0x7FFF`
            __m128i outx7fffLoVec = _mm_mullo_epi16(outVec, x7fffVec);
            __m128i outx7fffHiVec = _mm_mulhi_epi16(outVec, x7fffVec);
            // Same as above but for in and gain. Equivalent to `in[0..8] * gain`
            __m128i inxGainLoVec = _mm_mullo_epi16(inVec, gainVec);
            __m128i inxGainHiVec = _mm_mulhi_epi16(inVec, gainVec);

            // Interleave the lo and hi bits into one 32 bit value for each vector element.
            // So now we have 4 full elements in each vector instead of 8 half elements.
            outx7fffLoVec = _mm_unpacklo_epi16(outx7fffLoVec, outx7fffHiVec);
            outx7fffHiVec = _mm_unpackhi_epi16(outx7fffLoVec, outx7fffHiVec);
            inxGainLoVec = _mm_unpacklo_epi16(inxGainLoVec, inxGainHiVec);
            inxGainHiVec = _mm_unpackhi_epi16(inxGainLoVec, inxGainHiVec);

            // Now we have 4 32 bit elements.  Continue the calculaton per the reference implementation.
            // We already did out + 0x7fff and in * gain.
            // *out * 0x7fff + *in++ * gain is the final result of these two calculations.
            __m128i addLoVec = _mm_add_epi32(outx7fffLoVec, inxGainLoVec);
            __m128i addHiVec = _mm_add_epi32(outx7fffHiVec, inxGainHiVec);
            // Add 0x4000 to each element
            addLoVec = _mm_add_epi32(addLoVec, x4000Vec);
            addHiVec = _mm_add_epi32(addHiVec, x4000Vec);
            // Shift each element over by 15
            __m128i shiftedLoVec = _mm_srai_epi32(addLoVec, 15);
            __m128i shiftedHiVec = _mm_srai_epi32(addHiVec, 15);
            // Convert each 32 bit element to 16 bit with saturation (clamp) and store in `outVec`
            outVec = _mm_packs_epi32(shiftedLoVec, shiftedHiVec);
            // Write the final vector back to memory
            // The final calculation is ((out[0..8] * 0x7fff + in[0..8] * gain) + 0x4000) >> 15;
            _mm_storeu_si128((__m128i*)out, outVec);

            in += 8;
            out += 8;
            nbytes -= 8 * sizeof(int16_t);
        }
    }
}
#endif
#if defined(__ARM_NEON)
#include <arm_neon.h>
static const int32_t x4000Arr[4] = { 0x4000, 0x4000, 0x4000, 0x4000 };
void aMixImplNEON(uint16_t count, int16_t gain, uint16_t in_addr, uint16_t out_addr) {
    int nbytes = ROUND_UP_32(ROUND_DOWN_16(count << 4));
    int16_t* in = BUF_S16(in_addr);
    int16_t* out = BUF_S16(out_addr);
    int i;
    int32_t sample;

    if (gain == -0x8000) {
        while (nbytes > 0) {
            for (unsigned int i = 0; i < 2; i++) {
                int16x8_t outVec = vld1q_s16(out);
                int16x8_t inVec = vld1q_s16(in);
                int16x8_t subVec = vqsubq_s16(outVec, inVec);
                vst1q_s16(out, subVec);
                nbytes -= 8 * sizeof(int16_t);
                out += 8;
                in += 8;
            }
        }
    }
    int16x8_t gainVec = vdupq_n_s16(gain);
    int32x4_t x4000Vec = vld1q_s32(x4000Arr);
    while (nbytes > 0) {
        for (unsigned int i = 0; i < 2; i++) {
            // for (i = 0; i < 16; i++) {
            int16x8_t outVec = vld1q_s16(out);
            int16x8_t inVec = vld1q_s16(in);
            int16x4_t outLoVec = vget_low_s16(outVec);
            int16x8_t outLoVec2 = vcombine_s16(outLoVec, outLoVec);
            int16x4_t inLoVec = vget_low_s16(inVec);
            int16x8_t inLoVec2 = vcombine_s16(inLoVec, inLoVec);
            int32x4_t outX7fffHiVec = vmull_high_n_s16(outVec, 0x7FFF);
            int32x4_t outX7fffLoVec = vmull_high_n_s16(outLoVec2, 0x7FFF);

            int32x4_t inGainLoVec = vmull_high_s16(inLoVec2, gainVec);
            int32x4_t inGainHiVec = vmull_high_s16(inVec, gainVec);
            int32x4_t addVecLo = vaddq_s32(outX7fffLoVec, inGainLoVec);
            int32x4_t addVecHi = vaddq_s32(outX7fffHiVec, inGainHiVec);
            addVecHi = vaddq_s32(addVecHi, x4000Vec);
            addVecLo = vaddq_s32(addVecLo, x4000Vec);
            int32x4_t shiftVecHi = vshrq_n_s32(addVecHi, 15);
            int32x4_t shiftVecLo = vshrq_n_s32(addVecLo, 15);
            int16x4_t shiftedNarrowHiVec = vqmovn_s32(shiftVecHi);
            int16x4_t shiftedNarrowLoVec = vqmovn_s32(shiftVecLo);
            vst1_s16(out, shiftedNarrowLoVec);
            out += 4;
            vst1_s16(out, shiftedNarrowHiVec);
            // int16x8_t finalVec = vcombine_s16(shiftedNarrowLoVec, shiftedNarrowHiVec);
            // vst1q_s16(out, finalVec);
            out += 4;
            in += 8;

            nbytes -= 8 * sizeof(int16_t);
        }
    }
}
#endif

#if 0
static const ALIGN_ASSET(32) int16_t x7fff[16] = { 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF,};
static const ALIGN_ASSET(32) int32_t x4000[8] = { 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000};

#pragma GCC target("avx2")
// AVX2 version of the SSE2 implementation above. AVX2 wasn't released until 2014 and I don't have a good way of checking for it at compile time.
void aMixImpl256(uint16_t count, int16_t gain, uint16_t in_addr, uint16_t out_addr) {
    int nbytes = ROUND_UP_32(ROUND_DOWN_16(count << 4));
    int16_t* in = BUF_S16(in_addr);
    int16_t* out = BUF_S16(out_addr);
    int i;
    int32_t sample;
    if (gain == -0x8000) {
        while (nbytes > 0) {
            __m256i outVec =_mm256_loadu_si256((__m256*)out);
            __m256i inVec =_mm256_loadu_si256((__m256i*)in);
            __m256i subsVec =_mm256_subs_epi16(outVec, inVec);
            _mm256_storeu_si256(out, subsVec);
            in += 16;
            out += 16;
            nbytes -= 16 * sizeof(int16_t);
        }
    }
    // Load constants into vectors from aligned memory.
    __m256i x7fffVec = _mm256_load_si256((__m256i*)x7fff);
    __m256i x4000Vec = _mm256_load_si256((__m256i*)x4000);
    __m256i gainVec = _mm256_set1_epi16(gain);
    while (nbytes > 0) {
        // Load input and output data into vectors
        __m256i outVec = _mm256_loadu_si256((__m256i*)out);
        __m256i inVec = _mm256_loadu_si256((__m256i*)in);
        // Multiply `out` by `0x7FFF` producing 32 bit results, and store the upper and lower bits in each vector.
        // Equivalent to `out[0..16] * 0x7FFF`
        __m256i outx7fffLoVec = _mm256_mullo_epi16(outVec, x7fffVec);
        __m256i outx7fffHiVec = _mm256_mulhi_epi16(outVec, x7fffVec);
        // Same as above but for in and gain. Equivalent to `in[0..16] * gain`
        __m256i inxGainLoVec = _mm256_mullo_epi16(inVec, gainVec);
        __m256i inxGainHiVec = _mm256_mulhi_epi16(inVec, gainVec);

        // Interleave the lo and hi bits into one 32 bit value for each vector element.
        // So now we have 8 full elements in each vector instead of 16 half elements.
        outx7fffLoVec = _mm256_unpacklo_epi16(outx7fffLoVec, outx7fffHiVec);
        outx7fffHiVec = _mm256_unpackhi_epi16(outx7fffLoVec, outx7fffHiVec);
        inxGainLoVec = _mm256_unpacklo_epi16(inxGainLoVec, inxGainHiVec);
        inxGainHiVec = _mm256_unpackhi_epi16(inxGainLoVec, inxGainHiVec);

        // Now we have 8 32 bit elements.  Continue the calculaton per the reference implementation.
        // We already did out + 0x7fff and in * gain.
        // *out * 0x7fff + *in++ * gain is the final result of these two calculations.
        __m256i addLoVec = _mm256_add_epi32(outx7fffLoVec, inxGainLoVec);
        __m256i addHiVec = _mm256_add_epi32(outx7fffHiVec, inxGainHiVec);
        // Add 0x4000 to each element
        addLoVec = _mm256_add_epi32(addLoVec, x4000Vec);
        addHiVec = _mm256_add_epi32(addHiVec, x4000Vec);
        // Shift each element over by 15
        __m256i shiftedLoVec = _mm256_srai_epi32(addLoVec, 15);
        __m256i shiftedHiVec = _mm256_srai_epi32(addHiVec, 15);
        // Convert each 32 bit element to 16 bit with saturation (clamp) and store in `outVec`
        outVec = _mm256_packs_epi32(shiftedLoVec, shiftedHiVec);
        // Write the final vector back to memory
        // The final calculation is ((out[0..16] * 0x7fff + in[0..16] * gain) + 0x4000) >> 15;
        _mm256_storeu_si256((__m256i*)out, outVec);

        in += 16;
        out += 16;
        nbytes -= 16 * sizeof(int16_t);
    }
}
#endif
