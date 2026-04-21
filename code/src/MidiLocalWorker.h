#pragma once

#include <QByteArray>
#include <QObject>

#include <atomic>
#include <memory>

class QEventLoop;
class RtMidiIn;
class RtMidiOut;

class MidiLocalWorker final : public QObject {
    Q_OBJECT

public:
    explicit MidiLocalWorker(QObject* parent = nullptr);
    ~MidiLocalWorker() override;

public slots:
    void bootstrap(int midi_in_index, int midi_out_index);
    void shutdown();

signals:
    void stopped();

public slots:
    void forward(QByteArray payload);

private:
    static void rt_callback(double delta_sec, std::vector<unsigned char>* message, void* user_data);

    std::unique_ptr<RtMidiIn> midi_in_;
    std::unique_ptr<RtMidiOut> midi_out_;

    std::atomic<bool> running_{true};
    std::unique_ptr<QEventLoop> event_loop_;
};
