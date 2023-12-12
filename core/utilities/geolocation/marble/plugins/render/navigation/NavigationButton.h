// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#ifndef NAVIGATIONBUTTON_H
#define NAVIGATIONBUTTON_H

#include <QAbstractButton>

namespace Marble
{

class NavigationButton : public QAbstractButton
{
    Q_OBJECT
public:
    explicit NavigationButton( QWidget *parent = nullptr );

Q_SIGNALS:
    void repaintNeeded();

protected:
    void mousePressEvent ( QMouseEvent *mouseEvent ) override;
    void mouseReleaseEvent ( QMouseEvent *mouseEvent ) override;

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

    void enterEvent( QEnterEvent * e) override;

#else

    void enterEvent( QEvent * e) override;

#endif

    void leaveEvent( QEvent * e) override;
    void changeEvent( QEvent *e ) override;
    void paintEvent( QPaintEvent * ) override;

private:
    QIcon::Mode m_iconMode;
};

}

#endif
