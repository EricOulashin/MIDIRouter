#include "MidiPlayoutEngine.h"

#include <utility>

MidiPlayoutEngine::MidiPlayoutEngine(RtMidiOut* midi_out)
    : midi_out_(midi_out)
{
    worker_ = std::thread([this] { thread_main(); });
}

MidiPlayoutEngine::~MidiPlayoutEngine()
{
    stop();
}

void MidiPlayoutEngine::set_jitter_buffer(std::chrono::milliseconds buffer)
{
    std::lock_guard<std::mutex> lock(mutex_);
    jitter_buffer_ = buffer;
}

std::chrono::milliseconds MidiPlayoutEngine::jitter_buffer() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return jitter_buffer_;
}

void MidiPlayoutEngine::reset_session()
{
    std::lock_guard<std::mutex> lock(mutex_);
    session_ready_      = false;
    origin_sender_ns_   = 0;
    play_anchor_        = {};
    while (!queue_.empty())
        queue_.pop();
    cv_.notify_one();
}

void MidiPlayoutEngine::enqueue_v2(std::uint64_t sender_steady_ns,
                                   const std::vector<unsigned char>& bytes)
{
    if (bytes.empty())
        return;

    std::unique_lock<std::mutex> lock(mutex_);

    if (!session_ready_) {
        session_ready_    = true;
        origin_sender_ns_ = sender_steady_ns;
        play_anchor_      = Clock::now() + jitter_buffer_;
    }

    const auto offset = sender_steady_ns - origin_sender_ns_;
    Event ev;
    ev.deadline = play_anchor_ + std::chrono::nanoseconds(static_cast<long long>(offset));
    ev.bytes    = bytes;

    // If we are already late (or buffer very small), clamp to "now".
    const auto now = Clock::now();
    if (ev.deadline < now)
        ev.deadline = now;

    queue_.push(std::move(ev));
    cv_.notify_one();
}

void MidiPlayoutEngine::enqueue_immediate(const std::vector<unsigned char>& bytes)
{
    if (bytes.empty())
        return;
    std::unique_lock<std::mutex> lock(mutex_);
    Event ev;
    ev.deadline = Clock::now();
    ev.bytes    = bytes;
    queue_.push(std::move(ev));
    cv_.notify_one();
}

void MidiPlayoutEngine::stop()
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        stop_ = true;
    }
    cv_.notify_one();
    if (worker_.joinable())
        worker_.join();
}

void MidiPlayoutEngine::thread_main()
{
    std::unique_lock<std::mutex> lock(mutex_);
    while (!stop_) {
        while (!queue_.empty() && queue_.top().deadline <= Clock::now()) {
            Event ev = queue_.top();
            queue_.pop();
            lock.unlock();
            if (midi_out_)
                midi_out_->sendMessage(&ev.bytes);
            lock.lock();
        }
        if (stop_)
            break;
        if (queue_.empty()) {
            cv_.wait(lock, [this] { return stop_ || !queue_.empty(); });
            continue;
        }
        cv_.wait_until(lock, queue_.top().deadline, [this] {
            return stop_ || queue_.empty() || queue_.top().deadline <= Clock::now();
        });
    }
}
