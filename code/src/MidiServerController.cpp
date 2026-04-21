#include "MidiServerController.h"

#include "MidiPlayoutEngine.h"
#include "NetworkMidiProtocol.h"

#include <QTcpServer>
#include <QTcpSocket>

#include <cstring>
#include <vector>

MidiServerController::MidiServerController(MidiPlayoutEngine* engine, QObject* parent)
    : QObject(parent)
    , engine_(engine)
{}

bool MidiServerController::listen(quint16 port, QString* error_out)
{
    stop();
    server_ = std::make_unique<QTcpServer>(this);
    if (!server_->listen(QHostAddress::Any, port)) {
        if (error_out)
            *error_out = server_->errorString();
        emit listen_failed(server_->errorString());
        server_.reset();
        return false;
    }
    connect(server_.get(), &QTcpServer::newConnection, this, &MidiServerController::on_new_connection);
    return true;
}

void MidiServerController::stop()
{
    if (client_) {
        client_->disconnect(this);
        client_->deleteLater();
        client_ = nullptr;
    }
    rx_buffer_.clear();
    legacy_mode_ = false;
    if (server_) {
        server_->close();
        server_.reset();
    }
    if (engine_)
        engine_->reset_session();
}

void MidiServerController::on_new_connection()
{
    if (!server_ || !engine_)
        return;
    // Single client (same as original app).
    if (client_ != nullptr) {
        QTcpSocket* extra = server_->nextPendingConnection();
        if (extra) {
            extra->close();
            extra->deleteLater();
        }
        return;
    }

    client_ = server_->nextPendingConnection();
    if (!client_)
        return;

    engine_->reset_session();
    legacy_mode_ = false;
    rx_buffer_.clear();

    connect(client_, &QTcpSocket::readyRead, this, &MidiServerController::on_ready_read);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    connect(client_, &QTcpSocket::disconnected, this, &MidiServerController::on_client_disconnect);
#else
    connect(client_, &QTcpSocket::disconnected, this, &MidiServerController::on_client_disconnect);
#endif
    emit client_connected();
}

void MidiServerController::on_ready_read()
{
    if (!client_)
        return;
    rx_buffer_.append(client_->readAll());
    consume_buffer();
}

void MidiServerController::consume_buffer()
{
    if (!engine_)
        return;

    while (!rx_buffer_.isEmpty()) {
        const unsigned char* data = reinterpret_cast<const unsigned char*>(rx_buffer_.constData());
        const int n                 = rx_buffer_.size();

        if (!legacy_mode_ && n >= 4) {
            std::uint32_t m = 0;
            std::memcpy(&m, data, sizeof(m));
            if (m != netmidi::kMagic) {
                legacy_mode_ = true;
            }
        }

        if (legacy_mode_) {
            if (n < 4)
                return;
            const std::uint8_t nb = data[3];
            if (nb == 0 || nb > 3) {
                rx_buffer_.remove(0, 1); // resync on garbage
                continue;
            }
            std::vector<unsigned char> msg(nb);
            std::memcpy(msg.data(), data, nb);
            engine_->enqueue_immediate(msg);
            rx_buffer_.remove(0, 4);
            continue;
        }

        if (n < static_cast<int>(sizeof(netmidi::PacketV2)))
            return;

        netmidi::PacketV2 pkt{};
        if (!netmidi::unpack_v2(data, static_cast<std::size_t>(n), &pkt)) {
            legacy_mode_ = true;
            continue;
        }

        std::vector<unsigned char> msg(pkt.num_bytes);
        std::memcpy(msg.data(), pkt.bytes, pkt.num_bytes);
        engine_->enqueue_v2(pkt.sender_steady_ns, msg);
        rx_buffer_.remove(0, static_cast<int>(sizeof(netmidi::PacketV2)));
    }
}

void MidiServerController::on_client_disconnect()
{
    if (client_) {
        client_->deleteLater();
        client_ = nullptr;
    }
    rx_buffer_.clear();
    legacy_mode_ = false;
    if (engine_)
        engine_->reset_session();
    emit client_disconnected();
}
