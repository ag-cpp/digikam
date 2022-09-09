/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * https://www.digikam.org
 *
 * Date        : 2022-08-26
 * Description : OCR settings widgets
 *
 * Copyright (C) 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2022      by Quoc Hung Tran <quochungtran1999 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_TEXT_CONVERTER_SETTINGS_H
#define DIGIKAM_TEXT_CONVERTER_SETTINGS_H

// Qt includes

#include <QWidget>
#include <QString>
#include <QStringList>

// Local includes

#include "ocroptions.h"

namespace DigikamGenericTextConverterPlugin
{

class TextConverterSettings : public QWidget
{
    Q_OBJECT

public:

    explicit TextConverterSettings(QWidget* const parent = nullptr);
    ~TextConverterSettings() override;

    OcrOptions ocrOptions() const;
    void setOcrOptions(const OcrOptions& opt);

    void setDefaultSettings();

    void populateLanguagesMode(const QStringList& langs);

Q_SIGNALS:

    void signalSettingsChanged();

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericTextConverterPlugin

#endif // DIGIKAM_TEXT_CONVERTER_SETTINGS_H
