#include "AudioDecoder.h"
#include "z64audio.h"
#include "dr_libs/wav.h"
#include <stdexcept>
#define WAV_DECODE_CHUNK_SIZE 64
//A handful of definitions need to be copied from mixer.c.
#define ROUND_UP_32(v) (((v) + 31) & ~31)
//The below is copied verbatim from mixer.c.
static inline int16_t clamp16(int32_t v) {
    if (v < -0x8000) {
        return -0x8000;
    } else if (v > 0x7fff) {
        return 0x7fff;
    }
}

    AudioDecoder::AudioDecoder() {
    sample = NULL;
        prev1 = 0;
        prev2 = 0;

    }
AudioDecoder::~AudioDecoder()
{

}
void AudioDecoder::setSample(LUS::AudioSample* sample)
{
        this->sample = sample;
        if (sample->book.book == nullptr)
        memset(adpcm_table, 0, 8 * 2 * 8 * 2);
        else
        memcpy(adpcm_table, sample->sample.book->book,
16 * sample->sample.book->order * sample->sample.book->npredictors);
        prev1 = 0;
        prev2 = 0;
        in = sample->sample.sampleAddr;
        inStart = in;
        inEnd = in + sample->sample.size;
}
size_t AudioDecoder::decode(int16_t* out, size_t nSamples) {
        size_t samplesOut = 0;
        size_t nbytes = nSamples * 2;
        // Prevent the decoder from using more output bytes than declared to be available by the callee.
        nbytes -= 31;
        nbytes = ROUND_UP_32(nbytes);
        while (nbytes > 0 && in < inEnd) {
        int shift = *in >> 4;          // should be in 0..12 or 0..14
        int table_index = *in++ & 0xf; // should be in 0..7
        int16_t(*tbl)[8] = adpcm_table[table_index];
        int i;

        for (i = 0; i < 2; i++) {
            int16_t ins[8];
            int j, k;
            if (sample->sample.codec == CODEC_SMALL_ADPCM) {
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
                samplesOut++;

            }
            prev1 = out[-1];
            prev2 = out[-2];
        }
        nbytes -= 16 * sizeof(int16_t);
        }
        return samplesOut;

}

size_t AudioDecoder::decodeToWav    (LUS::AudioSample* sample, int16_t** buffer)
{
        int16_t* wavOut = nullptr;
        setSample(sample);

        drwav_data_format format;
        format.bitsPerSample = 16;
        format.channels = 1;
        format.container = drwav_container_riff;
        format.format = DR_WAVE_FORMAT_PCM;
//Todo: figure out how to really determine the sample rate. CODEC_ADPCM tends to stream at higher rates (usually 20KHZ) while CODEC_SMALL_ADPCM is usually around 14000. They're still not consistent though.
        if (sample->sample.codec == CODEC_ADPCM)
        format.sampleRate = 20000;
        else if (sample->sample.codec = CODEC_SMALL_ADPCM)
        format.sampleRate = 14000;
        else
        throw std::runtime_error("AudioDecoder: Unsupported codec.");
        drwav wav;
        size_t wavSize;
        if (!drwav_init_memory_write(&wav, (void**) &wavOut, &wavSize, &format, nullptr))
        throw std::runtime_error("AudioDecoder: Unable to initialize wave writer.");
        int16_t chunk[WAV_DECODE_CHUNK_SIZE];
        //Don't decode past the end of the loop.
        size_t samplesLeft = sample->sample.loop->end;
        while (samplesLeft > 0) {
        size_t samplesRead = decode(chunk, WAV_DECODE_CHUNK_SIZE);
        if (samplesRead > samplesLeft)
            samplesRead = samplesLeft;

        if (samplesRead == 0)
            break;
        if (drwav_write_pcm_frames(&wav, samplesRead, chunk) != samplesRead) {
            drwav_uninit(&wav);
            drwav_free(wavOut, nullptr);
            throw std::runtime_error("AudioDecoder: Unable to write wave data.");
        }
        samplesLeft -= samplesRead;

        }
        drwav_uninit(&wav);
        *buffer = wavOut;
    return wavSize;

}
