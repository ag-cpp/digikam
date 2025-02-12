/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-01-28
 * Description : color label widget
 *
 * SPDX-FileCopyrightText: 2011-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "colorlabelwidget.h"

// Qt includes

#include <QApplication>
#include <QPainter>
#include <QPixmap>
#include <QIcon>
#include <QLayout>
#include <QLabel>
#include <QButtonGroup>
#include <QWidgetAction>
#include <QFontMetrics>
#include <QFont>
#include <QToolButton>

// KDE includes

#if !defined(Q_OS_DARWIN) && defined(Q_CC_GNU)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#if defined(Q_CC_CLANG)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

#include <klocalizedstring.h>
#include <kactioncollection.h>

// Restore warnings
#if !defined(Q_OS_DARWIN) && defined(Q_CC_GNU)
#   pragma GCC diagnostic pop
#endif

#if defined(Q_CC_CLANG)
#   pragma clang diagnostic pop
#endif

// Local includes

#include "dxmlguiwindow.h"
#include "dexpanderbox.h"

namespace Digikam
{

class Q_DECL_HIDDEN ColorLabelWidget::Private
{
public:

    Private() = default;

    QButtonGroup*       colorBtns   = nullptr;

    QLabel*             desc        = nullptr;

    QToolButton*        btnNone     = nullptr;
    QToolButton*        btnRed      = nullptr;
    QToolButton*        btnOrange   = nullptr;
    QToolButton*        btnYellow   = nullptr;
    QToolButton*        btnGreen    = nullptr;
    QToolButton*        btnBlue     = nullptr;
    QToolButton*        btnMagenta  = nullptr;
    QToolButton*        btnGray     = nullptr;
    QToolButton*        btnBlack    = nullptr;
    QToolButton*        btnWhite    = nullptr;

    DHBox*              descBox     = nullptr;

    DAdjustableLabel*   shortcut    = nullptr;
};

ColorLabelWidget::ColorLabelWidget(QWidget* const parent)
    : DVBox(parent),
      d    (new Private)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setFocusPolicy(Qt::NoFocus);

    DHBox* const hbox = new DHBox(this);
    hbox->setContentsMargins(QMargins());
    hbox->setSpacing(0);

    d->btnNone = new QToolButton(hbox);
    d->btnNone->setCheckable(true);
    d->btnNone->setFocusPolicy(Qt::NoFocus);
    d->btnNone->setIcon(buildIcon(NoColorLabel));
    d->btnNone->installEventFilter(this);

    d->btnRed = new QToolButton(hbox);
    d->btnRed->setCheckable(true);
    d->btnRed->setFocusPolicy(Qt::NoFocus);
    d->btnRed->setIcon(buildIcon(RedLabel));
    d->btnRed->installEventFilter(this);

    d->btnOrange = new QToolButton(hbox);
    d->btnOrange->setCheckable(true);
    d->btnOrange->setFocusPolicy(Qt::NoFocus);
    d->btnOrange->setIcon(buildIcon(OrangeLabel));
    d->btnOrange->installEventFilter(this);

    d->btnYellow = new QToolButton(hbox);
    d->btnYellow->setCheckable(true);
    d->btnYellow->setFocusPolicy(Qt::NoFocus);
    d->btnYellow->setIcon(buildIcon(YellowLabel));
    d->btnYellow->installEventFilter(this);

    d->btnGreen = new QToolButton(hbox);
    d->btnGreen->setCheckable(true);
    d->btnGreen->setFocusPolicy(Qt::NoFocus);
    d->btnGreen->setIcon(buildIcon(GreenLabel));
    d->btnGreen->installEventFilter(this);

    d->btnBlue  = new QToolButton(hbox);
    d->btnBlue->setCheckable(true);
    d->btnBlue->setFocusPolicy(Qt::NoFocus);
    d->btnBlue->setIcon(buildIcon(BlueLabel));
    d->btnBlue->installEventFilter(this);

    d->btnMagenta = new QToolButton(hbox);
    d->btnMagenta->setCheckable(true);
    d->btnMagenta->setFocusPolicy(Qt::NoFocus);
    d->btnMagenta->setIcon(buildIcon(MagentaLabel));
    d->btnMagenta->installEventFilter(this);

    d->btnGray = new QToolButton(hbox);
    d->btnGray->setCheckable(true);
    d->btnGray->setFocusPolicy(Qt::NoFocus);
    d->btnGray->setIcon(buildIcon(GrayLabel));
    d->btnGray->installEventFilter(this);

    d->btnBlack = new QToolButton(hbox);
    d->btnBlack->setCheckable(true);
    d->btnBlack->setFocusPolicy(Qt::NoFocus);
    d->btnBlack->setIcon(buildIcon(BlackLabel));
    d->btnBlack->installEventFilter(this);

    d->btnWhite = new QToolButton(hbox);
    d->btnWhite->setCheckable(true);
    d->btnWhite->setFocusPolicy(Qt::NoFocus);
    d->btnWhite->setIcon(buildIcon(WhiteLabel));
    d->btnWhite->installEventFilter(this);

    d->colorBtns = new QButtonGroup(hbox);
    d->colorBtns->addButton(d->btnNone,    NoColorLabel);
    d->colorBtns->addButton(d->btnRed,     RedLabel);
    d->colorBtns->addButton(d->btnOrange,  OrangeLabel);
    d->colorBtns->addButton(d->btnYellow,  YellowLabel);
    d->colorBtns->addButton(d->btnGreen,   GreenLabel);
    d->colorBtns->addButton(d->btnBlue,    BlueLabel);
    d->colorBtns->addButton(d->btnMagenta, MagentaLabel);
    d->colorBtns->addButton(d->btnGray,    GrayLabel);
    d->colorBtns->addButton(d->btnBlack,   BlackLabel);
    d->colorBtns->addButton(d->btnWhite,   WhiteLabel);

    d->descBox  = new DHBox(this);
    d->descBox->setContentsMargins(QMargins());
    d->descBox->setSpacing(0);
    d->desc     = new QLabel(d->descBox);
    d->shortcut = new DAdjustableLabel(d->descBox);
    QFont fnt = d->shortcut->font();
    fnt.setItalic(true);
    d->shortcut->setFont(fnt);
    d->shortcut->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    d->shortcut->setWordWrap(false);

    setSpacing(0);
    setContentsMargins(QMargins());
    setColorLabels(QList<ColorLabel>() << NoColorLabel);
    setDescriptionBoxVisible(true);
    setButtonsExclusive(true);

    // -------------------------------------------------------------

#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))

    connect(d->colorBtns, SIGNAL(idReleased(int)),
            this, SIGNAL(signalColorLabelChanged(int)));

#else

    connect(d->colorBtns, SIGNAL(buttonReleased(int)),
            this, SIGNAL(signalColorLabelChanged(int)));

#endif

}

ColorLabelWidget::~ColorLabelWidget()
{
    delete d;
}

void ColorLabelWidget::setDescriptionBoxVisible(bool b)
{
    d->descBox->setVisible(b);

    if (!b)
    {
        const auto btns = d->colorBtns->buttons();

        for (QAbstractButton* const btn : btns)
        {
            ColorLabel id = (ColorLabel)(d->colorBtns->id(btn));
            btn->setToolTip(labelColorName(id));
        }
    }
}

void ColorLabelWidget::setButtonsExclusive(bool b)
{
    d->colorBtns->setExclusive(b);
}

void ColorLabelWidget::updateDescription(ColorLabel label)
{
    d->desc->setText(labelColorName(label));

    DXmlGuiWindow* const app = dynamic_cast<DXmlGuiWindow*>(qApp->activeWindow());

    if (app)
    {
        QAction* const ac = app->actionCollection()->action(QString::fromLatin1("colorshortcut-%1").arg(label));

        if (ac)
        {
            d->shortcut->setAdjustedText(ac->shortcut().toString());
        }
    }
}

bool ColorLabelWidget::eventFilter(QObject* obj, QEvent* ev)
{
    if (obj == d->btnNone)
    {
        if (ev->type() == QEvent::Enter)
        {
            updateDescription(NoColorLabel);

            return false;
        }
    }

    if (obj == d->btnRed)
    {
        if (ev->type() == QEvent::Enter)
        {
            updateDescription(RedLabel);

            return false;
        }
    }

    if (obj == d->btnOrange)
    {
        if (ev->type() == QEvent::Enter)
        {
            updateDescription(OrangeLabel);

            return false;
        }
    }

    if (obj == d->btnYellow)
    {
        if (ev->type() == QEvent::Enter)
        {
            updateDescription(YellowLabel);

            return false;
        }
    }

    if (obj == d->btnGreen)
    {
        if (ev->type() == QEvent::Enter)
        {
            updateDescription(GreenLabel);

            return false;
        }
    }

    if (obj == d->btnBlue)
    {
        if (ev->type() == QEvent::Enter)
        {
            updateDescription(BlueLabel);

            return false;
        }
    }

    if (obj == d->btnMagenta)
    {
        if (ev->type() == QEvent::Enter)
        {
            updateDescription(MagentaLabel);

            return false;
        }
    }

    if (obj == d->btnGray)
    {
        if (ev->type() == QEvent::Enter)
        {
            updateDescription(GrayLabel);

            return false;
        }
    }

    if (obj == d->btnBlack)
    {
        if (ev->type() == QEvent::Enter)
        {
            updateDescription(BlackLabel);

            return false;
        }
    }

    if (obj == d->btnWhite)
    {
        if (ev->type() == QEvent::Enter)
        {
            updateDescription(WhiteLabel);

            return false;
        }
    }

    // pass the event on to the parent class

    return DVBox::eventFilter(obj, ev);
}

void ColorLabelWidget::setColorLabels(const QList<ColorLabel>& list)
{
    const auto btns = d->colorBtns->buttons();

    for (QAbstractButton* const btn : btns)
    {
        ColorLabel id = (ColorLabel)(d->colorBtns->id(btn));
        btn->setChecked(list.contains(id));
        updateDescription(id);
    }
}

QList<ColorLabel> ColorLabelWidget::colorLabels() const
{
    QList<ColorLabel> list;

    const auto btns = d->colorBtns->buttons();

    for (QAbstractButton* const btn : btns)
    {
        if (btn && btn->isChecked())
        {
            list.append((ColorLabel)(d->colorBtns->id(btn)));
        }
    }

    return list;
}

QIcon ColorLabelWidget::buildIcon(ColorLabel label, int size)
{
    if (label != NoColorLabel)
    {
        QPixmap pix(size, size);
        QPainter p(&pix);
        p.setPen(qApp->palette().color(QPalette::Active, QPalette::ButtonText));
        p.fillRect(0, 0, pix.width()-1, pix.height()-1, labelColor(label));
        p.drawRect(0, 0, pix.width()-1, pix.height()-1);

        return QIcon(pix);
    }

    return QIcon::fromTheme(QLatin1String("emblem-unmounted"));
}

QColor ColorLabelWidget::labelColor(ColorLabel label)
{
    QColor color;

    switch (label)
    {
        case RedLabel:
        {
            color = qRgb(0xDF, 0x6E, 0x5F);
            break;
        }

        case OrangeLabel:
        {
            color = qRgb(0xEE, 0xAF, 0x6B);
            break;
        }

        case YellowLabel:
        {
            color = qRgb(0xE4, 0xD3, 0x78);
            break;
        }

        case GreenLabel:
        {
            color = qRgb(0xAF, 0xD8, 0x78);
            break;
        }

        case BlueLabel:
        {
            color = qRgb(0x77, 0xBA, 0xE8);
            break;
        }

        case MagentaLabel:
        {
            color = qRgb(0xCB, 0x98, 0xE1);
            break;
        }

        case GrayLabel:
        {
            color = qRgb(0xB7, 0xB7, 0xB7);
            break;
        }

        case BlackLabel:
        {
            color = qRgb(0x28, 0x28, 0x28);
            break;
        }

        case WhiteLabel:
        {
            color = qRgb(0xF7, 0xFE, 0xFA);
            break;
        }

        default:   // NoColorLabel
        {
            break;
        }
    }

    return color;
}

QString ColorLabelWidget::labelColorName(ColorLabel label)
{
    QString name;

    switch (label)
    {
        case RedLabel:
        {
            name = i18nc("@info: color label name", "Red");
            break;
        }

        case OrangeLabel:
        {
            name = i18nc("@info: color label name", "Orange");
            break;
        }

        case YellowLabel:
        {
            name = i18nc("@info: color label name", "Yellow");
            break;
        }

        case GreenLabel:
        {
            name = i18nc("@info: color label name", "Green");
            break;
        }

        case BlueLabel:
        {
            name = i18nc("@info: color label name", "Blue");
            break;
        }

        case MagentaLabel:
        {
            name = i18nc("@info: color label name", "Magenta");
            break;
        }

        case GrayLabel:
        {
            name = i18nc("@info: color label name", "Gray");
            break;
        }

        case BlackLabel:
        {
            name = i18nc("@info: color label name", "Black");
            break;
        }

        case WhiteLabel:
        {
            name = i18nc("@info: color label name", "White");
            break;
        }

        default:   // NoColorLabel
        {
            name = i18nc("@info: color label name", "None");
            break;
        }
    }

    return name;
}

// -----------------------------------------------------------------------------

class Q_DECL_HIDDEN ColorLabelSelector::Private
{

public:

    Private() = default;

    ColorLabelWidget* clw = nullptr;
};

ColorLabelSelector::ColorLabelSelector(QWidget* parent)
    : QPushButton(parent),
      d          (new Private)
{
    QMenu* const popup          = new QMenu(this);
    setMenu(popup);

    QWidgetAction* const action = new QWidgetAction(this);
    d->clw                      = new ColorLabelWidget(this);
    action->setDefaultWidget(d->clw);
    popup->addAction(action);
    slotColorLabelChanged(NoColorLabel);

    connect(d->clw, SIGNAL(signalColorLabelChanged(int)),
            this, SLOT(slotColorLabelChanged(int)));
}

ColorLabelSelector::~ColorLabelSelector()
{
    delete d;
}

ColorLabelWidget* ColorLabelSelector::colorLabelWidget() const
{
    return d->clw;
}

void ColorLabelSelector::setColorLabel(ColorLabel label)
{
    d->clw->setColorLabels(QList<ColorLabel>() << label);
    slotColorLabelChanged(label);
}

ColorLabel ColorLabelSelector::colorLabel()
{
    QList<ColorLabel> list = d->clw->colorLabels();

    if (!list.isEmpty())
    {
        return list.first();
    }

    return NoColorLabel;
}

void ColorLabelSelector::slotColorLabelChanged(int id)
{
    setText(QString());
    setIcon(d->clw->buildIcon((ColorLabel)id));
    setToolTip(i18nc("@info: color label selector", "Color Label: %1", d->clw->labelColorName((ColorLabel)id)));
    menu()->close();

    Q_EMIT signalColorLabelChanged(id);
}

// -----------------------------------------------------------------------------

ColorLabelMenuAction::ColorLabelMenuAction(QMenu* const parent)
    : QMenu(parent)
{
    setTitle(i18nc("@title: color label menu", "Color"));
    QWidgetAction* const wa     = new QWidgetAction(this);
    ColorLabelWidget* const clw = new ColorLabelWidget(parent);
    wa->setDefaultWidget(clw);
    addAction(wa);

    connect(clw, SIGNAL(signalColorLabelChanged(int)),
            this, SIGNAL(signalColorLabelChanged(int)));

    connect(clw, SIGNAL(signalColorLabelChanged(int)),
            parent, SLOT(close()));
}

ColorLabelMenuAction::~ColorLabelMenuAction()
{
}

} // namespace Digikam

#include "moc_colorlabelwidget.cpp"
