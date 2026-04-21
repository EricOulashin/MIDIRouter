#pragma once

#include <RtMidi.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

// Schedules outgoing MIDI using sender-side steady_clock timestamps so that
// inter-onset timing is preserved across variable network delay.

class MidiPlayoutEngine final {
public:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;

    explicit MidiPlayoutEngine(RtMidiOut* midi_out);
    ~MidiPlayoutEngine();

    MidiPlayoutEngine(const MidiPlayoutEngine&)            = delete;
    MidiPlayoutEngine& operator=(const MidiPlayoutEngine&) = delete;

    void set_jitter_buffer(std::chrono::milliseconds buffer);
    std::chrono::milliseconds jitter_buffer() const;

    // Call when a new TCP session starts (or client reconnects).
    void reset_session();

    // Schedule one message using capture-time on the sender (steady_clock ns).
    void enqueue_v2(std::uint64_t sender_steady_ns, const std::vector<unsigned char>& bytes);

    // Legacy / no timestamp: play as soon as possible (still passes through thread).
    void enqueue_immediate(const std::vector<unsigned char>& bytes);

    void stop();

private:
    struct Event {
        TimePoint deadline;
        std::vector<unsigned char> bytes;

        bool operator>(const Event& o) const { return deadline > o.deadline; }
    };

    void thread_main();

    RtMidiOut* midi_out_{};
    std::chrono::milliseconds jitter_buffer_{15};

    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::priority_queue<Event, std::vector<Event>, std::greater<Event>> queue_;
    bool stop_{false};

    bool session_ready_{false};
    std::uint64_t origin_sender_ns_{0};
    TimePoint play_anchor_{};

    std::thread worker_;
};
