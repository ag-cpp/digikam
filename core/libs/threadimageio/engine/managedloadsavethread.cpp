/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-01-20
 * Description : image file IO threaded interface.
 *
 * SPDX-FileCopyrightText: 2005-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2005-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "managedloadsavethread.h"

// Local includes

#include "digikam_debug.h"
#include "loadsavetask.h"
#include "previewtask.h"
#include "thumbnailtask.h"

namespace Digikam
{

ManagedLoadSaveThread::ManagedLoadSaveThread(QObject* const parent)
    : LoadSaveThread(parent)
{
}

ManagedLoadSaveThread::~ManagedLoadSaveThread()
{
    shutDown();
}

void ManagedLoadSaveThread::shutDown()
{
    switch (m_terminationPolicy)
    {
        case TerminationPolicyTerminateLoading:
        {
            QMutexLocker lock(threadMutex());
            LoadingTask* loadingTask = nullptr;

            if ((loadingTask = checkLoadingTask(m_currentTask, LoadingTaskFilterAll)))
            {
                loadingTask->setStatus(LoadingTask::LoadingTaskStatusStopping);
            }

            removeLoadingTasks(LoadingDescription(QString()), LoadingTaskFilterAll);
            break;
        }

        case TerminationPolicyTerminatePreloading:
        {
            QMutexLocker lock(threadMutex());
            LoadingTask* loadingTask = nullptr;

            if ((loadingTask = checkLoadingTask(m_currentTask, LoadingTaskFilterPreloading)))
            {
                loadingTask->setStatus(LoadingTask::LoadingTaskStatusStopping);
            }

            removeLoadingTasks(LoadingDescription(QString()), LoadingTaskFilterPreloading);
            break;
        }

        case TerminationPolicyWait:
        {
            break;
        }

        case TerminationPolicyTerminateAll:
        {
            stopAllTasks();
            break;
        }
    }

    LoadSaveThread::shutDown();
}

LoadingTask* ManagedLoadSaveThread::checkLoadingTask(LoadSaveTask* const task, LoadingTaskFilter filter) const
{
    if (task && (task->type() == LoadSaveTask::TaskTypeLoading))
    {
        LoadingTask* const loadingTask = dynamic_cast<LoadingTask*>(task);

        if      (filter == LoadingTaskFilterAll)
        {
            return loadingTask;
        }
        else if (filter == LoadingTaskFilterPreloading)
        {
            if (loadingTask && (loadingTask->status() == LoadingTask::LoadingTaskStatusPreloading))
            {
                return loadingTask;
            }
        }
    }

    return nullptr;
}

LoadingTask* ManagedLoadSaveThread::findExistingTask(const LoadingDescription& loadingDescription) const
{
    LoadingTask* loadingTask = nullptr;

    if (m_currentTask)
    {
        if (m_currentTask->type() == LoadSaveTask::TaskTypeLoading)
        {
            loadingTask = dynamic_cast<LoadingTask*>(m_currentTask);

            if (loadingTask)
            {
                const LoadingDescription& taskDescription = loadingTask->loadingDescription();

                if (taskDescription == loadingDescription)
                {
                    return loadingTask;
                }
            }
        }
    }

    for (int i = 0 ; i < m_todo.size() ; ++i)
    {
        LoadSaveTask* const task = m_todo.at(i);

        if (task->type() == LoadSaveTask::TaskTypeLoading)
        {
            loadingTask = dynamic_cast<LoadingTask*>(task);

            if (loadingTask && (loadingTask->loadingDescription() == loadingDescription))
            {
                return loadingTask;
            }
        }
    }

    return nullptr;
}

void ManagedLoadSaveThread::setTerminationPolicy(TerminationPolicy terminationPolicy)
{
    m_terminationPolicy = terminationPolicy;
}

ManagedLoadSaveThread::TerminationPolicy ManagedLoadSaveThread::terminationPolicy() const
{
    return m_terminationPolicy;
}

void ManagedLoadSaveThread::setLoadingPolicy(LoadingPolicy policy)
{
    m_loadingPolicy = policy;
}

ManagedLoadSaveThread::LoadingPolicy ManagedLoadSaveThread::loadingPolicy() const
{
    return m_loadingPolicy;
}

void ManagedLoadSaveThread::load(const LoadingDescription& description, LoadingPolicy policy)
{
    load(description, LoadingModeNormal, policy);
}

void ManagedLoadSaveThread::load(const LoadingDescription& description)
{
    load(description, LoadingModeNormal, m_loadingPolicy);
}

void ManagedLoadSaveThread::load(const LoadingDescription& description, LoadingMode loadingMode,
                                 AccessMode accessMode)
{
    load(description, loadingMode, m_loadingPolicy, accessMode);
}

void ManagedLoadSaveThread::load(const LoadingDescription& description, LoadingMode loadingMode,
                                 LoadingPolicy policy, AccessMode accessMode)
{
    QMutexLocker lock(threadMutex());
    LoadingTask* loadingTask  = nullptr;
    LoadingTask* existingTask = nullptr;

    if ((policy != LoadingPolicySimplePrepend) && (policy != LoadingPolicySimpleAppend))
    {
        existingTask = findExistingTask(description);
    }

    //qCDebug(DIGIKAM_GENERAL_LOG) << "ManagedLoadSaveThread::load " << description.filePath << ", policy " << policy;

    switch (policy)
    {
        case LoadingPolicyFirstRemovePrevious:
        {
            // reuse task if it exists

            if (existingTask)
            {
                existingTask->setStatus(LoadingTask::LoadingTaskStatusLoading);
            }

            // stop current task

            if ((loadingTask = checkLoadingTask(m_currentTask, LoadingTaskFilterAll)))
            {
                if (loadingTask != existingTask)
                {
                    loadingTask->setStatus(LoadingTask::LoadingTaskStatusStopping);
                }
            }
/*
            qCDebug(DIGIKAM_GENERAL_LOG) << "LoadingPolicyFirstRemovePrevious, Existing task " << existingTask
                                         << ", m_currentTask " << m_currentTask << ", loadingTask " << loadingTask;
*/
            // remove all loading tasks

            for (int i = 0 ; i < m_todo.size() ; ++i)
            {
                LoadingTask* task = nullptr;

                if ((task = checkLoadingTask(m_todo.at(i), LoadingTaskFilterAll)))
                {
                    if (task != existingTask)
                    {
/*
                        qCDebug(DIGIKAM_GENERAL_LOG) << "Removing task " << task << " from list";
*/
                        delete m_todo.takeAt(i--);
                    }
                }
            }

            // append new, exclusive loading task

            if (existingTask)
            {
                break;
            }

            m_todo.append(createLoadingTask(description, false, loadingMode, accessMode));
            break;
        }

        case LoadingPolicyPrepend:
        {
            if (existingTask)
            {
                existingTask->setStatus(LoadingTask::LoadingTaskStatusLoading);
            }

            // stop and postpone current task if it is a preloading task

            if ((loadingTask = checkLoadingTask(m_currentTask, LoadingTaskFilterPreloading)))
            {
                loadingTask->setStatus(LoadingTask::LoadingTaskStatusStopping);
                load(loadingTask->loadingDescription(), LoadingPolicyPreload);
            }
/*
            qCDebug(DIGIKAM_GENERAL_LOG) << "LoadingPolicyPrepend, Existing task " << existingTask << ", m_currentTask " << m_currentTask;
*/
            // prepend new loading task

            if (existingTask)
            {
                break;
            }

            m_todo.prepend(createLoadingTask(description, false, loadingMode, accessMode));
            break;
        }

        case LoadingPolicySimplePrepend:
        {
            m_todo.prepend(createLoadingTask(description, false, loadingMode, accessMode));
            break;
        }

        case LoadingPolicyAppend:
        {
            if (existingTask)
            {
                existingTask->setStatus(LoadingTask::LoadingTaskStatusLoading);
            }

            // stop and postpone current task if it is a preloading task

            if ((loadingTask = checkLoadingTask(m_currentTask, LoadingTaskFilterPreloading)))
            {
                loadingTask->setStatus(LoadingTask::LoadingTaskStatusStopping);
                load(loadingTask->loadingDescription(), LoadingPolicyPreload);
            }

            if (existingTask)
            {
                break;
            }
/*
            qCDebug(DIGIKAM_GENERAL_LOG) << "LoadingPolicyAppend, Existing task " << existingTask << ", m_currentTask " << m_currentTask;
*/
            // append new loading task, put it in front of preloading tasks

            int i;

            for (i = 0 ; i < m_todo.count() ; ++i)
            {
                if (checkLoadingTask(m_todo.at(i), LoadingTaskFilterPreloading))
                {
                    break;
                }
            }

            m_todo.insert(i, createLoadingTask(description, false, loadingMode, accessMode));
            break;
        }

        case LoadingPolicySimpleAppend:
        {
            m_todo.append(createLoadingTask(description, false, loadingMode, accessMode));
            break;
        }

        case LoadingPolicyPreload:
        {
            // append to the very end of the list
/*
            qCDebug(DIGIKAM_GENERAL_LOG) << "LoadingPolicyPreload, Existing task " << existingTask;
*/
            if (existingTask)
            {
                break;
            }

            m_todo.append(createLoadingTask(description, true, loadingMode, accessMode));
            break;
        }
    }

    start(lock);
}

void ManagedLoadSaveThread::loadPreview(const LoadingDescription& description, LoadingPolicy policy)
{
    // Preview threads typically only support preview tasks,
    // so no need to differentiate with normal loading tasks.

    load(description, LoadingModeShared, policy);
}

void ManagedLoadSaveThread::loadThumbnail(const LoadingDescription& description)
{
    // Thumbnail threads typically only support thumbnail tasks,
    // so no need to differentiate with normal loading tasks.

    QMutexLocker lock(threadMutex());
    LoadingTask* const existingTask = findExistingTask(description);

    // reuse task if it exists

    if (existingTask)
    {
        existingTask->setStatus(LoadingTask::LoadingTaskStatusLoading);
        return;
    }

    // append new loading task

    m_todo.prepend(new ThumbnailLoadingTask(this, description));

    start(lock);
}

void ManagedLoadSaveThread::preloadThumbnail(const LoadingDescription& description)
{
    QMutexLocker lock(threadMutex());
    LoadingTask* const existingTask = findExistingTask(description);

    // reuse task if it exists

    if (existingTask)
    {
        return;
    }

    // create new loading task

    ThumbnailLoadingTask* const task = new ThumbnailLoadingTask(this, description);

    // mark as preload task

    task->setStatus(LoadingTask::LoadingTaskStatusPreloading);

    // append to the end of the list

    m_todo.append(task);
    start(lock);
}

void ManagedLoadSaveThread::preloadThumbnailGroup(const QList<LoadingDescription>& descriptions)
{
    if (descriptions.isEmpty())
    {
        return;
    }

    QMutexLocker lock(threadMutex());
    QList<LoadSaveTask*> todo;

    for (const LoadingDescription& description : std::as_const(descriptions))
    {
        LoadingTask* const existingTask = findExistingTask(description);

        // reuse task if it exists

        if (existingTask)
        {
            continue;
        }

        // create new loading task

        ThumbnailLoadingTask* const task = new ThumbnailLoadingTask(this, description);

        // mark as preload task

        task->setStatus(LoadingTask::LoadingTaskStatusPreloading);

        // append to the end of the list

        todo << task;
    }

    if (!todo.isEmpty())
    {
        m_todo << todo;
        start(lock);
    }
}

void ManagedLoadSaveThread::prependThumbnailGroup(const QList<LoadingDescription>& descriptions)
{
    // This method is meant to prepend a group of loading tasks after the current task,
    // in the order they are given here, pushing the existing tasks to the back respectively removing double tasks.

    if (descriptions.isEmpty())
    {
        return;
    }

    QMutexLocker lock(threadMutex());

    int index = 0;

    for (int i = 0 ; i < descriptions.size() ; ++i)
    {
        LoadingTask* const existingTask = findExistingTask(descriptions.at(i));

        // remove task, if not the current task

        if (existingTask)
        {
            if (existingTask == dynamic_cast<LoadingTask*>(m_currentTask))
            {
                continue;
            }

            m_todo.removeAll(existingTask);

            if (index > m_todo.size())
            {
                --index;
            }

            delete existingTask;
        }

        // insert new loading task, in the order given by descriptions list

        m_todo.insert(index++, new ThumbnailLoadingTask(this, descriptions.at(i)));
    }

    start(lock);
}

LoadingTask* ManagedLoadSaveThread::createLoadingTask(const LoadingDescription& description,
                                                      bool preloading, LoadingMode loadingMode,
                                                      AccessMode accessMode)
{
    if (description.previewParameters.type == LoadingDescription::PreviewParameters::PreviewImage)
    {
        return new PreviewLoadingTask(this, description);
    }

    if (loadingMode == LoadingModeShared)
    {
        if (preloading)
        {
            return new SharedLoadingTask(this, description, accessMode, LoadingTask::LoadingTaskStatusPreloading);
        }
        else
        {
            return new SharedLoadingTask(this, description, accessMode);
        }
    }
    else
    {
        if (preloading)
        {
            return new LoadingTask(this, description, LoadingTask::LoadingTaskStatusPreloading);
        }
        else
        {
            return new LoadingTask(this, description);
        }
    }
}

void ManagedLoadSaveThread::stopLoading(const QString& filePath, LoadingTaskFilter filter)
{
    QMutexLocker lock(threadMutex());
    removeLoadingTasks(LoadingDescription(filePath), filter);
}

void ManagedLoadSaveThread::stopLoading(const LoadingDescription& desc, LoadingTaskFilter filter)
{
    QMutexLocker lock(threadMutex());
    removeLoadingTasks(desc, filter);
}

void ManagedLoadSaveThread::stopAllTasks()
{
    QMutexLocker lock(threadMutex());

    if (m_currentTask)
    {
        if      (m_currentTask->type() == LoadSaveTask::TaskTypeSaving)
        {
            SavingTask* const savingTask = dynamic_cast<SavingTask*>(m_currentTask);

            if (savingTask)
            {
                savingTask->setStatus(SavingTask::SavingTaskStatusStopping);
            }
        }
        else if (m_currentTask->type() == LoadSaveTask::TaskTypeLoading)
        {
            LoadingTask* const loadingTask = dynamic_cast<LoadingTask*>(m_currentTask);

            if (loadingTask)
            {
                loadingTask->setStatus(LoadingTask::LoadingTaskStatusStopping);
            }
        }
    }

    for (LoadSaveTask* const task : std::as_const(m_todo))
    {
        delete task;
    }

    m_todo.clear();
}

void ManagedLoadSaveThread::stopSaving(const QString& filePath)
{
    QMutexLocker lock(threadMutex());

    // stop current task if it is matching the criteria

    if (m_currentTask && (m_currentTask->type() == LoadSaveTask::TaskTypeSaving))
    {
        SavingTask* const savingTask = dynamic_cast<SavingTask*>(m_currentTask);

        if (savingTask && (filePath.isNull() || (savingTask->filePath() == filePath)))
        {
            savingTask->setStatus(SavingTask::SavingTaskStatusStopping);
        }
    }

    // remove relevant tasks from list

    for (int i = 0 ; i < m_todo.size() ; ++i)
    {
        LoadSaveTask* const task = m_todo.at(i);

        if (task->type() == LoadSaveTask::TaskTypeSaving)
        {
            SavingTask* const savingTask = dynamic_cast<SavingTask*>(task);

            if (savingTask && (filePath.isNull() || (savingTask->filePath() == filePath)))
            {
                delete m_todo.takeAt(i--);
            }
        }
    }
}

void ManagedLoadSaveThread::removeLoadingTasks(const LoadingDescription& description, LoadingTaskFilter filter)
{
    LoadingTask* loadingTask = nullptr;

    // stop current task if it is matching the criteria

    if ((loadingTask = checkLoadingTask(m_currentTask, filter)))
    {
        if (description.filePath.isNull() || (loadingTask->loadingDescription() == description))
        {
            loadingTask->setStatus(LoadingTask::LoadingTaskStatusStopping);
        }
    }

    // remove relevant tasks from list

    for (int i = 0 ; i < m_todo.size() ; ++i)
    {
        LoadSaveTask* const task = m_todo.at(i);

        if ((loadingTask = checkLoadingTask(task, filter)))
        {
            if (description.filePath.isNull() || (loadingTask->loadingDescription() == description))
            {
                delete m_todo.takeAt(i--);
            }
        }
    }
}

void ManagedLoadSaveThread::save(const DImg& image, const QString& filePath, const QString& format)
{
    QMutexLocker lock(threadMutex());
    LoadingTask* loadingTask = nullptr;

    // stop and postpone current task if it is a preloading task

    if (m_currentTask && (loadingTask = checkLoadingTask(m_currentTask, LoadingTaskFilterPreloading)))
    {
        loadingTask->setStatus(LoadingTask::LoadingTaskStatusStopping);
        load(loadingTask->loadingDescription(), LoadingPolicyPreload);
    }

    // append new loading task, put it in front of preloading tasks

    int i;

    for (i = 0 ; i < m_todo.count() ; ++i)
    {
        LoadSaveTask* const task = m_todo.at(i);

        if (checkLoadingTask(task, LoadingTaskFilterPreloading))
        {
            break;
        }
    }

    m_todo.insert(i, new SavingTask(this, image, filePath, format));
    start(lock);
}

} // namespace Digikam

#include "moc_managedloadsavethread.cpp"
