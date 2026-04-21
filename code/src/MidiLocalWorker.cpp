#include "MidiLocalWorker.h"

#include <QEventLoop>

#include <RtMidi.h>

#include <vector>

MidiLocalWorker::MidiLocalWorker(QObject* parent)
    : QObject(parent)
{}

MidiLocalWorker::~MidiLocalWorker()
{
    shutdown();
}

void MidiLocalWorker::bootstrap(int midi_in_index, int midi_out_index)
{
    try {
        midi_in_  = std::make_unique<RtMidiIn>();
        midi_out_ = std::make_unique<RtMidiOut>();
        midi_in_->ignoreTypes(false, false, false);
        midi_in_->openPort(static_cast<unsigned int>(midi_in_index));
        midi_out_->openPort(static_cast<unsigned int>(midi_out_index));
        midi_in_->setCallback(&MidiLocalWorker::rt_callback, this);
    } catch (const std::exception&) {
        event_loop_.reset();
        emit stopped();
        return;
    }


    event_loop_ = std::make_unique<QEventLoop>();
    event_loop_->exec();
    event_loop_.reset();
    emit stopped();
}

void MidiLocalWorker::shutdown()
{
    running_ = false;
    if (event_loop_)
        event_loop_->quit();
    if (midi_in_) {
        try {
            midi_in_->cancelCallback();
            midi_in_->closePort();
        } catch (...) {
        }
        midi_in_.reset();
    }
    if (midi_out_) {
        try {
            midi_out_->closePort();
        } catch (...) {
        }
        midi_out_.reset();
    }
}

void MidiLocalWorker::rt_callback(double /*delta_sec*/, std::vector<unsigned char>* message,
                                  void* user_data)
{
    auto* self = static_cast<MidiLocalWorker*>(user_data);
    if (!self || !message || !self->running_.load(std::memory_order_relaxed))
        return;
    QByteArray payload(reinterpret_cast<const char*>(message->data()),
                       static_cast<int>(message->size()));
    QMetaObject::invokeMethod(self,
                              "forward",
                              Qt::QueuedConnection,
                              Q_ARG(QByteArray, payload));
}

void MidiLocalWorker::forward(QByteArray payload)
{
    if (!running_.load(std::memory_order_relaxed) || !midi_out_)
        return;
    std::vector<unsigned char> raw(payload.begin(), payload.end());
    try {
        midi_out_->sendMessage(&raw);
    } catch (...) {
    }
}
