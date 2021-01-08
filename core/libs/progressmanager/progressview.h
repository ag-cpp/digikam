/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-01-13
 * Description : progress manager
 *
 * Copyright (C) 2007-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012      by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2004      by Till Adam <adam at kde dot org>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_PROGRESS_VIEW_H
#define DIGIKAM_PROGRESS_VIEW_H

// Qt includes

#include <QScrollArea>

// Local includes

#include "overlaywidget.h"
#include "digikam_export.h"

namespace Digikam
{
class ProgressItem;

class TransactionItem : public DVBox
{
    Q_OBJECT

public:

    explicit TransactionItem(QWidget* const parent, ProgressItem* const item, bool first);
    ~TransactionItem() override;

    void hideHLine();

    void setProgress(int progress);
    void setLabel(const QString&);
    void setThumbnail(const QPixmap&);

    // NOTE: the given text is interpreted as RichText, so you might need to
    // use .toHtmlEscaped() it before passing
    void setStatus(const QString&);

    void setTotalSteps( int totalSteps );

    ProgressItem* item() const;

    void addSubTransaction(ProgressItem* const item);

    // The progressitem is deleted immediately, we take 5s to go out,
    // so better not use mItem during this time.
    void setItemComplete();

public Q_SLOTS:

    void slotItemCanceled();

private:

    class Private;
    Private* const d;
};

// --------------------------------------------------------------------------------

class TransactionItemView : public QScrollArea
{
    Q_OBJECT

public:

    explicit TransactionItemView(QWidget* const parent=nullptr, const QString& name=QString());
    ~TransactionItemView() override {}

    TransactionItem* addTransactionItem(ProgressItem* item, bool first);

    QSize sizeHint()        const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:

    void slotLayoutFirstItem();

Q_SIGNALS:

    void signalTransactionViewIsEmpty();

protected:

    void resizeEvent(QResizeEvent* event) override;

private:

    DVBox* m_bigBox;
};

// --------------------------------------------------------------------------------

class DIGIKAM_EXPORT ProgressView : public OverlayWidget
{
    Q_OBJECT

public:

    explicit ProgressView(QWidget* const alignWidget, QWidget* const parent, const QString& name = QString());
    ~ProgressView() override;

    void setVisible(bool b) override;

public Q_SLOTS:

    void slotToggleVisibility();

Q_SIGNALS:

    void visibilityChanged(bool);

protected Q_SLOTS:

    void slotTransactionAdded(ProgressItem*);
    void slotTransactionCompleted(ProgressItem*);
    void slotTransactionCanceled(ProgressItem*);
    void slotTransactionProgress(ProgressItem*, unsigned int progress);
    void slotTransactionStatus(ProgressItem*, const QString&);
    void slotTransactionLabel(ProgressItem*, const QString&);
    void slotTransactionUsesBusyIndicator(ProgressItem*, bool);
    void slotTransactionThumbnail(ProgressItem*, const QPixmap&);
    void slotClose();
    void slotShow();
    void slotHide();

protected:

    void closeEvent(QCloseEvent*) override;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_PROGRESS_VIEW_H
