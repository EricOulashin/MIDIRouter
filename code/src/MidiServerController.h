#pragma once

#include <QByteArray>
#include <QTcpServer>
#include <QObject>
#include <QtGlobal>

#include <memory>
class QTcpSocket;
class MidiPlayoutEngine;

class MidiServerController final : public QObject {
    Q_OBJECT

public:
    explicit MidiServerController(MidiPlayoutEngine* engine, QObject* parent = nullptr);

    bool listen(quint16 port, QString* error_out);

    void stop();

signals:
    void client_connected();
    void client_disconnected();
    void listen_failed(const QString& reason);

private slots:
    void on_new_connection();
    void on_ready_read();
    void on_client_disconnect();

private:
    void consume_buffer();

    MidiPlayoutEngine* engine_{nullptr};
    std::unique_ptr<QTcpServer> server_;
    QTcpSocket* client_{nullptr};
    QByteArray rx_buffer_;
    bool legacy_mode_{false};
};
