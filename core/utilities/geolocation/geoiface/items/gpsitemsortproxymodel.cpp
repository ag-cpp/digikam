/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-03-21
 * Description : A model to hold GPS information about items.
 *
 * SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2010      by Michael G. Hansen <mike at mghansen dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "gpsitemsortproxymodel.h"

// Qt includes

#include <QPointer>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN GPSItemSortProxyModel::Private
{
public:

    Private() = default;

    GPSItemModel*              imageModel               = nullptr;
    QItemSelectionModel*       sourceSelectionModel     = nullptr;
    GPSLinkItemSelectionModel* linkItemSelectionModel   = nullptr;
};

GPSItemSortProxyModel::GPSItemSortProxyModel(GPSItemModel* const imageModel,
                                             QItemSelectionModel* const sourceSelectionModel)
    : QSortFilterProxyModel(imageModel),
      d                    (new Private)
{
    d->imageModel             = imageModel;
    d->sourceSelectionModel   = sourceSelectionModel;
    setSourceModel(imageModel);
    d->linkItemSelectionModel = new GPSLinkItemSelectionModel(this, d->sourceSelectionModel);
}

GPSItemSortProxyModel::~GPSItemSortProxyModel()
{
    delete d;
}

bool GPSItemSortProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    if ((!left.isValid()) || (!right.isValid()))
    {
/*
        qCDebug(DIGIKAM_GENERAL_LOG) << "INVALID INDICES" << left << right;
*/
        return false;
    }

    const int column                        = left.column();
    const GPSItemContainer* const itemLeft  = d->imageModel->itemFromIndex(left);
    const GPSItemContainer* const itemRight = d->imageModel->itemFromIndex(right);
/*
    qCDebug(DIGIKAM_GENERAL_LOG) << itemLeft << itemRight << column << rowCount() << d->imageModel->rowCount();
*/
    return itemLeft->lessThan(itemRight, column);
}

QItemSelectionModel* GPSItemSortProxyModel::mappedSelectionModel() const
{
    return d->linkItemSelectionModel;
}

// --------------------------------------------------------------------------------------

class Q_DECL_HIDDEN GPSLinkItemSelectionModelPrivate
{
public:

    explicit GPSLinkItemSelectionModelPrivate(GPSLinkItemSelectionModel* const proxySelectionModel)
        : q_ptr(proxySelectionModel)
    {
        QObject::connect(q_ptr, &QItemSelectionModel::currentChanged, q_ptr,
                         [this](const QModelIndex& idx)
            {
                slotCurrentChanged(idx);
            }
        );

        QObject::connect(q_ptr, &QItemSelectionModel::modelChanged, q_ptr,
                         [this]
            {
                reinitializeIndexMapper();
            }
        );
    }

public:

    Q_DECLARE_PUBLIC(GPSLinkItemSelectionModel)
    GPSLinkItemSelectionModel* const q_ptr = nullptr;

public:

    bool assertSelectionValid(const QItemSelection& selection) const
    {
        for (const QItemSelectionRange& range : std::as_const(selection))
        {
            if (!range.isValid())
            {
                qCDebug(DIGIKAM_GENERAL_LOG) << selection;
            }

            Q_ASSERT(range.isValid());
        }

        return true;
    }

    void reinitializeIndexMapper()
    {
        delete m_indexMapper;
        m_indexMapper = nullptr;

        if (!q_ptr->model() || !m_linkedItemSelectionModel || !m_linkedItemSelectionModel->model())
        {
            return;
        }

        m_indexMapper                        = new GPSModelIndexProxyMapper(q_ptr->model(), m_linkedItemSelectionModel->model(), q_ptr);
        const QItemSelection mappedSelection = m_indexMapper->mapSelectionRightToLeft(m_linkedItemSelectionModel->selection());
        q_ptr->QItemSelectionModel::select(mappedSelection, QItemSelectionModel::ClearAndSelect);
    }

    void sourceSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
    void sourceCurrentChanged(const QModelIndex& current);
    void slotCurrentChanged(const QModelIndex& current);

public:

    QItemSelectionModel*      m_linkedItemSelectionModel    = nullptr;
    bool                      m_ignoreCurrentChanged        = false;
    GPSModelIndexProxyMapper* m_indexMapper                 = nullptr;
};

GPSLinkItemSelectionModel::GPSLinkItemSelectionModel(QAbstractItemModel* const model,
                                                     QItemSelectionModel* const proxySelector,
                                                     QObject* const parent)
    : QItemSelectionModel(model, parent),
      d_ptr              (new GPSLinkItemSelectionModelPrivate(this))
{
    setLinkedItemSelectionModel(proxySelector);
}

GPSLinkItemSelectionModel::GPSLinkItemSelectionModel(QObject* const parent)
    : QItemSelectionModel(nullptr, parent),
      d_ptr              (new GPSLinkItemSelectionModelPrivate(this))
{
}

GPSLinkItemSelectionModel::~GPSLinkItemSelectionModel()
{
    delete d_ptr;
}

QItemSelectionModel* GPSLinkItemSelectionModel::linkedItemSelectionModel() const
{
    Q_D(const GPSLinkItemSelectionModel);

    return d->m_linkedItemSelectionModel;
}

void GPSLinkItemSelectionModel::setLinkedItemSelectionModel(QItemSelectionModel* const selectionModel)
{
    Q_D(GPSLinkItemSelectionModel);

    if (d->m_linkedItemSelectionModel != selectionModel)
    {
        if (d->m_linkedItemSelectionModel)
        {
            disconnect(d->m_linkedItemSelectionModel);
        }

        d->m_linkedItemSelectionModel = selectionModel;

        if (d->m_linkedItemSelectionModel)
        {
            connect(d->m_linkedItemSelectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                    this, SLOT(sourceSelectionChanged(QItemSelection,QItemSelection)));

            connect(d->m_linkedItemSelectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                    this, SLOT(sourceCurrentChanged(QModelIndex)));

            connect(d->m_linkedItemSelectionModel, &QItemSelectionModel::modelChanged,
                    this, [this] { d_ptr->reinitializeIndexMapper(); });
        }

        d->reinitializeIndexMapper();

        Q_EMIT linkedItemSelectionModelChanged();
    }
}

void GPSLinkItemSelectionModel::select(const QModelIndex& index, QItemSelectionModel::SelectionFlags command)
{
    Q_D(GPSLinkItemSelectionModel);

    // When an item is removed, the current index is set to the top index in the model.
    // That causes a selectionChanged signal with a selection which we do not want.

    if (d->m_ignoreCurrentChanged)
    {
        return;
    }

    // Do *not* replace next line with: QItemSelectionModel::select(index, command)
    //
    // Doing so would end up calling GPSLinkItemSelectionModel::select(QItemSelection, QItemSelectionModel::SelectionFlags)
    //
    // This is because the code for QItemSelectionModel::select(QModelIndex, QItemSelectionModel::SelectionFlags) looks like this:
    // {
    //     QItemSelection selection(index, index);
    //     select(selection, command);
    // }
    // So it calls GPSLinkItemSelectionModel overload of
    // select(QItemSelection, QItemSelectionModel::SelectionFlags)
    //
    // When this happens and the selection flags include Toggle, it causes the
    // selection to be toggled twice.

    QItemSelectionModel::select(QItemSelection(index, index), command);

    if (index.isValid())
    {
        d->m_linkedItemSelectionModel->select(d->m_indexMapper->mapSelectionLeftToRight(QItemSelection(index, index)), command);
    }
    else
    {
        d->m_linkedItemSelectionModel->clearSelection();
    }
}

void GPSLinkItemSelectionModel::select(const QItemSelection& selection, QItemSelectionModel::SelectionFlags command)
{
    Q_D(GPSLinkItemSelectionModel);

    d->m_ignoreCurrentChanged      = true;
    QItemSelection _selection      = selection;
    QItemSelectionModel::select(_selection, command);

    Q_ASSERT(d->assertSelectionValid(_selection));

    QItemSelection mappedSelection = d->m_indexMapper->mapSelectionLeftToRight(_selection);

    Q_ASSERT(d->assertSelectionValid(mappedSelection));

    d->m_linkedItemSelectionModel->select(mappedSelection, command);
    d->m_ignoreCurrentChanged      = false;
}

void GPSLinkItemSelectionModelPrivate::slotCurrentChanged(const QModelIndex& current)
{
    const QModelIndex mappedCurrent = m_indexMapper->mapLeftToRight(current);

    if (!mappedCurrent.isValid())
    {
        return;
    }

    m_linkedItemSelectionModel->setCurrentIndex(mappedCurrent, QItemSelectionModel::NoUpdate);
}

void GPSLinkItemSelectionModelPrivate::sourceSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
    Q_Q(GPSLinkItemSelectionModel);

    QItemSelection _selected               = selected;
    QItemSelection _deselected             = deselected;

    Q_ASSERT(assertSelectionValid(_selected));
    Q_ASSERT(assertSelectionValid(_deselected));

    const QItemSelection mappedDeselection = m_indexMapper->mapSelectionRightToLeft(_deselected);
    const QItemSelection mappedSelection   = m_indexMapper->mapSelectionRightToLeft(_selected);

    q->QItemSelectionModel::select(mappedDeselection, QItemSelectionModel::Deselect);
    q->QItemSelectionModel::select(mappedSelection, QItemSelectionModel::Select);
}

void GPSLinkItemSelectionModelPrivate::sourceCurrentChanged(const QModelIndex& current)
{
    Q_Q(GPSLinkItemSelectionModel);

    const QModelIndex mappedCurrent = m_indexMapper->mapRightToLeft(current);

    if (!mappedCurrent.isValid())
    {
        return;
    }

    q->setCurrentIndex(mappedCurrent, QItemSelectionModel::NoUpdate);
}

// --------------------------------------------------------------------------------------

class Q_DECL_HIDDEN GPSModelIndexProxyMapperPrivate
{
public:

    explicit GPSModelIndexProxyMapperPrivate(const QAbstractItemModel* const leftModel,
                                             const QAbstractItemModel* const rightModel,
                                             GPSModelIndexProxyMapper* const qq)
        : q_ptr       (qq),
          m_leftModel (leftModel),
          m_rightModel(rightModel)
    {
        createProxyChain();
    }

    void createProxyChain();
    void checkConnected();
    void setConnected(bool connected);

    // cppcheck-suppress unusedPrivateFunction
    bool assertSelectionValid(const QItemSelection& selection) const
    {
        for (const QItemSelectionRange& range : std::as_const(selection))
        {
            if (!range.isValid())
            {
                qCDebug(DIGIKAM_GENERAL_LOG) << selection << m_leftModel << m_rightModel << m_proxyChainDown << m_proxyChainUp;
            }

            Q_ASSERT(range.isValid());
        }

        return true;
    }

public:

    Q_DECLARE_PUBLIC(GPSModelIndexProxyMapper)
    GPSModelIndexProxyMapper* const             q_ptr       = nullptr;

    QList<QPointer<const QAbstractProxyModel> > m_proxyChainUp;
    QList<QPointer<const QAbstractProxyModel> > m_proxyChainDown;

    QPointer<const QAbstractItemModel>          m_leftModel;
    QPointer<const QAbstractItemModel>          m_rightModel;

    bool                                        mConnected  = false;
};

/**
 * The idea here is that <tt>this</tt> selection model and proxySelectionModel might be in different parts of the
 * proxy chain. We need to build up to two chains of proxy models to create mappings between them.
 *
 * We first build the chain from 1 to 5, then start building the chain from 7 to 1. We stop when we find that proxy 2 is
 * already in the first chain.
 */
void GPSModelIndexProxyMapperPrivate::createProxyChain()
{
    for (auto p : std::as_const(m_proxyChainUp))
    {
        p->disconnect(q_ptr);
    }

    for (auto p : std::as_const(m_proxyChainDown))
    {
        p->disconnect(q_ptr);
    }

    m_proxyChainUp.clear();
    m_proxyChainDown.clear();
    QPointer<const QAbstractItemModel> targetModel                = m_rightModel;

    QList<QPointer<const QAbstractProxyModel> > proxyChainDown;
    QPointer<const QAbstractProxyModel> selectionTargetProxyModel = qobject_cast<const QAbstractProxyModel *>(targetModel);

    while (selectionTargetProxyModel)
    {
        proxyChainDown.prepend(selectionTargetProxyModel);

        QObject::connect(selectionTargetProxyModel.data(), &QAbstractProxyModel::sourceModelChanged,
                         q_ptr, [this]
            {
                createProxyChain();
            }
        );

        selectionTargetProxyModel = qobject_cast<const QAbstractProxyModel*>(selectionTargetProxyModel->sourceModel());

        if (selectionTargetProxyModel == m_leftModel)
        {
            m_proxyChainDown = proxyChainDown;
            checkConnected();

            return;
        }
    }

    QPointer<const QAbstractItemModel> sourceModel       = m_leftModel;
    QPointer<const QAbstractProxyModel> sourceProxyModel = qobject_cast<const QAbstractProxyModel *>(sourceModel);

    while (sourceProxyModel)
    {
        m_proxyChainUp.append(sourceProxyModel);

        QObject::connect(sourceProxyModel.data(), &QAbstractProxyModel::sourceModelChanged,
                         q_ptr, [this]
            {
                createProxyChain();
            }
        );

        sourceProxyModel      = qobject_cast<const QAbstractProxyModel *>(sourceProxyModel->sourceModel());
        const int targetIndex = proxyChainDown.indexOf(sourceProxyModel);

        if (targetIndex != -1)
        {
            m_proxyChainDown = proxyChainDown.mid(targetIndex + 1, proxyChainDown.size());
            checkConnected();

            return;
        }
    }

    m_proxyChainDown = proxyChainDown;
    checkConnected();
}

void GPSModelIndexProxyMapperPrivate::checkConnected()
{
    auto konamiRight = m_proxyChainUp.isEmpty()   ? m_leftModel  : m_proxyChainUp.last()->sourceModel();
    auto konamiLeft  = m_proxyChainDown.isEmpty() ? m_rightModel : m_proxyChainDown.first()->sourceModel();
    setConnected(konamiLeft && (konamiLeft == konamiRight));
}

void GPSModelIndexProxyMapperPrivate::setConnected(bool connected)
{
    if (mConnected != connected)
    {
        Q_Q(GPSModelIndexProxyMapper);

        mConnected = connected;

        Q_EMIT q->isConnectedChanged();
    }
}

GPSModelIndexProxyMapper::GPSModelIndexProxyMapper(const QAbstractItemModel* const leftModel,
                                                   const QAbstractItemModel* const rightModel,
                                                   QObject* const parent)
    : QObject(parent),
      d_ptr  (new GPSModelIndexProxyMapperPrivate(leftModel, rightModel, this))
{
}

GPSModelIndexProxyMapper::~GPSModelIndexProxyMapper()
{
    delete d_ptr;
}

QModelIndex GPSModelIndexProxyMapper::mapLeftToRight(const QModelIndex& index) const
{
    const QItemSelection selection = mapSelectionLeftToRight(QItemSelection(index, index));

    if (selection.isEmpty())
    {
        return QModelIndex();
    }

    return selection.indexes().first();
}

QModelIndex GPSModelIndexProxyMapper::mapRightToLeft(const QModelIndex& index) const
{
    const QItemSelection selection = mapSelectionRightToLeft(QItemSelection(index, index));

    if (selection.isEmpty())
    {
        return QModelIndex();
    }

    return selection.indexes().first();
}

QItemSelection GPSModelIndexProxyMapper::mapSelectionLeftToRight(const QItemSelection& selection) const
{
    Q_D(const GPSModelIndexProxyMapper);

    if (selection.isEmpty() || !d->mConnected)
    {
        return QItemSelection();
    }

    if (selection.first().model() != d->m_leftModel)
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "FAIL" << selection.first().model() << d->m_leftModel << d->m_rightModel;
    }

    Q_ASSERT(selection.first().model() == d->m_leftModel);

    QItemSelection seekSelection = selection;

    Q_ASSERT(d->assertSelectionValid(seekSelection));

    QListIterator<QPointer<const QAbstractProxyModel> > iUp(d->m_proxyChainUp);

    while (iUp.hasNext())
    {
        const QPointer<const QAbstractProxyModel> proxy = iUp.next();

        if (!proxy)
        {
            return QItemSelection();
        }

        Q_ASSERT(seekSelection.isEmpty() || (seekSelection.first().model() == proxy));

        seekSelection = proxy->mapSelectionToSource(seekSelection);

        Q_ASSERT(seekSelection.isEmpty() || (seekSelection.first().model() == proxy->sourceModel()));

        Q_ASSERT(d->assertSelectionValid(seekSelection));
    }

    QListIterator<QPointer<const QAbstractProxyModel> > iDown(d->m_proxyChainDown);

    while (iDown.hasNext())
    {
        const QPointer<const QAbstractProxyModel> proxy = iDown.next();

        if (!proxy)
        {
            return QItemSelection();
        }

        Q_ASSERT(seekSelection.isEmpty() || (seekSelection.first().model() == proxy->sourceModel()));

        seekSelection = proxy->mapSelectionFromSource(seekSelection);

        Q_ASSERT(seekSelection.isEmpty() || (seekSelection.first().model() == proxy));

        Q_ASSERT(d->assertSelectionValid(seekSelection));
    }

    Q_ASSERT((!seekSelection.isEmpty() && (seekSelection.first().model() == d->m_rightModel)) || true);

    return seekSelection;
}

QItemSelection GPSModelIndexProxyMapper::mapSelectionRightToLeft(const QItemSelection& selection) const
{
    Q_D(const GPSModelIndexProxyMapper);

    if (selection.isEmpty() || !d->mConnected)
    {
        return QItemSelection();
    }

    if (selection.first().model() != d->m_rightModel)
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "FAIL" << selection.first().model() << d->m_leftModel << d->m_rightModel;
    }

    Q_ASSERT(selection.first().model() == d->m_rightModel);

    QItemSelection seekSelection = selection;

    Q_ASSERT(d->assertSelectionValid(seekSelection));

    QListIterator<QPointer<const QAbstractProxyModel> > iDown(d->m_proxyChainDown);
    iDown.toBack();

    while (iDown.hasPrevious())
    {
        const QPointer<const QAbstractProxyModel> proxy = iDown.previous();

        if (!proxy)
        {
            return QItemSelection();
        }

        seekSelection = proxy->mapSelectionToSource(seekSelection);

        Q_ASSERT(d->assertSelectionValid(seekSelection));
    }

    QListIterator<QPointer<const QAbstractProxyModel> > iUp(d->m_proxyChainUp);

    iUp.toBack();

    while (iUp.hasPrevious())
    {
        const QPointer<const QAbstractProxyModel> proxy = iUp.previous();

        if (!proxy)
        {
            return QItemSelection();
        }

        seekSelection = proxy->mapSelectionFromSource(seekSelection);

        Q_ASSERT(d->assertSelectionValid(seekSelection));
    }

    Q_ASSERT((!seekSelection.isEmpty() && (seekSelection.first().model() == d->m_leftModel)) || true);

    return seekSelection;
}

bool GPSModelIndexProxyMapper::isConnected() const
{
    Q_D(const GPSModelIndexProxyMapper);

    return d->mConnected;
}

} // namespace Digikam

#include "moc_gpsitemsortproxymodel.cpp"
