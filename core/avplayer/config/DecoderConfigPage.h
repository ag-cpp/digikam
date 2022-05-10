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

#ifndef AV_PLAYER_DECODER_CONFIG_PAGE_H
#define AV_PLAYER_DECODER_CONFIG_PAGE_H

// Qt includes

#include <QVariant>
#include <QWidget>

// Local includes

#include "ConfigPageBase.h"

QT_BEGIN_NAMESPACE
class QListWidget;
class QToolButton;
class QSpinBox;
class QVBoxLayout;
QT_END_NAMESPACE

namespace AVPlayer
{

class DecoderConfigPage : public ConfigPageBase
{
    Q_OBJECT

    class DecoderItemWidget;

public:

    explicit DecoderConfigPage(QWidget* const parent = nullptr);
    virtual QString name()             const override;

    QVariantHash audioDecoderOptions() const;
    QVariantHash videoDecoderOptions() const;

protected:

    virtual void applyToUi()                 override;
    virtual void applyFromUi()               override;

private Q_SLOTS:

    void videoDecoderEnableChanged();
    void priorityUp();
    void priorityDown();
    void onDecSelected(DecoderItemWidget* iw);
    void updateDecodersUi();
    void onConfigChanged();

private:

    QSpinBox*                   mpThreads;
    QToolButton*                mpUp;
    QToolButton*                mpDown;
    QList<DecoderItemWidget*>   mDecItems;
    DecoderItemWidget*          mpSelectedDec;
    QVBoxLayout*                mpDecLayout;
};

} // namespace AVPlayer

#endif // AV_PLAYER_DECODER_CONFIG_PAGE_H
