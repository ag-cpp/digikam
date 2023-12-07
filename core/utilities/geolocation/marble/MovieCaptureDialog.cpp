// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "MovieCaptureDialog.h"
#include "ui_MovieCaptureDialog.h"

#include <QFileDialog>
#include <QMessageBox>

#include "MarbleWidget.h"
#include "MovieCapture.h"

#include <klocalizedstring.h>

namespace Marble {

MovieCaptureDialog::MovieCaptureDialog(MarbleWidget *widget, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MovieCaptureDialog),
    m_recorder(new MovieCapture(widget, parent))
{
    ui->setupUi(this);
    m_recorder->setSnapshotMethod(MovieCapture::TimeDriven);
    QPushButton *startButton = ui->buttonBox->addButton(i18nc("Start recording a movie", "&Start"), QDialogButtonBox::ActionRole);

    connect(ui->fpsSlider, SIGNAL(valueChanged(int)),
            ui->fpsSpin, SLOT(setValue(int)));

    connect(ui->fpsSpin, SIGNAL(valueChanged(int)),
            ui->fpsSlider, SLOT(setValue(int)));

    connect(ui->fpsSlider, SIGNAL(valueChanged(int)),
            m_recorder, SLOT(setFps(int)));

    connect(ui->buttonBox, SIGNAL(rejected()),
            this, SLOT(close()));

    connect(startButton, SIGNAL(clicked()),
            this, SLOT(startRecording()));

    connect(ui->openButton, SIGNAL(clicked()),
            this, SLOT(loadDestinationFile()));
}

MovieCaptureDialog::~MovieCaptureDialog()
{
    delete ui;
}

void MovieCaptureDialog::loadDestinationFile()
{
    const QVector<MovieFormat> formats = m_recorder->availableFormats();
    if( formats.isEmpty() ) {
        QMessageBox::warning( this, i18n( "Codecs are unavailable" ), i18n( "Supported codecs are not found." ) );
        return;
    }
    QString filter = formats.first().name() + QLatin1String(" (*.") + formats.first().extension() + QLatin1Char(')');
    for( int i = 1; i < formats.size(); i++ )
    {
        filter += QLatin1String(";;") + formats.at(i).name() + QLatin1String(" (*.") + formats.at(i).extension() + QLatin1Char(')');
    }
    const QString defaultFileName =
            ui->destinationEdit->text().isEmpty() ? QString::fromUtf8("") : ui->destinationEdit->text();

    const QString destination =
            QFileDialog::getSaveFileName(this, i18n("Save video file"), defaultFileName,
                                         filter );

    if (destination.isEmpty()) {
        return;
    }

    bool supported = false;
    for(const MovieFormat &format: formats) {
        if (destination.endsWith(QLatin1Char('.') + format.extension())) {
            supported = true;
            break;
        }
    }

    if (!supported) {
        QString formatsExtensions = QLatin1Char('.') + formats.at(0).extension();
        for( int i = 1; i < formats.size(); ++i )
        {
            formatsExtensions += QLatin1String(", .") + formats.at(i).extension();
        }
        QMessageBox::warning(this, i18n("Filename is not valid"),
                             i18n("This file format is not supported. "
                                "Please, use %1 instead").arg( formatsExtensions ) );
        return;
    }

    ui->destinationEdit->setText(destination);
    m_recorder->setFilename(destination);
}

void MovieCaptureDialog::startRecording()
{
    const QString path = ui->destinationEdit->text();

    if (path.isEmpty()) {
        QMessageBox::warning(this, i18n("Missing filename"),
                             i18n("Destination video file is not set. "
                                "I don't know where to save recorded "
                                "video. Please, specify one."));
        return;
    }

    hide();

    m_recorder->setFps(ui->fpsSlider->value());
    if (m_recorder->startRecording())
        Q_EMIT started();
}

void MovieCaptureDialog::stopRecording()
{
    m_recorder->stopRecording();
}

} // namespace Marble

#include "moc_MovieCaptureDialog.cpp"
