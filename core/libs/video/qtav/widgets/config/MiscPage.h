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

#ifndef QTAV_WIDGETS_MISC_PAGE_H
#define QTAV_WIDGETS_MISC_PAGE_H

#include "ConfigPageBase.h"

// Qt includes

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QComboBox>

// Local includes

#include "QtAVWidgets_Global.h"

namespace QtAV
{

class QTAV_WIDGETS_EXPORT MiscPage : public ConfigPageBase
{
    Q_OBJECT

public:

    MiscPage(bool previewOpt = true);

    virtual QString name() const override;

protected:

    virtual void applyToUi()     override;
    virtual void applyFromUi()   override;

private:

    QCheckBox*      m_preview_on      = nullptr;
    QSpinBox*       m_preview_w       = nullptr;
    QSpinBox*       m_preview_h       = nullptr;
    QSpinBox*       m_notify_interval = nullptr;
    QDoubleSpinBox* m_fps             = nullptr;
    QSpinBox*       m_buffer_value    = nullptr;
    QDoubleSpinBox* m_timeout         = nullptr;
    QCheckBox*      m_timeout_abort   = nullptr;
    QComboBox*      m_opengl          = nullptr;
    QComboBox*      m_angle_platform  = nullptr;
    QCheckBox*      m_egl             = nullptr;
};

} // namespace QtAV

#endif // QTAV_WIDGETS_MISC_PAGE_H
