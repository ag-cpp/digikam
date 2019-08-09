/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-01-20
 * Description : image file IO threaded interface.
 *
 * Copyright (C) 2005-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2005-2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_LOAD_SAVE_TASK_H
#define DIGIKAM_LOAD_SAVE_TASK_H

// Qt includes

#include <QList>
#include <QEvent>

// Local includes

#include "dimg.h"
#include "dimgloaderobserver.h"
#include "loadingdescription.h"
#include "loadingcache.h"

namespace Digikam
{

class LoadSaveThread;

class LoadSaveTask
{
public:

    enum TaskType
    {
        TaskTypeLoading,
        TaskTypeSaving
    };

public:

    explicit LoadSaveTask(LoadSaveThread* const thread)
        : m_thread(thread)
    {
    };

    virtual ~LoadSaveTask()
    {
    };

    virtual void execute() = 0;
    virtual TaskType type() = 0;

    virtual void progressInfo(DImg* const img, float progress) = 0;
    virtual bool continueQuery(DImg* const img) = 0;

protected:

    LoadSaveThread* m_thread;
};

//---------------------------------------------------------------------------------------------------

class LoadingTask : public LoadSaveTask,
                    public DImgLoaderObserver
{
public:

    enum LoadingTaskStatus
    {
        LoadingTaskStatusLoading,
        LoadingTaskStatusPreloading,
        LoadingTaskStatusStopping
    };

public:

    explicit LoadingTask(LoadSaveThread* const thread, const LoadingDescription& description,
                         LoadingTaskStatus loadingTaskStatus = LoadingTaskStatusLoading)
        : LoadSaveTask(thread),
          m_loadingDescription(description),
          m_loadingTaskStatus(loadingTaskStatus)
    {
    }

    LoadingTaskStatus status() const
    {
        return m_loadingTaskStatus;
    }

    QString filePath() const
    {
        return m_loadingDescription.filePath;
    }

    const LoadingDescription& loadingDescription() const
    {
        return m_loadingDescription;
    }

    // LoadSaveTask

    virtual void execute() override;
    virtual TaskType type() override;

    // DImgLoaderObserver

    virtual void progressInfo(DImg* const img, float progress) override;
    virtual bool continueQuery(DImg* const img) override;

    virtual void setStatus(LoadingTaskStatus status);

protected:

    LoadingDescription m_loadingDescription;
    LoadingTaskStatus  m_loadingTaskStatus;
};

//---------------------------------------------------------------------------------------------------

class SharedLoadingTask : public LoadingTask,
                          public LoadingProcess,
                          public LoadingProcessListener
{
public:

    explicit SharedLoadingTask(LoadSaveThread* const thread, const LoadingDescription& description,
                               LoadSaveThread::AccessMode mode = LoadSaveThread::AccessModeReadWrite,
                               LoadingTaskStatus loadingTaskStatus = LoadingTaskStatusLoading);

    virtual void execute() override;
    virtual void progressInfo(DImg* const img, float progress) override;
    virtual bool continueQuery(DImg* const img) override;
    virtual void setStatus(LoadingTaskStatus status) override;

    virtual bool needsPostProcessing() const;
    virtual void postProcess();

    // LoadingProcess

    virtual bool completed() const override;
    virtual QString filePath() const override;
    virtual QString cacheKey() const override;
    virtual void addListener(LoadingProcessListener* const listener) override;
    virtual void removeListener(LoadingProcessListener* const listener) override;
    virtual void notifyNewLoadingProcess(LoadingProcess* const process, const LoadingDescription& description) override;

    // LoadingProcessListener

    virtual bool querySendNotifyEvent() const override;
    virtual void setResult(const LoadingDescription& loadingDescription, const DImg& img) override;
    virtual LoadSaveNotifier* loadSaveNotifier() const override;
    virtual LoadSaveThread::AccessMode accessMode() override;

    DImg img() { return m_img; }

protected:

    bool                           m_completed;
    LoadSaveThread::AccessMode     m_accessMode;
    LoadingProcess*                m_usedProcess;
    QList<LoadingProcessListener*> m_listeners;
    DImg                           m_img;
    LoadingDescription             m_resultLoadingDescription;
};

//---------------------------------------------------------------------------------------------------

class SavingTask : public LoadSaveTask,
                   public DImgLoaderObserver
{
public:

    enum SavingTaskStatus
    {
        SavingTaskStatusSaving,
        SavingTaskStatusStopping
    };

public:

    explicit SavingTask(LoadSaveThread* const thread, const DImg& img, const QString& filePath, const QString& format)
        : LoadSaveTask(thread),
          m_filePath(filePath),
          m_format(format),
          m_img(img),
          m_savingTaskStatus(SavingTaskStatusSaving)
    {
    };

    SavingTaskStatus status() const
    {
        return m_savingTaskStatus;
    }

    QString filePath() const
    {
        return m_filePath;
    }

public:

    virtual void     execute() override;
    virtual TaskType type() override;

    virtual void     progressInfo(DImg* const img, float progress) override;
    virtual bool     continueQuery(DImg* const img) override;

    virtual void     setStatus(SavingTaskStatus status);

private:

    QString          m_filePath;
    QString          m_format;
    DImg             m_img;
    SavingTaskStatus m_savingTaskStatus;
};

} // namespace Digikam

#endif // DIGIKAM_LOAD_SAVE_TASK_H
