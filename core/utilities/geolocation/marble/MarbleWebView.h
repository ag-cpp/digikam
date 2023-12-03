// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#ifndef MARBLEWEBVIEW_H
#define MARBLEWEBVIEW_H

#include <QWebEngineView>
#include <QWebEnginePage>
#include <QPaintEvent>

#include "digikam_export.h"

class DIGIKAM_EXPORT MarbleWebPage : public QWebEnginePage
{
    Q_OBJECT
public:
    explicit MarbleWebPage(QObject *parent = nullptr)  : QWebEnginePage(parent){}

Q_SIGNALS:
    void linkClicked(const QUrl & url);
protected:
    bool acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame) override {
        Q_UNUSED(isMainFrame)
        if (type == QWebEnginePage::NavigationTypeLinkClicked) {
            Q_EMIT linkClicked(url);
            return false;
        }
        return true;
    }
};

class DIGIKAM_EXPORT MarbleWebView : public QWebEngineView
{
    Q_OBJECT
public:
    explicit MarbleWebView(QWidget *parent = nullptr);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private Q_SLOTS:
    void copySelectedText();

private:
    QMenu *m_contextMenu;
    QAction *m_copyAction;
};

#endif // MARBLEWEBVIEW_H
