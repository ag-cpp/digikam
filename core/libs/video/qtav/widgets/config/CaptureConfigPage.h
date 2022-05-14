/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * Copyright (C)      2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef QTAV_WIDGETS_CAPTURE_CONFIG_PAGE_H
#define QTAV_WIDGETS_CAPTURE_CONFIG_PAGE_H

// Qt includes

#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>

// Local includes

#include "QtAVWidgets_Global.h"
#include "ConfigPageBase.h"

namespace QtAV
{

/*
 * TODO: ConfigPageBase: auto save(true for menu ui, false for dialog ui)
 * virtual public slot: apply()
 */

class AVPlayerSlider;

class QTAV_WIDGETS_EXPORT CaptureConfigPage : public ConfigPageBase
{
    Q_OBJECT

public:

    explicit CaptureConfigPage(QWidget* const parent = nullptr);
    virtual QString name() const override;

protected:

    virtual void applyToUi()     override;
    virtual void applyFromUi()   override;

private Q_SLOTS:

    // only emit Q_SIGNALS. no value stores.

    void selectSaveDir();
    void browseCaptureDir();

private:

    QLineEdit*      mpDir;
    QComboBox*      mpFormat;
    AVPlayerSlider* mpQuality;
};

} // namespace QtAV

#endif // QTAV_WIDGETS_CAPTURE_CONFIG_PAGE_H
