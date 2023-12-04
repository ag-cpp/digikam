// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef PLAYBACKSOUNDCUEITEM_H
#define PLAYBACKSOUNDCUEITEM_H

#include "PlaybackItem.h"

namespace Marble
{

class GeoDataSoundCue;

class PlaybackSoundCueItem : public PlaybackItem
{
    Q_OBJECT
public:
    explicit PlaybackSoundCueItem( const GeoDataSoundCue* soundCue );
    const GeoDataSoundCue* soundCue() const;
    double duration() const override;
    void play() override;
    void pause() override;
    void seek( double position ) override;
    void stop() override;
private:
    const GeoDataSoundCue* m_soundCue;
    QString m_href;
};

}
#endif
