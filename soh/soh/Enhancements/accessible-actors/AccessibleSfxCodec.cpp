#include "AccessibleSfxCodec.h"

#include <string.h>
#include <atomic>
#include <algorithm>

#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisenc.h>
#include <vorbis/vorbisfile.h>

#define A11Y_SFX_ENCODE_CHUNK 1024

namespace {

void appendPage(std::vector<uint8_t>& out, const ogg_page& page) {
    out.insert(out.end(), page.header, page.header + page.header_len);
    out.insert(out.end(), page.body, page.body + page.body_len);
}

struct OggMemoryFile {
    const uint8_t* data;
    size_t pos;
    size_t size;
};

size_t oggRead(void* out, size_t size, size_t elems, void* src) {
    OggMemoryFile* file = static_cast<OggMemoryFile*>(src);
    size_t toRead = size * elems;

    if (toRead > file->size - file->pos) {
        toRead = file->size - file->pos;
    }
    memcpy(out, file->data + file->pos, toRead);
    file->pos += toRead;

    return toRead / size;
}

int oggSeek(void* src, ogg_int64_t pos, int whence) {
    OggMemoryFile* file = static_cast<OggMemoryFile*>(src);
    size_t newPos;

    switch (whence) {
        case SEEK_SET:
            newPos = pos;
            break;
        case SEEK_CUR:
            newPos = file->pos + pos;
            break;
        case SEEK_END:
            newPos = file->size + pos;
            break;
        default:
            return -1;
    }
    if (newPos > file->size) {
        return -1;
    }
    file->pos = newPos;
    return 0;
}

int oggClose([[maybe_unused]] void* src) {
    return 0;
}

long oggTell(void* src) {
    return static_cast<long>(static_cast<OggMemoryFile*>(src)->pos);
}

const ov_callbacks oggCallbacks = {
    oggRead,
    oggSeek,
    oggClose,
    oggTell,
};

} // namespace

bool AccessibleSfx_EncodeVorbis(const int16_t* mono, size_t frames, std::vector<uint8_t>& out) {
    // Ogg wants a stream serial number. Nothing multiplexes these streams, so a counter is fine.
    static std::atomic<int> nextSerial(1);

    vorbis_info vi;
    vorbis_comment vc;
    vorbis_dsp_state vd;
    vorbis_block vb;
    ogg_stream_state os;
    ogg_page page;
    ogg_packet packet;
    bool ok = false;

    if (frames == 0) {
        return false;
    }

    vorbis_info_init(&vi);
    if (vorbis_encode_init_vbr(&vi, 1, A11Y_SFX_SAMPLE_RATE, A11Y_SFX_VORBIS_QUALITY) != 0) {
        vorbis_info_clear(&vi);
        return false;
    }

    vorbis_comment_init(&vc);
    vorbis_analysis_init(&vd, &vi);
    vorbis_block_init(&vd, &vb);
    ogg_stream_init(&os, nextSerial++);

    out.clear();

    {
        ogg_packet header, headerComments, headerCode;
        vorbis_analysis_headerout(&vd, &vc, &header, &headerComments, &headerCode);
        ogg_stream_packetin(&os, &header);
        ogg_stream_packetin(&os, &headerComments);
        ogg_stream_packetin(&os, &headerCode);
        // The headers must land in their own pages, ahead of any audio.
        while (ogg_stream_flush(&os, &page) != 0) {
            appendPage(out, page);
        }
    }

    size_t pos = 0;
    while (true) {
        int chunk = static_cast<int>(std::min<size_t>(A11Y_SFX_ENCODE_CHUNK, frames - pos));
        if (chunk > 0) {
            float** buffer = vorbis_analysis_buffer(&vd, chunk);
            for (int i = 0; i < chunk; i++) {
                buffer[0][i] = mono[pos + i] / 32768.0f;
            }
            pos += chunk;
        }
        // A zero-length write signals end of stream and drains the encoder.
        if (vorbis_analysis_wrote(&vd, chunk) != 0) {
            goto cleanup;
        }

        while (vorbis_analysis_blockout(&vd, &vb) == 1) {
            if (vorbis_analysis(&vb, NULL) != 0 || vorbis_bitrate_addblock(&vb) != 0) {
                goto cleanup;
            }
            while (vorbis_bitrate_flushpacket(&vd, &packet)) {
                ogg_stream_packetin(&os, &packet);
                while (ogg_stream_pageout(&os, &page) != 0) {
                    appendPage(out, page);
                }
            }
        }

        if (chunk == 0) {
            // Push out whatever partial page the drain left behind.
            while (ogg_stream_flush(&os, &page) != 0) {
                appendPage(out, page);
            }
            break;
        }
    }
    ok = !out.empty();

cleanup:
    ogg_stream_clear(&os);
    vorbis_block_clear(&vb);
    vorbis_dsp_clear(&vd);
    vorbis_comment_clear(&vc);
    vorbis_info_clear(&vi);
    if (!ok) {
        out.clear();
    }
    return ok;
}

bool AccessibleSfx_DecodeVorbis(const uint8_t* data, size_t size, std::vector<int16_t>& out) {
    OggMemoryFile file = { data, 0, size };
    OggVorbis_File vf;

    if (ov_open_callbacks(&file, &vf, NULL, 0, oggCallbacks) != 0) {
        return false;
    }

    vorbis_info* vi = ov_info(&vf, -1);
    if (vi == NULL || vi->channels != 1) {
        ov_clear(&vf);
        return false;
    }

    out.clear();
    ogg_int64_t total = ov_pcm_total(&vf, -1);
    if (total > 0) {
        out.reserve(static_cast<size_t>(total));
    }

    char chunk[4096];
    int bitStream = 0;
    long read;
    do {
        // Signed 16 bit, native endianness, as the audio engine expects.
        read = ov_read(&vf, chunk, sizeof(chunk), 0, 2, 1, &bitStream);
        if (read < 0) {
            ov_clear(&vf);
            out.clear();
            return false;
        }
        out.insert(out.end(), reinterpret_cast<int16_t*>(chunk), reinterpret_cast<int16_t*>(chunk + read));
    } while (read != 0);

    ov_clear(&vf);
    return !out.empty();
}

AccessibleSfxDecoder::AccessibleSfxDecoder() {
    running = true;
    thread = std::thread(&AccessibleSfxDecoder::runThread, this);
}

AccessibleSfxDecoder::~AccessibleSfxDecoder() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        running = false;
    }
    cv.notify_all();
    thread.join();
}

void AccessibleSfxDecoder::enqueue(std::function<void()> job) {
    {
        std::lock_guard<std::mutex> lock(mtx);
        jobs.push_back(std::move(job));
    }
    cv.notify_one();
}

void AccessibleSfxDecoder::runThread() {
    while (true) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this] { return !running || !jobs.empty(); });
            if (!running) {
                return;
            }
            job = std::move(jobs.front());
            jobs.pop_front();
        }
        job();
    }
}
