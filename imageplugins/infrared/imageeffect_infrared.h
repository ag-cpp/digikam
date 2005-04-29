/* ============================================================
 * File  : imageeffect_infrared.h
 * Author: Gilles Caulier <caulier dot gilles at free.fr>
 * Date  : 2005-02-22
 * Description : a digiKam image editor plugin for simulate 
 *               infrared film.
 * 
 * Copyright 2005 by Gilles Caulier
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

#ifndef IMAGEEFFECT_INFRARED_H
#define IMAGEEFFECT_INFRARED_H

// Qt include.

#include <qimage.h>

// KDE include.

#include <kdialogbase.h>

class QPushButton;
class QSlider;
class QLCDNumber;
class QCheckBox;

namespace Digikam
{
class ImagePreviewWidget;
}

namespace DigikamInfraredImagesPlugin
{

class ImageEffect_Infrared : public KDialogBase
{
    Q_OBJECT

public:

    ImageEffect_Infrared(QWidget* parent);
    ~ImageEffect_Infrared();

protected:

    void closeEvent(QCloseEvent *e);
    
private:

    bool         m_cancel;

    QWidget     *m_parent;
    
    QPushButton *m_helpButton;
    
    QCheckBox   *m_addFilmGrain; 
                
    QSlider     *m_sensibilitySlider;
    
    QLCDNumber  *m_sensibilityLCDValue;
    
    Digikam::ImagePreviewWidget *m_imagePreviewWidget;
    
private:

    void infrared(uint* data, int Width, int Height, int Sensibility, bool Grain);
    
    // A color is represented in RGB value (e.g. 0xFFFFFF is white color). 
    // But R, G and B values has 256 values to be used so, this function analize 
    // the value and limits to this range.
    inline uchar LimitValues (int ColorValue)
       {
       if (ColorValue > 255) ColorValue = 255;        
       if (ColorValue < 0) ColorValue = 0;
       return ((uchar) ColorValue);
       };
    
    inline int GetStride (int Width)
       { 
       int LineWidth = Width * 4;
       if (LineWidth % 4) return (4 - (LineWidth % 4)); 
       return (0); 
       };
              
private slots:

    void slotHelp();
    void slotEffect();
    void slotOk();
    void slotCancel();
    void slotUser1();
    void slotSensibilityChanged(int);
   
};

}  // NameSpace DigikamInfraredImagesPlugin

#endif /* IMAGEEFFECT_INFRARED_H */
