#if defined(__linux__) || defined(__BSD__)

#include "PulseAudioPlayer.h"
#include <spdlog/spdlog.h>

namespace Ship
{
    static void pas_context_state_cb(pa_context *c, void *userdata) {
        switch (pa_context_get_state(c)) {
            case PA_CONTEXT_READY:
            case PA_CONTEXT_TERMINATED:
            case PA_CONTEXT_FAILED:
                *(bool*)userdata = true;
                break;
            default:
                break;
        }
    }

    static void pas_stream_state_cb(pa_stream *s, void *userdata) {
        switch (pa_stream_get_state(s)) {
            case PA_STREAM_READY:
            case PA_STREAM_FAILED:
            case PA_STREAM_TERMINATED:
                *(bool*)userdata = true;
                break;
            default:
                break;
        }
    }

    static void pas_stream_write_cb(pa_stream* s, size_t length, void* userdata) {
    }

    static void pas_update_complete(pa_stream* stream, int success, void* userdata) {
        *(bool*)userdata = true;
    }

    static void pas_write_complete(void* userdata) {
        *(bool*)userdata = true;
    }

    bool PulseAudioPlayer::Init()
    {
        bool done = false;
        const pa_buffer_attr* applied_attr = nullptr;

        // Create mainloop
        m_MainLoop = pa_mainloop_new();
        if (m_MainLoop == NULL) {
            return false;
        }

        // Create context and connect
        m_Context = pa_context_new(pa_mainloop_get_api(m_MainLoop), "Ocarina of Time");
        if (m_Context == NULL) {
            goto fail;
        }

        pa_context_set_state_callback(m_Context, pas_context_state_cb, &done);

        if (pa_context_connect(m_Context, NULL, PA_CONTEXT_NOFLAGS, NULL) < 0) {
            goto fail;
        }

        while (!done) {
            pa_mainloop_iterate(m_MainLoop, true, NULL);
        }
        pa_context_set_state_callback(m_Context, NULL, NULL);
        if (pa_context_get_state(m_Context) != PA_CONTEXT_READY) {
            goto fail;
        }

        // Create stream
        pa_sample_spec ss;
        ss.format = PA_SAMPLE_S16LE;
        ss.rate = 32000;
        ss.channels = 2;

        pa_buffer_attr attr;
        attr.maxlength = (1600 + 544 + 528 + 1600) * 4;
        attr.tlength = (528*2 + 544) * 4;
        attr.prebuf = 1500 * 4;
        attr.minreq = 161 * 4;
        attr.fragsize = (uint32_t)-1;

        m_Stream = pa_stream_new(m_Context, "zelda", &ss, NULL);
        if (m_Stream == NULL) {
            goto fail;
        }

        done = false;
        pa_stream_set_state_callback(m_Stream, pas_stream_state_cb, &done);
        pa_stream_set_write_callback(m_Stream, pas_stream_write_cb, NULL);
        if (pa_stream_connect_playback(m_Stream, NULL, &attr, PA_STREAM_ADJUST_LATENCY, NULL, NULL) < 0) {
            goto fail;
        }

        while (!done) {
            pa_mainloop_iterate(m_MainLoop, true, NULL);
        }
        pa_stream_set_state_callback(m_Stream, NULL, NULL);
        if (pa_stream_get_state(m_Stream) != PA_STREAM_READY) {
            goto fail;
        }

        applied_attr = pa_stream_get_buffer_attr(m_Stream);
        SPDLOG_TRACE("maxlength: {}\ntlength: {}\nprebuf: {}\nminreq: {}\nfragsize: {}\n",
            applied_attr->maxlength, applied_attr->tlength, applied_attr->prebuf, applied_attr->minreq, applied_attr->fragsize);
        m_Attr = *applied_attr;

        return true;

    fail:
        if (m_Stream != NULL) {
            pa_stream_unref(m_Stream);
            m_Stream = NULL;
        }
        if (m_Context != NULL) {
            pa_context_disconnect(m_Context);
            pa_context_unref(m_Context);
            m_Context = NULL;
        }
        if (m_MainLoop != NULL) {
            pa_mainloop_free(m_MainLoop);
            m_MainLoop = NULL;
        }
        return false;
    }

    int PulseAudioPlayer::Buffered()
    {
        if (m_Stream == NULL) {
            return 0;
        }

        bool done = false;
        pa_stream_update_timing_info(m_Stream, pas_update_complete, &done);
        while (!done) {
            pa_mainloop_iterate(m_MainLoop, true, NULL);
        }

        const pa_timing_info *info = pa_stream_get_timing_info(m_Stream);
        if (info == NULL) {
            SPDLOG_ERROR("pa_stream_get_timing_info failed, state is %d\n", pa_stream_get_state(m_Stream));
        }
        return (info->write_index - info->read_index) / 4;
    }

    int PulseAudioPlayer::GetDesiredBuffered()
    {
        // return 1100;
        return 1680;
    }

    void PulseAudioPlayer::Play(const uint8_t* buff, uint32_t len)
    {
        size_t ws = m_Attr.maxlength - Buffered() * 4;
        if (ws < len) {
            len = ws;
        }
        if (pa_stream_write_ext_free(m_Stream, buff, len, pas_write_complete, &m_WriteComplete, 0LL, PA_SEEK_RELATIVE) < 0) {
            SPDLOG_ERROR("pa_stream_write failed");
            return;
        }
        while (!m_WriteComplete) {
            pa_mainloop_iterate(m_MainLoop, true, NULL);
        }
        m_WriteComplete = false;
    }
}

#endif