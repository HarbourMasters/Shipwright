#ifndef AUDIO_SEQ_QUEUE_H
#define AUDIO_SEQ_QUEUE_H

#include <stdint.h>

#ifdef __cplusplus

#include <queue>
#include <mutex>
#include <condition_variable>

// A threadsafe-queue.
template <class T> class SafeQueue {
  public:
    SafeQueue(void) : q(), m(), c() {
    }

    ~SafeQueue(void) {
    }

    // Add an element to the queue.
    void enqueue(T t) {
        std::lock_guard<std::mutex> lock(m);
        q.push(t);
        c.notify_one();
    }

    // Get the "front"-element.
    // If the queue is empty, wait till a element is avaiable.
    T dequeue(void) {
        std::unique_lock<std::mutex> lock(m);
        while (q.empty()) {
            // release lock as long as the wait and reaquire it afterwards.
            c.wait(lock);
        }
        T val = q.front();
        q.pop();
        return val;
    }

    int32_t isEmpty() {
        return static_cast<int32_t>(q.empty());
    }

  private:
    std::queue<T> q;
    mutable std::mutex m;
    std::condition_variable c;
};
#endif

#ifdef __cplusplus
extern "C" {
#endif

void AudioQueue_Enqueue(char* seqId);
char* AudioQueue_Dequeue(void);
int32_t AudioQueue_IsEmpty(void);
void AudioQueue_GetSeqInfo(const char* path, uint64_t* numFrames, uint32_t* numChannels, uint32_t* sampleRate,
                           int16_t** sampleData);

#ifdef __cplusplus
}
#endif

#endif
