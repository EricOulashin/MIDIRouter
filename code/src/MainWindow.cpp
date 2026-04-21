#include "MainWindow.h"

#include "MidiClientWorker.h"
#include "MidiLocalWorker.h"
#include "MidiPlayoutEngine.h"
#include "MidiServerController.h"

#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMetaObject>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QStatusBar>
#include <QThread>
#include <QVBoxLayout>
#include <QVariant>

#include <RtMidi.h>

#include <exception>

namespace {

/// Runs bootstrap() without calling QThread's outer exec(), so nested QEventLoops work.
class LocalMidiThread final : public QThread {
public:
    explicit LocalMidiThread(QObject* parent = nullptr)
        : QThread(parent)
    {}

    MidiLocalWorker* worker{nullptr};
    int in_ix{};
    int out_ix{};

protected:
    void run() override
    {
        if (worker)
            worker->bootstrap(in_ix, out_ix);
    }
};

class ClientMidiThread final : public QThread {
public:
    explicit ClientMidiThread(QObject* parent = nullptr)
        : QThread(parent)
    {}

    MidiClientWorker* worker{nullptr};
    QString host;
    quint16 port{};
    int in_ix{};
    bool mirror{};
    int out_ix{};

protected:
    void run() override
    {
        if (worker)
            worker->bootstrap(host, port, in_ix, mirror, out_ix);
    }
};

} // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    try {
        rtmidi_in_probe_  = std::make_unique<RtMidiIn>();
        rtmidi_out_probe_ = std::make_unique<RtMidiOut>();
    } catch (const std::exception& e) {
        QMessageBox::critical(nullptr,
                              QStringLiteral("MIDI Router"),
                              QStringLiteral("RtMidi initialization failed:\n%1").arg(QString::fromUtf8(e.what())));
        std::exit(1);
    }

    const unsigned int n_in  = rtmidi_in_probe_->getPortCount();
    const unsigned int n_out = rtmidi_out_probe_->getPortCount();
    if (n_in == 0 && n_out == 0) {
        QMessageBox::warning(nullptr,
                             QStringLiteral("MIDI Router"),
                             QStringLiteral("No MIDI ports were found. The application will exit."));
        std::exit(0);
    }

    setup_ui();
    populate_midi_lists();

    if (n_in > 0 && n_out == 0) {
        QMessageBox::information(this,
                                 QStringLiteral("MIDI Router"),
                                 QStringLiteral("There are no MIDI output ports; only client mode is available."));
        radio_client_->setChecked(true);
        radio_local_->setEnabled(false);
        radio_server_->setEnabled(false);
        apply_mode_ui();
    } else if (n_in == 0 && n_out > 0) {
        QMessageBox::information(this,
                                 QStringLiteral("MIDI Router"),
                                 QStringLiteral("There are no MIDI input ports; only server mode is available."));
        radio_server_->setChecked(true);
        radio_local_->setEnabled(false);
        radio_client_->setEnabled(false);
        apply_mode_ui();
    }
}

MainWindow::~MainWindow()
{
    stop_all();
}

void MainWindow::setup_ui()
{
    setWindowTitle(QStringLiteral("MIDI Router"));
    setMinimumSize(460, 320);

    auto* central = new QWidget(this);
    setCentralWidget(central);
    auto* root = new QVBoxLayout(central);

    auto* mode_box = new QGroupBox(QStringLiteral("Mode"), central);
    auto* mode_layout = new QHBoxLayout(mode_box);
    radio_local_   = new QRadioButton(QStringLiteral("Local"), mode_box);
    radio_server_  = new QRadioButton(QStringLiteral("Server"), mode_box);
    radio_client_  = new QRadioButton(QStringLiteral("Client"), mode_box);
    chk_local_too_ = new QCheckBox(QStringLiteral("Local too"), mode_box);
    chk_local_too_->setEnabled(false);
    btn_go_        = new QPushButton(QStringLiteral("Start"), mode_box);
    btn_stop_      = new QPushButton(QStringLiteral("Stop"), mode_box);
    btn_stop_->setEnabled(false);
    radio_local_->setChecked(true);
    mode_layout->addWidget(radio_local_);
    mode_layout->addWidget(radio_server_);
    mode_layout->addWidget(radio_client_);
    mode_layout->addWidget(chk_local_too_);
    mode_layout->addWidget(btn_go_);
    mode_layout->addWidget(btn_stop_);
    root->addWidget(mode_box);

    auto* form = new QFormLayout();
    lbl_midi_in_    = new QLabel(QStringLiteral("MIDI In"));
    combo_midi_in_  = new QComboBox();
    lbl_midi_out_   = new QLabel(QStringLiteral("MIDI Out"));
    combo_midi_out_ = new QComboBox();
    lbl_host_       = new QLabel(QStringLiteral("Server"));
    edit_host_      = new QLineEdit();
    edit_host_->setPlaceholderText(QStringLiteral("hostname or IP"));
    lbl_port_       = new QLabel(QStringLiteral("Port"));
    edit_port_      = new QLineEdit(QStringLiteral("2112"));
    lbl_jitter_     = new QLabel(QStringLiteral("Jitter buffer (ms)"));
    spin_jitter_ms_ = new QSpinBox();
    spin_jitter_ms_->setRange(5, 500);
    spin_jitter_ms_->setValue(25);
    spin_jitter_ms_->setToolTip(
        QStringLiteral("Playback delay added on the server so timing stays steady over the network."));
    form->addRow(lbl_midi_in_, combo_midi_in_);
    form->addRow(lbl_midi_out_, combo_midi_out_);
    form->addRow(lbl_host_, edit_host_);
    form->addRow(lbl_port_, edit_port_);
    form->addRow(lbl_jitter_, spin_jitter_ms_);
    root->addLayout(form);

    statusBar()->showMessage(QStringLiteral("Ready"));

    auto* file_menu = menuBar()->addMenu(QStringLiteral("&File"));
    file_menu->addAction(QStringLiteral("E&xit"), QKeySequence::Quit, this, &QWidget::close);

    auto* help_menu = menuBar()->addMenu(QStringLiteral("&Help"));
    help_menu->addAction(QStringLiteral("&About"), this, [this]() {
        QMessageBox::about(
            this,
            QStringLiteral("About MIDI Router"),
            QStringLiteral(
                "<h3>MIDI Router</h3>"
                "<p>Routes MIDI between ports or over TCP with timestamp scheduling for stable timing.</p>"
                "<p>Copyright © Eric Oulashin. Qt / RtMidi.</p>"));
    });

    connect(radio_local_, &QRadioButton::toggled, this, &MainWindow::on_mode_changed);
    connect(radio_server_, &QRadioButton::toggled, this, &MainWindow::on_mode_changed);
    connect(radio_client_, &QRadioButton::toggled, this, &MainWindow::on_mode_changed);
    connect(chk_local_too_, &QCheckBox::toggled, this, &MainWindow::on_local_too_toggled);
    connect(btn_go_, &QPushButton::clicked, this, &MainWindow::on_go);
    connect(btn_stop_, &QPushButton::clicked, this, &MainWindow::on_stop);

    apply_mode_ui();
}

void MainWindow::populate_midi_lists()
{
    combo_midi_in_->clear();
    combo_midi_out_->clear();
    try {
        const unsigned int n_in = rtmidi_in_probe_->getPortCount();
        for (unsigned int i = 0; i < n_in; ++i)
            combo_midi_in_->addItem(QString::fromStdString(rtmidi_in_probe_->getPortName(i)),
                                    QVariant(static_cast<int>(i)));

        const unsigned int n_out = rtmidi_out_probe_->getPortCount();
        for (unsigned int i = 0; i < n_out; ++i)
            combo_midi_out_->addItem(QString::fromStdString(rtmidi_out_probe_->getPortName(i)),
                                     QVariant(static_cast<int>(i)));
    } catch (const std::exception& e) {
        QMessageBox::warning(this,
                             QStringLiteral("MIDI Router"),
                             QStringLiteral("Could not enumerate MIDI ports:\n%1").arg(QString::fromUtf8(e.what())));
    }

    if (combo_midi_in_->count() > 0)
        combo_midi_in_->setCurrentIndex(0);
    if (combo_midi_out_->count() > 0)
        combo_midi_out_->setCurrentIndex(0);
}

Mode MainWindow::mode_from_ui() const
{
    if (radio_server_->isChecked())
        return Mode::Server;
    if (radio_client_->isChecked())
        return Mode::Client;
    return Mode::Local;
}

void MainWindow::apply_mode_ui()
{
    const Mode m = mode_from_ui();
    lbl_host_->setVisible(m == Mode::Client);
    edit_host_->setVisible(m == Mode::Client);
    lbl_port_->setVisible(m == Mode::Client || m == Mode::Server);
    edit_port_->setVisible(m == Mode::Client || m == Mode::Server);

    lbl_midi_in_->setVisible(m == Mode::Local || m == Mode::Client);
    combo_midi_in_->setVisible(m == Mode::Local || m == Mode::Client);

    lbl_midi_out_->setVisible(m == Mode::Local || m == Mode::Server
                              || (m == Mode::Client && chk_local_too_->isChecked()));
    combo_midi_out_->setVisible(m == Mode::Local || m == Mode::Server
                                 || (m == Mode::Client && chk_local_too_->isChecked()));

    lbl_jitter_->setVisible(m == Mode::Server);
    spin_jitter_ms_->setVisible(m == Mode::Server);

    chk_local_too_->setEnabled(m == Mode::Client);

    switch (m) {
        case Mode::Local:
            btn_go_->setText(QStringLiteral("Start"));
            break;
        case Mode::Server:
            btn_go_->setText(QStringLiteral("Host"));
            break;
        case Mode::Client:
            btn_go_->setText(QStringLiteral("Connect"));
            break;
    }
}

void MainWindow::on_mode_changed()
{
    if (!running_)
        apply_mode_ui();
}

void MainWindow::on_local_too_toggled(bool /*on*/)
{
    if (!running_)
        apply_mode_ui();
}

void MainWindow::set_running_ui(bool running)
{
    running_ = running;
    btn_go_->setEnabled(!running);
    btn_stop_->setEnabled(running);
    radio_local_->setEnabled(!running);
    radio_server_->setEnabled(!running);
    radio_client_->setEnabled(!running);
    chk_local_too_->setEnabled(!running && mode_from_ui() == Mode::Client);
    combo_midi_in_->setEnabled(!running);
    combo_midi_out_->setEnabled(!running);
    edit_host_->setEnabled(!running);
    edit_port_->setEnabled(!running);
    spin_jitter_ms_->setEnabled(!running && mode_from_ui() == Mode::Server);
}

void MainWindow::stop_all()
{
    if (local_worker_) {
        QMetaObject::invokeMethod(local_worker_, "shutdown", Qt::BlockingQueuedConnection);
    }
    if (local_thread_) {
        local_thread_->wait(8000);
        delete local_thread_;
        local_thread_ = nullptr;
        local_worker_ = nullptr;
    }

    if (client_worker_) {
        QMetaObject::invokeMethod(client_worker_, "shutdown", Qt::BlockingQueuedConnection);
    }
    if (client_thread_) {
        client_thread_->wait(8000);
        delete client_thread_;
        client_thread_ = nullptr;
        client_worker_ = nullptr;
    }

    server_controller_.reset();
    playout_engine_.reset();
    server_midi_out_.reset();

    set_running_ui(false);
    apply_mode_ui();
    statusBar()->showMessage(QStringLiteral("Stopped"));
}

void MainWindow::on_go()
{
    switch (mode_from_ui()) {
        case Mode::Local:
            start_local();
            break;
        case Mode::Server:
            start_server();
            break;
        case Mode::Client:
            start_client();
            break;
    }
}

void MainWindow::on_stop()
{
    stop_all();
}

void MainWindow::start_local()
{
    if (combo_midi_in_->count() == 0 || combo_midi_out_->count() == 0) {
        QMessageBox::warning(this, QStringLiteral("MIDI Router"), QStringLiteral("Select MIDI In and Out."));
        return;
    }

    auto* t       = new LocalMidiThread(this);
    local_thread_ = t;
    local_worker_ = new MidiLocalWorker();
    local_worker_->moveToThread(t);
    t->worker = local_worker_;
    t->in_ix  = combo_midi_in_->currentData().toInt();
    t->out_ix = combo_midi_out_->currentData().toInt();

    connect(local_worker_, &MidiLocalWorker::stopped, t, &QThread::quit);
    connect(t, &QThread::finished, local_worker_, &QObject::deleteLater);

    set_running_ui(true);
    statusBar()->showMessage(QStringLiteral("Local routing…"));
    t->start();
}

void MainWindow::start_server()
{
    if (combo_midi_out_->count() == 0) {
        QMessageBox::warning(this, QStringLiteral("MIDI Router"), QStringLiteral("No MIDI output available."));
        return;
    }

    bool ok = false;
    const quint16 port = static_cast<quint16>(edit_port_->text().toUInt(&ok));
    if (!ok || port == 0) {
        QMessageBox::warning(this, QStringLiteral("MIDI Router"), QStringLiteral("Invalid port."));
        return;
    }

    try {
        server_midi_out_ = std::make_unique<RtMidiOut>();
        server_midi_out_->openPort(static_cast<unsigned int>(combo_midi_out_->currentData().toInt()));
    } catch (const std::exception& e) {
        QMessageBox::critical(this,
                              QStringLiteral("MIDI Router"),
                              QStringLiteral("Could not open MIDI output:\n%1").arg(QString::fromUtf8(e.what())));
        server_midi_out_.reset();
        return;
    }

    playout_engine_ = std::make_unique<MidiPlayoutEngine>(server_midi_out_.get());
    playout_engine_->set_jitter_buffer(std::chrono::milliseconds(spin_jitter_ms_->value()));

    server_controller_ = std::make_unique<MidiServerController>(playout_engine_.get(), this);
    connect(server_controller_.get(), &MidiServerController::listen_failed, this,
            &MainWindow::on_server_listen_failed);

    QString err;
    if (!server_controller_->listen(port, &err)) {
        server_controller_.reset();
        playout_engine_.reset();
        server_midi_out_.reset();
        return;
    }

    connect(server_controller_.get(), &MidiServerController::client_connected, this, [this]() {
        statusBar()->showMessage(QStringLiteral("Client connected"));
    });
    connect(server_controller_.get(), &MidiServerController::client_disconnected, this, [this]() {
        statusBar()->showMessage(QStringLiteral("Client disconnected — waiting"));
    });

    set_running_ui(true);
    statusBar()->showMessage(QStringLiteral("Waiting for client on port %1…").arg(port));
}

void MainWindow::start_client()
{
    if (combo_midi_in_->count() == 0) {
        QMessageBox::warning(this, QStringLiteral("MIDI Router"), QStringLiteral("No MIDI input available."));
        return;
    }
    if (chk_local_too_->isChecked() && combo_midi_out_->count() == 0) {
        QMessageBox::warning(this, QStringLiteral("MIDI Router"), QStringLiteral("Local too requires a MIDI output."));
        return;
    }

    const QString host = edit_host_->text().trimmed();
    bool ok = false;
    const quint16 port = static_cast<quint16>(edit_port_->text().toUInt(&ok));
    if (host.isEmpty() || !ok || port == 0) {
        QMessageBox::warning(this, QStringLiteral("MIDI Router"), QStringLiteral("Enter server host and port."));
        return;
    }

    auto* t        = new ClientMidiThread(this);
    client_thread_ = t;
    client_worker_ = new MidiClientWorker();
    client_worker_->moveToThread(t);
    t->worker   = client_worker_;
    t->host     = host;
    t->port     = port;
    t->in_ix    = combo_midi_in_->currentData().toInt();
    t->mirror   = chk_local_too_->isChecked();
    t->out_ix   = combo_midi_out_->currentData().toInt();

    connect(client_worker_, &MidiClientWorker::connection_failed, this, &MainWindow::on_client_connection_failed);
    connect(client_worker_, &MidiClientWorker::stopped, t, &QThread::quit);
    connect(t, &QThread::finished, client_worker_, &QObject::deleteLater);

    set_running_ui(true);
    statusBar()->showMessage(QStringLiteral("Connecting…"));
    t->start();
}

void MainWindow::on_client_connection_failed(const QString& err)
{
    QMessageBox::critical(this, QStringLiteral("MIDI Router"), err);
    stop_all();
}

void MainWindow::on_server_listen_failed(const QString& err)
{
    QMessageBox::critical(this, QStringLiteral("MIDI Router"), err);
    stop_all();
}
