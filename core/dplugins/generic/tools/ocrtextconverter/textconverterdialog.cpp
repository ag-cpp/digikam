/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * https://www.digikam.org
 *
 * Date        : 2022-08-26
 * Description : Text converter batch dialog
 *
 * Copyright (C) 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2022      by Quoc Hung Tran <quochungtran1999 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "textconverterdialog.h"

// C++ includes

#include <sstream>

// Qt includes

#include <QGridLayout>
#include <QTimer>
#include <QMessageBox>

// KDE includes

#include <kconfiggroup.h>
#include <ksharedconfig.h>
#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "dcombobox.h"
#include "dprogresswdg.h"
#include "textconverterlist.h"
#include "textconvertersettings.h"
#include "ocroptions.h"
#include "dtextedit.h"
#include "textconverterthread.h"
#include "textconverteraction.h"
#include "tesseractbinary.h"
#include "dbinarysearch.h"

using namespace Digikam;

namespace DigikamGenericTextConverterPlugin
{

class TextConverterDialog::Private
{
public:

    Private()
      : busy                (false),
        progressBar         (nullptr),
        thread              (nullptr),
        iface               (nullptr),
        listView            (nullptr),
        ocrSettings         (nullptr),
        textedit            (nullptr),
        saveTextButton      (nullptr),
        currentSelectedItem (nullptr),
        binWidget           (nullptr)
    {
    }

    bool                              busy;

    QList<QUrl>                       fileList;

    QMap<QUrl, QString>               textEditList;

    DProgressWdg*                     progressBar;

    TextConverterActionThread*        thread;

    DInfoInterface*                   iface;

    TextConverterList*                listView;

    TextConverterSettings*            ocrSettings;

    DTextEdit*                        textedit;

    QPushButton*                      saveTextButton;

    TextConverterListViewItem*        currentSelectedItem;

    OcrTesseractEngine                ocrEngine;

    TesseractBinary                   tesseractBin;
    DBinarySearch*                    binWidget;
};

TextConverterDialog::TextConverterDialog(QWidget* const parent, DInfoInterface* const iface)
    : DPluginDialog(parent, QLatin1String("Text Converter Dialog")),
      d            (new Private)
{
    setWindowTitle(i18nc("@title", "Text Converter"));
    setMinimumSize(900, 500);
    setModal(true);

    d->iface                  = iface;

    m_buttons->addButton(QDialogButtonBox::Close);
    m_buttons->addButton(QDialogButtonBox::Ok);
    m_buttons->button(QDialogButtonBox::Ok)->setText(i18nc("@action:button", "&Start OCR"));
    m_buttons->button(QDialogButtonBox::Ok)->setDisabled(true);
    QWidget* const mainWidget = new QWidget(this);

    QVBoxLayout* const vbx    = new QVBoxLayout(this);
    vbx->addWidget(mainWidget);
    vbx->addWidget(m_buttons);
    setLayout(vbx);

    //-------------------------------------------------------------------------------------------

    QGridLayout* const mainLayout     = new QGridLayout(mainWidget);
    d->listView                       = new TextConverterList(mainWidget);

    QLabel* const tesseractLabel      = new QLabel(i18nc("@label", "This tool use the %1 open-source "
                                                   "engine to perform Optical Characters Recognition. "
                                                   "Tesseract program and the desired languages packages must "
                                                   "be installed on your system.",
                                                   QString::fromUtf8("<a href='https://github.com/tesseract-ocr/tesseract'>Tesseract</a>")),
                                                   mainWidget);
    tesseractLabel->setWordWrap(true);
    tesseractLabel->setOpenExternalLinks(true);

    d->binWidget                      = new DBinarySearch(mainWidget);
    d->binWidget->addBinary(d->tesseractBin);

#ifdef Q_OS_MACOS

    // Std Macports install

    d->binWidget->addDirectory(QLatin1String("/opt/local/bin"));

    // digiKam Bundle PKG install

    d->binWidget->addDirectory(macOSBundlePrefix() + QLatin1String("bin"));

#endif

#ifdef Q_OS_WIN

    d->binWidget->addDirectory(QLatin1String("C:/Program Files/digiKam"));

#endif

#ifdef Q_OS_UNIX

    d->binWidget->addDirectory(QLatin1String("/usr/bin"));
    d->binWidget->addDirectory(QLatin1String("/usr/local/bin"));
    d->binWidget->addDirectory(QLatin1String("/bin"));

#endif

    //-------------------------------------------------------------------------------------------

    d->ocrSettings                    = new TextConverterSettings(mainWidget);

    d->progressBar                    = new DProgressWdg(mainWidget);
    d->progressBar->reset();
    d->progressBar->hide();

    d->textedit                       = new DTextEdit(mainWidget);
    d->textedit->setLinesVisible(20);
    d->textedit->setPlaceholderText(QLatin1String("Recognized text is displayed here"));

    d->saveTextButton                 = new QPushButton(mainWidget);
    d->saveTextButton->setText(i18nc("@action: button", "Save"));
    d->saveTextButton->setEnabled(false);

    //-------------------------------------------------------------------------------------------

    mainLayout->addWidget(d->listView,                       0, 0, 9, 1);
    mainLayout->addWidget(tesseractLabel,                    0, 1, 1, 1);
    mainLayout->addWidget(d->binWidget,                      1, 1, 2, 1);
    mainLayout->addWidget(d->ocrSettings,                    3, 1, 1, 1);
    mainLayout->addWidget(d->textedit,                       4, 1, 1, 1);
    mainLayout->addWidget(d->saveTextButton,                 5, 1, 1, 1);
    mainLayout->addWidget(d->progressBar,                    6, 1, 1, 1);
    mainLayout->setColumnStretch(0, 10);
    mainLayout->setRowStretch(1, 2);
    mainLayout->setRowStretch(4, 10);
    mainLayout->setContentsMargins(QMargins());

    // ---------------------------------------------------------------

    d->thread = new TextConverterActionThread(this);

    connect(d->thread, SIGNAL(signalStarting(DigikamGenericTextConverterPlugin::TextConverterActionData)),
            this, SLOT(slotTextConverterAction(DigikamGenericTextConverterPlugin::TextConverterActionData)));

    connect(d->thread, SIGNAL(signalFinished(DigikamGenericTextConverterPlugin::TextConverterActionData)),
            this, SLOT(slotTextConverterAction(DigikamGenericTextConverterPlugin::TextConverterActionData)));

    connect(d->thread, SIGNAL(finished()),
            this, SLOT(slotThreadFinished()));

    // ---------------------------------------------------------------

    connect(m_buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
            this, SLOT(slotStartStop()));

    connect(m_buttons->button(QDialogButtonBox::Close), SIGNAL(clicked()),
            this, SLOT(slotClose()));

    connect(d->ocrSettings, SIGNAL(signalSettingsChanged()),
            this, SLOT(slotStartStop()));

    connect(d->progressBar, SIGNAL(signalProgressCanceled()),
            this, SLOT(slotStartStop()));

    connect(d->listView->listView(), &DItemsListView::itemDoubleClicked,
            this, &TextConverterDialog::slotDoubleClick);

    connect(d->listView->listView(), &DItemsListView::itemSelectionChanged,
            this, &TextConverterDialog::slotSetDisable);

    connect(d->saveTextButton, SIGNAL(clicked()),
            this, SLOT(slotUpdateText()));

    connect(this, SIGNAL(signalMetadataChangedForUrl(QUrl)),
            d->iface, SLOT(slotMetadataChangedForUrl(QUrl)));

    connect(d->binWidget, SIGNAL(signalBinariesFound(bool)),
            this, SLOT(slotTesseractBinaryFound(bool)));

    // ---------------------------------------------------------------

    d->listView->setIface(d->iface);
    d->listView->loadImagesFromCurrentSelection();

    readSettings();

    // ---------------------------------------------------------------

    QTimer::singleShot(0, this, SLOT(slotStartFoundTesseract()));
}

TextConverterDialog::~TextConverterDialog()
{
    delete d;
}

void TextConverterDialog::slotDoubleClick(QTreeWidgetItem* element)
{
    TextConverterListViewItem* const item = dynamic_cast<TextConverterListViewItem*>(element);

    if (!item)
    {
        d->currentSelectedItem = nullptr;
        return;
    }

    d->currentSelectedItem = item;

    if (d->textEditList.contains(item->url()))
    {
        d->textedit->setText(d->textEditList[item->url()]);
        d->saveTextButton->setEnabled(true);
    }
    else
    {
        d->textedit->clear();
    }
}

void TextConverterDialog::slotUpdateText()
{
    QString newText = d->textedit->text();
    OcrOptions opt  = d->ocrSettings->ocrOptions();

    if (!d->textedit->text().isEmpty()           &&
        !d->currentSelectedItem->url().isEmpty() &&
        !d->currentSelectedItem->destFileName().isEmpty())
    {
        d->textEditList[d->currentSelectedItem->url()] = newText;
        d->currentSelectedItem->setRecognizedWords(QString::fromLatin1("%1").arg(calculateNumberOfWords(newText)));

        if (opt.isSaveTextFile)
        {
            d->ocrEngine.saveTextFile(d->currentSelectedItem->destFileName(), newText);
        }

        if (opt.isSaveXMP)
        {
            d->ocrEngine.saveXMP(d->currentSelectedItem->url().toLocalFile(), newText);

            Q_EMIT signalMetadataChangedForUrl(d->currentSelectedItem->url());
        }
    }
}

void TextConverterDialog::slotSetDisable()
{
    d->saveTextButton->setEnabled(false);
}

void TextConverterDialog::slotTextConverterAction(const DigikamGenericTextConverterPlugin::TextConverterActionData& ad)
{
    if (ad.starting)
    {
        switch (ad.action)
        {
            case(PROCESS):
            {
                busy(true);
                d->listView->processing(ad.fileUrl);
                d->progressBar->progressStatusChanged(i18n("Processing %1", ad.fileUrl.fileName()));
                break;
            }

            default:
            {
                qCWarning(DIGIKAM_GENERAL_LOG) << "DigikamGenericTextConverterPlugin: Unknown action";
                break;
            }
        }
    }
    else
    {
        if (ad.result != OcrTesseractEngine::PROCESS_COMPLETE)
        {
            switch (ad.action)
            {
                case(PROCESS):
                {
                    processingFailed(ad.fileUrl, ad.result);
                    break;
                }

                default:
                {
                    qCWarning(DIGIKAM_GENERAL_LOG) << "DigikamGenericTextConverterPlugin: Unknown action";
                    break;
                }
            }
        }
        else                    // Something is done...
        {
            switch (ad.action)
            {
                case(PROCESS):
                {
                    d->textEditList[ad.fileUrl] = ad.outputText;
                    processed(ad.fileUrl, ad.destPath, ad.outputText);
                    Q_EMIT signalMetadataChangedForUrl(ad.fileUrl);
                    break;
                }

                default:
                {
                    qCWarning(DIGIKAM_GENERAL_LOG) << "DigikamGenericTextConverterPlugin: Unknown action";
                    break;
                }
            }
        }
    }
}

void TextConverterDialog::processingFailed(const QUrl& url, int result)
{
    d->listView->processed(url, false);
    d->progressBar->setValue(d->progressBar->value()+1);

    TextConverterListViewItem* const item = dynamic_cast<TextConverterListViewItem*>(d->listView->listView()->findItem(url));

    if (!item)
    {
        return;
    }

    QString status;

    switch (result)
    {
        case OcrTesseractEngine::PROCESS_FAILED:
        {
            status = i18n("Process failed");
            break;
        }

        case OcrTesseractEngine::PROCESS_CANCELED:
        {
            status = i18n("Process canceled");
            break;
        }

        // TODO Tesseract ocr error

        default:
        {
            status = i18n("Internal error");
            break;
        }
    }

    item->setStatus(status);
}

int TextConverterDialog::calculateNumberOfWords(const QString& text) const
{
    if (!text.isEmpty())
    {
        std::stringstream ss;
        ss << text.toStdString();

        int count = 0;
        std::string word;

        while (ss >> word)
        {
            if ((word.length() == 1) && std::ispunct(word[0]))
            {
                continue;
            }

            count++;
        }

        return count;
    }

    return 0;
}

void TextConverterDialog::processed(const QUrl& url,
                                    const QString& outputFile,
                                    const QString& ocrResult)
{
    TextConverterListViewItem* const item = dynamic_cast<TextConverterListViewItem*>(d->listView->listView()->findItem(url));

    if (!item)
    {
        return;
    }

    if (!outputFile.isEmpty())
    {
        item->setDestFileName(outputFile);
    }

    d->listView->processed(url, true);
    item->setStatus(i18n("Success"));
    item->setRecognizedWords(QString::fromLatin1("%1").arg(calculateNumberOfWords(ocrResult)));
    d->progressBar->setValue(d->progressBar->value() + 1);
}

void TextConverterDialog::processAll()
{
    OcrOptions opt    = d->ocrSettings->ocrOptions();
    opt.tesseractPath = d->tesseractBin.path();
    d->thread->setOcrOptions(opt);
    d->thread->ocrProcessFiles(d->fileList);

    if (!d->thread->isRunning())
    {
        d->thread->start();
    }
}

void TextConverterDialog::slotStartStop()
{
    if (!d->busy)
    {
        d->fileList.clear();

        if (d->listView->listView()->topLevelItemCount() == 0)
        {
            d->textedit->clear();
        }

        QTreeWidgetItemIterator it(d->listView->listView());

        while (*it)
        {
            TextConverterListViewItem* const lvItem = dynamic_cast<TextConverterListViewItem*>(*it);

            if (lvItem)
            {
                if (!lvItem->isDisabled() && (lvItem->state() != TextConverterListViewItem::Success))
                {
                    lvItem->setIcon(1, QIcon());
                    lvItem->setState(TextConverterListViewItem::Waiting);
                    d->fileList.append(lvItem->url());
                }
            }

            ++it;
        }

        if (d->fileList.empty())
        {
            QMessageBox::information(this, i18n("Text Converter"),
                                     i18n("The list does not contain any digital files to process. You need to select them."));
            busy(false);
            slotAborted();

            return;
        }

        d->progressBar->setMaximum(d->fileList.count());
        d->progressBar->setValue(0);
        d->progressBar->show();
        d->progressBar->progressScheduled(i18n("Text Converter"), true, true);
        d->progressBar->progressThumbnailChanged(QIcon::fromTheme(QLatin1String("image-x-adobe-dng")).pixmap(22, 22));

        processAll();
    }
    else
    {
        d->fileList.clear();
        d->thread->cancel();
        busy(false);

        d->listView->cancelProcess();

        QTimer::singleShot(500, this, SLOT(slotAborted()));
    }
}

void TextConverterDialog::closeEvent(QCloseEvent* e)
{
    if (!e)
    {
        return;
    }

    // Stop current conversion if necessary

    if (d->busy)
    {
        slotStartStop();
    }

    saveSettings();
    d->listView->listView()->clear();
    e->accept();
}

void TextConverterDialog::slotClose()
{
    // Stop current conversion if necessary

    if (d->busy)
    {
        slotStartStop();
    }

    saveSettings();
    d->listView->listView()->clear();
    d->fileList.clear();
    accept();
}

void TextConverterDialog::slotDefault()
{
    d->ocrSettings->setDefaultSettings();
}

void TextConverterDialog::readSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("OCR Tesseract Settings"));
    OcrOptions opt;
    opt.language       = group.readEntry("ocrLanguages",          int(OcrOptions::Languages::DEFAULT));
    opt.psm            = group.readEntry("PageSegmentationModes", int(OcrOptions::PageSegmentationModes::DEFAULT));
    opt.oem            = group.readEntry("EngineModes",           int(OcrOptions::EngineModes::DEFAULT));
    opt.dpi            = group.readEntry("Dpi",                   300);
    opt.isSaveTextFile = group.readEntry("Check Save Test File",  true);
    opt.isSaveXMP      = group.readEntry("Check Save in XMP",     true);

    d->ocrSettings->setOcrOptions(opt);
}

void TextConverterDialog::saveSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("OCR Tesseract Settings"));
    OcrOptions opt            = d->ocrSettings->ocrOptions();

    group.writeEntry("ocrLanguages",              opt.language);
    group.writeEntry("PageSegmentationModes",     (int)opt.psm);
    group.writeEntry("EngineModes",               (int)opt.oem);
    group.writeEntry("Dpi",                       (int)opt.dpi);
    group.writeEntry("Check Save Test File",      (bool)opt.isSaveTextFile);
    group.writeEntry("Check Save in XMP",         (bool)opt.isSaveXMP);
}

void TextConverterDialog::addItems(const QList<QUrl>& itemList)
{
    d->listView->slotAddImages(itemList);
}

void TextConverterDialog::slotThreadFinished()
{
    busy(false);
    slotAborted();
}

void TextConverterDialog::busy(bool busy)
{
    d->busy = busy;

    if (d->busy)
    {
        m_buttons->button(QDialogButtonBox::Ok)->setText(i18n("&Abort"));
        m_buttons->button(QDialogButtonBox::Ok)->setToolTip(i18n("Abort OCR processing of Raw files."));
    }
    else
    {
        m_buttons->button(QDialogButtonBox::Ok)->setText(i18n("&Start OCR"));
        m_buttons->button(QDialogButtonBox::Ok)->setToolTip(i18n("Start OCR using the current settings."));
    }

    d->ocrSettings->setEnabled(!d->busy);
    d->listView->listView()->viewport()->setEnabled(!d->busy);
    d->busy ? setCursor(Qt::WaitCursor) : unsetCursor();
}

void TextConverterDialog::slotAborted()
{
    d->progressBar->setValue(0);
    d->progressBar->hide();
    d->progressBar->progressCompleted();
}

void TextConverterDialog::slotStartFoundTesseract()
{
    if (d->binWidget->allBinariesFound())
    {
        slotTesseractBinaryFound(true);
    }
}

void TextConverterDialog::slotTesseractBinaryFound(bool b)
{
    busy(false);

    // Disable Start button if Tesseract is not found.

    m_buttons->button(QDialogButtonBox::Ok)->setDisabled(b);

    if (b)
    {
        m_buttons->button(QDialogButtonBox::Ok)->setToolTip(i18n("Start OCR using the current settings."));
    }
    else
    {
        m_buttons->button(QDialogButtonBox::Ok)->setToolTip(i18n("Tesseract program is not found on your system."));
    }

    d->ocrSettings->populateLanguagesMode(d->tesseractBin.tesseractLanguages());
}

} // namespace DigikamGenericTextConverterPlugin
