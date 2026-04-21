#include "MidiClientWorker.h"

#include "NetworkMidiProtocol.h"

#include <QAbstractSocket>
#include <QEventLoop>
#include <QTimer>
#include <QTcpSocket>
#include <QtNetwork>

#include <RtMidi.h>

#include <chrono>
#include <utility>
#include <vector>

namespace {

std::uint64_t steady_ns()
{
    using namespace std::chrono;
    return duration_cast<nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

} // namespace

MidiClientWorker::MidiClientWorker(QObject* parent)
    : QObject(parent)
{}

MidiClientWorker::~MidiClientWorker()
{
    shutdown();
}

void MidiClientWorker::bootstrap(const QString& host, quint16 port, int midi_in_port_index,
                                 bool mirror_local, int midi_out_port_index)
{
    event_loop_ = std::make_unique<QEventLoop>();

    socket_ = new QTcpSocket(this);

    auto* timeout = new QTimer(this);
    timeout->setSingleShot(true);
    timeout->setInterval(15000);
    connect(socket_, &QTcpSocket::connected, timeout, &QTimer::stop);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    connect(socket_, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError) {
        if (!running_.load(std::memory_order_relaxed))
            return;
        emit connection_failed(socket_->errorString());
        if (event_loop_)
            event_loop_->quit();
    });
#else
    connect(socket_,
            QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
            this,
            [this](QAbstractSocket::SocketError) {
                if (!running_.load(std::memory_order_relaxed))
                    return;
                emit connection_failed(socket_->errorString());
                if (event_loop_)
                    event_loop_->quit();
            });
#endif

    connect(socket_, &QTcpSocket::connected, this, [this, midi_in_port_index, mirror_local, midi_out_port_index]() {
        try {
            midi_in_ = std::make_unique<RtMidiIn>();
            midi_in_->ignoreTypes(/*sysex*/ true, /*timing*/ true, /*sense*/ true);
            midi_in_->openPort(static_cast<unsigned int>(midi_in_port_index));
            midi_in_->setCallback(&MidiClientWorker::rt_callback, this);

            if (mirror_local) {
                midi_out_mirror_ = std::make_unique<RtMidiOut>();
                midi_out_mirror_->openPort(static_cast<unsigned int>(midi_out_port_index));
            }
        } catch (const std::exception& e) {
            emit connection_failed(QString::fromUtf8(e.what()));
            if (event_loop_)
                event_loop_->quit();
            return;
        }
    });

    connect(timeout, &QTimer::timeout, this, [this]() {
        if (socket_ && socket_->state() != QAbstractSocket::ConnectedState) {
            emit connection_failed(QStringLiteral("Connection timed out."));
            socket_->abort();
            if (event_loop_)
                event_loop_->quit();
        }
    });
    timeout->start();

    socket_->connectToHost(host, port);

    event_loop_->exec();
    timeout->disconnect();
    timeout->deleteLater();

    event_loop_.reset();
    emit stopped();
}

void MidiClientWorker::shutdown()
{
    running_ = false;
    if (socket_) {
        socket_->abort();
    }
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
    if (midi_out_mirror_) {
        try {
            midi_out_mirror_->closePort();
        } catch (...) {
        }
        midi_out_mirror_.reset();
    }
    if (socket_) {
        socket_->deleteLater();
        socket_ = nullptr;
    }
}

void MidiClientWorker::rt_callback(double /*delta_sec*/, std::vector<unsigned char>* message,
                                   void* user_data)
{
    auto* self = static_cast<MidiClientWorker*>(user_data);
    if (!self || !message || !self->running_.load(std::memory_order_relaxed))
        return;

    const quint64 ns = steady_ns();
    QByteArray payload(reinterpret_cast<const char*>(message->data()),
                       static_cast<int>(message->size()));

    QMetaObject::invokeMethod(self,
                              "deliver_outgoing",
                              Qt::QueuedConnection,
                              Q_ARG(QByteArray, payload),
                              Q_ARG(quint64, ns));
}

void MidiClientWorker::deliver_outgoing(QByteArray payload, quint64 steady_ns_in)
{
    if (!running_.load(std::memory_order_relaxed))
        return;

    std::vector<unsigned char> raw(payload.begin(), payload.end());

    if (midi_out_mirror_) {
        try {
            midi_out_mirror_->sendMessage(&raw);
        } catch (...) {
        }
    }

    netmidi::PacketV2 pkt{};
    netmidi::pack_v2(pkt, static_cast<std::uint64_t>(steady_ns_in), raw.data(), raw.size());

    if (socket_ && socket_->state() == QAbstractSocket::ConnectedState) {
        socket_->write(reinterpret_cast<const char*>(&pkt), static_cast<int>(sizeof(pkt)));
    }
}
