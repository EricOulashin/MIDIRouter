#pragma once

#include <QByteArray>
#include <QObject>
#include <QString>
#include <QtGlobal>

#include <memory>

#include <atomic>
#include <cstdint>
class QEventLoop;
class QTcpSocket;
class RtMidiIn;
class RtMidiOut;

// Lives on a QThread with an event loop (exec): owns socket and receives RtMidi callbacks
// via queued slot for thread-safe TCP writes.

class MidiClientWorker final : public QObject {
    Q_OBJECT

public:
    explicit MidiClientWorker(QObject* parent = nullptr);
    ~MidiClientWorker() override;

public slots:
    /// Blocks until connected or timeout, opens MIDI, then calls exec() until quit().
    void bootstrap(const QString& host, quint16 port, int midi_in_port_index, bool mirror_local,
                   int midi_out_port_index);

    void shutdown();

signals:
    void connection_failed(const QString& reason);
    void stopped();

public slots:
    void deliver_outgoing(QByteArray payload, quint64 steady_ns);

private:
    static void rt_callback(double delta_sec, std::vector<unsigned char>* message, void* user_data);

    QTcpSocket* socket_{nullptr};
    std::unique_ptr<RtMidiIn> midi_in_;
    std::unique_ptr<RtMidiOut> midi_out_mirror_;

    std::atomic<bool> running_{true};

    std::unique_ptr<QEventLoop> event_loop_;
};
