#pragma once

#include "modes.h"

#include <QMainWindow>
#include <QString>

#include <memory>

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QSpinBox;
class QStatusBar;

class MidiPlayoutEngine;
class MidiServerController;
class MidiClientWorker;
class MidiLocalWorker;
class QThread;

class RtMidiIn;
class RtMidiOut;

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_mode_changed();
    void on_go();
    void on_stop();
    void on_local_too_toggled(bool on);
    void on_client_connection_failed(const QString& err);
    void on_server_listen_failed(const QString& err);

private:
    void setup_ui();
    void apply_mode_ui();
    void populate_midi_lists();
    void stop_all();
    void set_running_ui(bool running);

    void start_local();
    void start_server();
    void start_client();

    Mode mode_from_ui() const;

    QRadioButton* radio_local_{nullptr};
    QRadioButton* radio_server_{nullptr};
    QRadioButton* radio_client_{nullptr};

    QCheckBox* chk_local_too_{nullptr};

    QLabel* lbl_midi_in_{nullptr};
    QComboBox* combo_midi_in_{nullptr};
    QLabel* lbl_midi_out_{nullptr};
    QComboBox* combo_midi_out_{nullptr};

    QLabel* lbl_host_{nullptr};
    QLineEdit* edit_host_{nullptr};
    QLabel* lbl_port_{nullptr};
    QLineEdit* edit_port_{nullptr};

    QLabel* lbl_jitter_{nullptr};
    QSpinBox* spin_jitter_ms_{nullptr};

    QPushButton* btn_go_{nullptr};
    QPushButton* btn_stop_{nullptr};

    std::unique_ptr<RtMidiIn> rtmidi_in_probe_;
    std::unique_ptr<RtMidiOut> rtmidi_out_probe_;

    bool running_{false};

    QThread* client_thread_{nullptr};
    MidiClientWorker* client_worker_{nullptr};

    QThread* local_thread_{nullptr};
    MidiLocalWorker* local_worker_{nullptr};

    std::unique_ptr<RtMidiOut> server_midi_out_;
    std::unique_ptr<MidiPlayoutEngine> playout_engine_;
    std::unique_ptr<MidiServerController> server_controller_;
};
