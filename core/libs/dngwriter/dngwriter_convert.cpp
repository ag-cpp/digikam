/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-09-25
 * Description : a tool to convert RAW file to DNG
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2010-2011 by Jens Mueller <tschenser at gmx dot de>
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

#include "dngwriter_p.h"

namespace Digikam
{

int DNGWriter::convert()
{
    int ret;
    d->cancel = false;

    try
    {
        ret = extractRawStage();

        if (ret != PROCESSCONTINUE)
        {
            return ret;
        }

        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Raw data loaded:" ;
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Data Size:     " << d->rawData.size() << " bytes";
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Date:          " << d->identify->dateTime.toString(Qt::ISODate);
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Make:          " << d->identify->make;
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Model:         " << d->identify->model;
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- ImageSize:     " << d->identify->imageSize.width()  << "x" << d->identify->imageSize.height();
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- FullSize:      " << d->identify->fullSize.width()   << "x" << d->identify->fullSize.height();
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- OutputSize:    " << d->identify->outputSize.width() << "x" << d->identify->outputSize.height();
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Orientation:   " << d->identify->orientation;
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Top margin:    " << d->identify->topMargin;
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Left margin:   " << d->identify->leftMargin;
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Filter:        " << d->identify->filterPattern;
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Colors:        " << d->identify->rawColors;
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- Black:         " << d->identify->blackPoint;
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- White:         " << d->identify->whitePoint;
        qCDebug(DIGIKAM_GENERAL_LOG) << "--- CAM->XYZ:" ;

        QString matrixVal;

        for (int i = 0 ; i < 4 ; ++i)
        {
            qCDebug(DIGIKAM_GENERAL_LOG)
                     << "                   "
                     << QString().asprintf("%03.4f  %03.4f  %03.4f", d->identify->cameraXYZMatrix[i][0],
                                                                     d->identify->cameraXYZMatrix[i][1],
                                                                     d->identify->cameraXYZMatrix[i][2]);
        }

        // Check if CFA layout is supported by DNG SDK.

        Private::DNGBayerPattern bayerPattern = Private::Unknown;
        uint32 filter                         = 0;
        bool fujiRotate90                     = false;

        // Standard bayer layouts

        if      (d->identify->filterPattern == QLatin1String("GRBGGRBGGRBGGRBG"))
        {
            bayerPattern = Private::Standard;
            filter       = 0;
        }
        else if (d->identify->filterPattern == QLatin1String("RGGBRGGBRGGBRGGB"))
        {
            bayerPattern = Private::Standard;
            filter       = 1;
        }
        else if (d->identify->filterPattern == QLatin1String("BGGRBGGRBGGRBGGR"))
        {
            bayerPattern = Private::Standard;
            filter       = 2;
        }
        else if (d->identify->filterPattern == QLatin1String("GBRGGBRGGBRGGBRG"))
        {
            bayerPattern = Private::Standard;
            filter       = 3;
        }
        else if ((d->identify->filterPattern      == QLatin1String("RGBGRGBGRGBGRGBG")) &&
                 (d->identifyMake->make.toUpper() == QLatin1String("FUJIFILM")))
        {
            // Fuji layouts

            bayerPattern = Private::Fuji;
            fujiRotate90 = false;
            filter       = 0;
        }
        else if ((d->identify->filterPattern      == QLatin1String("RBGGBRGGRBGGBRGG")) &&
                 (d->identifyMake->make.toUpper() == QLatin1String("FUJIFILM")))
        {
            // Fuji layouts

            bayerPattern = Private::Fuji;
            fujiRotate90 = true;
            filter       = 0;
        }
        else if ((d->identify->filterPattern      == QLatin1String("GGGGBRGGGGRBGGGG")) &&
                 (d->identifyMake->make.toUpper() == QLatin1String("FUJIFILM")))
        {
            // Fuji layouts

            bayerPattern = Private::Fuji6x6;
            fujiRotate90 = false;
            filter       = 1;
        }
        else if ((d->identify->rawColors == 3)                 &&
                 (d->identify->filterPattern.isEmpty())        &&
/*
                 (identify->filterPattern == QString(""))   &&
*/
                 ((uint32)d->rawData.size() == d->identify->outputSize.width() * d->identify->outputSize.height() * 3 * sizeof(uint16)))
        {
            bayerPattern = Private::LinearRaw;
        }
        else if (d->identify->rawColors == 4)           // Four color sensors
        {
            bayerPattern = Private::FourColor;

            if (d->identify->filterPattern.length() != 16)
            {
                qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Bayer mosaic not supported. Aborted..." ;
                d->clearMemory();

                return FILENOTSUPPORTED;
            }

            for (int i = 0 ; i < 16 ; ++i)
            {
                filter = filter >> 2;

                if      (d->identify->filterPattern[i] == QLatin1Char('G'))
                {
                    filter |= 0x00000000;
                }
                else if (d->identify->filterPattern[i] == QLatin1Char('M'))
                {
                    filter |= 0x40000000;
                }
                else if (d->identify->filterPattern[i] == QLatin1Char('C'))
                {
                    filter |= 0x80000000;
                }
                else if (d->identify->filterPattern[i] == QLatin1Char('Y'))
                {
                    filter |= 0xC0000000;
                }
                else
                {
                    qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Bayer mosaic not supported. Aborted..." ;
                    d->clearMemory();

                    return FILENOTSUPPORTED;
                }
            }
        }
        else
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Bayer mosaic not supported. Aborted..." ;
            d->clearMemory();

            return FILENOTSUPPORTED;
        }

        if (fujiRotate90)
        {
            if (!d->fujiRotate())
            {
                qCDebug(DIGIKAM_GENERAL_LOG) << "Can not rotate fuji image. Aborted...";
                d->clearMemory();

                return PROCESSFAILED;
            }

            int tmp         = d->outputWidth;
            d->outputWidth  = d->outputHeight;
            d->outputHeight = tmp;
        }

        d->activeArea   = dng_rect(d->identify->outputSize.height(), d->identify->outputSize.width());
        d->activeWidth  = d->identify->outputSize.width();
        d->activeHeight = d->identify->outputSize.height();

        // Check if number of Raw Color components is supported.

        if ((d->identify->rawColors != 3) && (d->identify->rawColors != 4))
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Number of Raw color components not supported. Aborted..." ;
            d->clearMemory();

            return PROCESSFAILED;
        }

        int width  = d->identify->outputSize.width();
        int height = d->identify->outputSize.height();

/*
        // NOTE: code to hack RAW data extraction

        QString   rawdataFilePath(d->inputInfo.completeBaseName() + QString(".dat"));
        QFileInfo rawdataInfo(rawdataFilePath);

        QFile rawdataFile(rawdataFilePath);

        if (!rawdataFile.open(QIODevice::WriteOnly))
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Cannot open file to write RAW data. Aborted..." ;
            d->clearMemory();
            return PROCESSFAILED;
        }

        QDataStream rawdataStream(&rawdataFile);
        rawdataStream.writeRawData(d->rawData.data(), d->rawData.size());
        rawdataFile.close();
*/
        // -----------------------------------------------------------------------------------------

        if (d->cancel)
        {
            d->clearMemory();
            return PROCESSCANCELED;
        }

        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: DNG memory allocation and initialization" ;

        dng_memory_allocator memalloc(gDefaultDNGMemoryAllocator);

        dng_rect rect(height, width);
        DNGWriterHost host(d, &memalloc);

        host.SetSaveDNGVersion(dngVersion_SaveDefault);
        host.SetSaveLinearDNG(false);
        host.SetKeepOriginalFile(true);

        AutoPtr<dng_image> image(new dng_simple_image(rect, (bayerPattern == Private::LinearRaw) ? 3 : 1, ttShort, memalloc));

        if (d->cancel)
        {
            d->clearMemory();
            return PROCESSCANCELED;
        }

        // -----------------------------------------------------------------------------------------

        dng_pixel_buffer buffer;

        buffer.fArea       = rect;
        buffer.fPlane      = 0;
        buffer.fPlanes     = (bayerPattern == Private::LinearRaw) ? 3 : 1;
        buffer.fRowStep    = buffer.fPlanes * width;
        buffer.fColStep    = buffer.fPlanes;
        buffer.fPlaneStep  = 1;
        buffer.fPixelType  = ttShort;
        buffer.fPixelSize  = TagTypeSize(ttShort);
        buffer.fData       = d->rawData.data();
        image->Put(buffer);

        if (d->cancel)
        {
            d->clearMemory();
            return PROCESSCANCELED;
        }

        // -----------------------------------------------------------------------------------------

        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: DNG Negative structure creation";

        AutoPtr<dng_negative> negative(host.Make_dng_negative());

        negative->SetDefaultScale(dng_urational(d->outputWidth,  d->activeWidth),
                                  dng_urational(d->outputHeight, d->activeHeight));

        if (bayerPattern != Private::LinearRaw)
        {
            negative->SetDefaultCropOrigin(8, 8);
            negative->SetDefaultCropSize(d->activeWidth - 16, d->activeHeight - 16);
        }
        else
        {
            negative->SetDefaultCropOrigin(0, 0);
            negative->SetDefaultCropSize(d->activeWidth, d->activeHeight);
        }

        negative->SetActiveArea(d->activeArea);
        negative->SetModelName(d->identify->model.toLatin1().constData());
        negative->SetLocalName(QString::fromUtf8("%1 %2").arg(d->identify->make, d->identify->model).toLatin1().constData());
        negative->SetOriginalRawFileName(d->inputInfo.fileName().toLatin1().constData());
        negative->SetColorChannels(d->identify->rawColors);

        ColorKeyCode colorCodes[4] =
        {
            colorKeyMaxEnum,
            colorKeyMaxEnum,
            colorKeyMaxEnum,
            colorKeyMaxEnum
        };

        for (int i = 0 ; i < qMax(4, d->identify->colorKeys.length()) ; ++i)
        {
            if      (d->identify->colorKeys[i] == QLatin1Char('R'))
            {
                colorCodes[i] = colorKeyRed;
            }
            else if (d->identify->colorKeys[i] == QLatin1Char('G'))
            {
                colorCodes[i] = colorKeyGreen;
            }
            else if (d->identify->colorKeys[i] == QLatin1Char('B'))
            {
                colorCodes[i] = colorKeyBlue;
            }
            else if (d->identify->colorKeys[i] == QLatin1Char('C'))
            {
                colorCodes[i] = colorKeyCyan;
            }
            else if (d->identify->colorKeys[i] == QLatin1Char('M'))
            {
                colorCodes[i] = colorKeyMagenta;
            }
            else if (d->identify->colorKeys[i] == QLatin1Char('Y'))
            {
                colorCodes[i] = colorKeyYellow;
            }
        }

        negative->SetColorKeys(colorCodes[0], colorCodes[1], colorCodes[2], colorCodes[3]);

        switch (bayerPattern)
        {
            case Private::Standard:

                // Standard bayer mosaicing. All work fine there.
                // Bayer CCD mask: https://en.wikipedia.org/wiki/Bayer_filter

                negative->SetBayerMosaic(filter);
                break;

            case Private::Fuji:

                // TODO: Fuji is special case. Need to setup different bayer rules here.
                // It do not work in all settings. Need indeep investiguations.
                // Fuji superCCD: https://en.wikipedia.org/wiki/Super_CCD

                negative->SetFujiMosaic(filter);
                break;

            case Private::Fuji6x6:

                // TODO: Fuji is special case. Need to setup different bayer rules here.
                // It do not work in all settings. Need indeep investiguations.

                negative->SetFujiMosaic6x6(filter);
                break;

            case Private::FourColor:
                negative->SetQuadMosaic(filter);
                break;

            default:
                break;
        }

        negative->SetWhiteLevel(d->identify->whitePoint, 0);
        negative->SetWhiteLevel(d->identify->whitePoint, 1);
        negative->SetWhiteLevel(d->identify->whitePoint, 2);
        negative->SetWhiteLevel(d->identify->whitePoint, 3);

        const dng_mosaic_info* const mosaicinfo = negative->GetMosaicInfo();

        if ((mosaicinfo != nullptr) && (mosaicinfo->fCFAPatternSize == dng_point(2, 2)))
        {
            negative->SetQuadBlacks(d->identify->blackPoint + d->identify->blackPointCh[0],
                                    d->identify->blackPoint + d->identify->blackPointCh[1],
                                    d->identify->blackPoint + d->identify->blackPointCh[2],
                                    d->identify->blackPoint + d->identify->blackPointCh[3]);
        }
        else
        {
            negative->SetBlackLevel(d->identify->blackPoint, 0);
        }

        negative->SetBaselineExposure(0.0);
        negative->SetBaselineNoise(1.0);
        negative->SetBaselineSharpness(1.0);

        dng_orientation orientation;

        switch (d->identify->orientation)
        {
            case DRawInfo::ORIENTATION_180:
                orientation = dng_orientation::Rotate180();
                break;

            case DRawInfo::ORIENTATION_Mirror90CCW:
                orientation = dng_orientation::Mirror90CCW();
                break;

            case DRawInfo::ORIENTATION_90CCW:
                orientation = dng_orientation::Rotate90CCW();
                break;

            case DRawInfo::ORIENTATION_90CW:
                orientation = dng_orientation::Rotate90CW();
                break;

            default:   // ORIENTATION_NONE
                orientation = dng_orientation::Normal();
                break;
        }

        negative->SetBaseOrientation(orientation);
        negative->SetAntiAliasStrength(dng_urational(100, 100));
        negative->SetLinearResponseLimit(1.0);
        negative->SetShadowScale( dng_urational(1, 1) );
        negative->SetAnalogBalance(dng_vector_3(1.0, 1.0, 1.0));

        // -------------------------------------------------------------------------------

        AutoPtr<dng_camera_profile> prof(new dng_camera_profile);
        prof->SetName(QString::fromUtf8("%1 %2").arg(d->identify->make, d->identify->model).toLatin1().constData());

        // Set Camera->XYZ Color matrix as profile.

        dng_matrix matrix;

        switch (d->identify->rawColors)
        {
            case 3:
            {
                dng_matrix_3by3 camXYZ;
                camXYZ[0][0] = d->identify->cameraXYZMatrix[0][0];
                camXYZ[0][1] = d->identify->cameraXYZMatrix[0][1];
                camXYZ[0][2] = d->identify->cameraXYZMatrix[0][2];
                camXYZ[1][0] = d->identify->cameraXYZMatrix[1][0];
                camXYZ[1][1] = d->identify->cameraXYZMatrix[1][1];
                camXYZ[1][2] = d->identify->cameraXYZMatrix[1][2];
                camXYZ[2][0] = d->identify->cameraXYZMatrix[2][0];
                camXYZ[2][1] = d->identify->cameraXYZMatrix[2][1];
                camXYZ[2][2] = d->identify->cameraXYZMatrix[2][2];

                if (camXYZ.MaxEntry() == 0.0)
                {
                    qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: camera XYZ Matrix is null : camera not supported" ;
                    d->clearMemory();
                    return FILENOTSUPPORTED;
                }

                matrix = camXYZ;

                break;
            }

            case 4:
            {
                dng_matrix_4by3 camXYZ;
                camXYZ[0][0] = d->identify->cameraXYZMatrix[0][0];
                camXYZ[0][1] = d->identify->cameraXYZMatrix[0][1];
                camXYZ[0][2] = d->identify->cameraXYZMatrix[0][2];
                camXYZ[1][0] = d->identify->cameraXYZMatrix[1][0];
                camXYZ[1][1] = d->identify->cameraXYZMatrix[1][1];
                camXYZ[1][2] = d->identify->cameraXYZMatrix[1][2];
                camXYZ[2][0] = d->identify->cameraXYZMatrix[2][0];
                camXYZ[2][1] = d->identify->cameraXYZMatrix[2][1];
                camXYZ[2][2] = d->identify->cameraXYZMatrix[2][2];
                camXYZ[3][0] = d->identify->cameraXYZMatrix[3][0];
                camXYZ[3][1] = d->identify->cameraXYZMatrix[3][1];
                camXYZ[3][2] = d->identify->cameraXYZMatrix[3][2];

                if (camXYZ.MaxEntry() == 0.0)
                {
                    qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: camera XYZ Matrix is null : camera not supported" ;
                    d->clearMemory();
                    return FILENOTSUPPORTED;
                }

                matrix = camXYZ;

                break;
            }
        }

        prof->SetColorMatrix1((dng_matrix) matrix);
        prof->SetCalibrationIlluminant1(lsD65);
        negative->AddProfile(prof);

        dng_vector camNeutral(d->identify->rawColors);

        for (int i = 0 ; i < d->identify->rawColors ; ++i)
        {
            camNeutral[i] = 1.0 / d->identify->cameraMult[i];
        }

        negative->SetCameraNeutral(camNeutral);

        if (d->cancel)
        {
            d->clearMemory();
            return PROCESSCANCELED;
        }

        // -----------------------------------------------------------------------------------------

        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Updating metadata to DNG Negative" ;

        dng_date_time_info orgDateTimeInfo;
        dng_exif* const exif = negative->GetExif();
        exif->fModel.Set_ASCII(d->identify->model.toLatin1().constData());
        exif->fMake.Set_ASCII(d->identify->make.toLatin1().constData());

        QString   str;
        QScopedPointer<DMetadata> meta(new DMetadata);

        if (meta->load(inputFile()))
        {
            long int   num, den;
            long       val;

            // Time from original shot

            orgDateTimeInfo.SetDateTime(d->dngDateTime(meta->getItemDateTime()));
            exif->fDateTimeOriginal  = orgDateTimeInfo;

            dng_date_time_info digiDateTimeInfo;
            digiDateTimeInfo.SetDateTime(d->dngDateTime(meta->getDigitizationDateTime(true)));
            exif->fDateTimeDigitized = digiDateTimeInfo;

            negative->UpdateDateTime(orgDateTimeInfo);

            // String Tags

            str = meta->getExifTagString("Exif.Image.Make");
            if (!str.isEmpty()) exif->fMake.Set_ASCII(str.trimmed().toLatin1().constData());

            str = meta->getExifTagString("Exif.Image.Model");
            if (!str.isEmpty()) exif->fModel.Set_ASCII(str.trimmed().toLatin1().constData());

            str = meta->getExifTagString("Exif.Image.Software");
            if (!str.isEmpty()) exif->fSoftware.Set_ASCII(str.trimmed().toLatin1().constData());

            str = meta->getExifTagString("Exif.Image.ImageDescription");
            if (!str.isEmpty()) exif->fImageDescription.Set_ASCII(str.trimmed().toLatin1().constData());

            str = meta->getExifTagString("Exif.Image.Artist");
            if (!str.isEmpty()) exif->fArtist.Set_ASCII(str.trimmed().toLatin1().constData());

            str = meta->getExifTagString("Exif.Image.Copyright");
            if (!str.isEmpty()) exif->fCopyright.Set_ASCII(str.trimmed().toLatin1().constData());

            str = meta->getExifTagString("Exif.Photo.UserComment");
            if (!str.isEmpty()) exif->fUserComment.Set_ASCII(str.trimmed().toLatin1().constData());

            str = meta->getExifTagString("Exif.Image.CameraSerialNumber");
            if (!str.isEmpty()) exif->fCameraSerialNumber.Set_ASCII(str.trimmed().toLatin1().constData());

            str = meta->getExifTagString("Exif.GPSInfo.GPSLatitudeRef");
            if (!str.isEmpty()) exif->fGPSLatitudeRef.Set_ASCII(str.trimmed().toLatin1().constData());

            str = meta->getExifTagString("Exif.GPSInfo.GPSLongitudeRef");
            if (!str.isEmpty()) exif->fGPSLongitudeRef.Set_ASCII(str.trimmed().toLatin1().constData());

            str = meta->getExifTagString("Exif.GPSInfo.GPSSatellites");
            if (!str.isEmpty()) exif->fGPSSatellites.Set_ASCII(str.trimmed().toLatin1().constData());

            str = meta->getExifTagString("Exif.GPSInfo.GPSStatus");
            if (!str.isEmpty()) exif->fGPSStatus.Set_ASCII(str.trimmed().toLatin1().constData());

            str = meta->getExifTagString("Exif.GPSInfo.GPSMeasureMode");
            if (!str.isEmpty()) exif->fGPSMeasureMode.Set_ASCII(str.trimmed().toLatin1().constData());

            str = meta->getExifTagString("Exif.GPSInfo.GPSSpeedRef");
            if (!str.isEmpty()) exif->fGPSSpeedRef.Set_ASCII(str.trimmed().toLatin1().constData());

            str = meta->getExifTagString("Exif.GPSInfo.GPSTrackRef");
            if (!str.isEmpty()) exif->fGPSTrackRef.Set_ASCII(str.trimmed().toLatin1().constData());

            str = meta->getExifTagString("Exif.GPSInfo.GPSSpeedRef");
            if (!str.isEmpty()) exif->fGPSSpeedRef.Set_ASCII(str.trimmed().toLatin1().constData());

            str = meta->getExifTagString("Exif.GPSInfo.GPSImgDirectionRef");
            if (!str.isEmpty()) exif->fGPSSpeedRef.Set_ASCII(str.trimmed().toLatin1().constData());

            str = meta->getExifTagString("Exif.GPSInfo.GPSMapDatum");
            if (!str.isEmpty()) exif->fGPSMapDatum.Set_ASCII(str.trimmed().toLatin1().constData());

            str = meta->getExifTagString("Exif.GPSInfo.GPSDestLatitudeRef");
            if (!str.isEmpty()) exif->fGPSDestLatitudeRef.Set_ASCII(str.trimmed().toLatin1().constData());

            str = meta->getExifTagString("Exif.GPSInfo.GPSDestLongitudeRef");
            if (!str.isEmpty()) exif->fGPSDestLongitudeRef.Set_ASCII(str.trimmed().toLatin1().constData());

            str = meta->getExifTagString("Exif.GPSInfo.GPSDestBearingRef");
            if (!str.isEmpty()) exif->fGPSDestBearingRef.Set_ASCII(str.trimmed().toLatin1().constData());

            str = meta->getExifTagString("Exif.GPSInfo.GPSDestDistanceRef");
            if (!str.isEmpty()) exif->fGPSDestDistanceRef.Set_ASCII(str.trimmed().toLatin1().constData());

            str = meta->getExifTagString("Exif.GPSInfo.GPSProcessingMethod");
            if (!str.isEmpty()) exif->fGPSProcessingMethod.Set_ASCII(str.trimmed().toLatin1().constData());

            str = meta->getExifTagString("Exif.GPSInfo.GPSAreaInformation");
            if (!str.isEmpty()) exif->fGPSAreaInformation.Set_ASCII(str.trimmed().toLatin1().constData());

            str = meta->getExifTagString("Exif.GPSInfo.GPSDateStamp");
            if (!str.isEmpty()) exif->fGPSDateStamp.Set_ASCII(str.trimmed().toLatin1().constData());

            // Rational Tags

            if (meta->getExifTagRational("Exif.Photo.ExposureTime", num, den))          exif->fExposureTime             = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.Photo.FNumber", num, den))               exif->fFNumber                  = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.Photo.ShutterSpeedValue", num, den))     exif->fShutterSpeedValue        = dng_srational(num, den);
            if (meta->getExifTagRational("Exif.Photo.ApertureValue", num, den))         exif->fApertureValue            = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.Photo.BrightnessValue", num, den))       exif->fBrightnessValue          = dng_srational(num, den);
            if (meta->getExifTagRational("Exif.Photo.ExposureBiasValue", num, den))     exif->fExposureBiasValue        = dng_srational(num, den);
            if (meta->getExifTagRational("Exif.Photo.MaxApertureValue", num, den))      exif->fMaxApertureValue         = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.Photo.FocalLength", num, den))           exif->fFocalLength              = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.Photo.DigitalZoomRatio", num, den))      exif->fDigitalZoomRatio         = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.Photo.SubjectDistance", num, den))       exif->fSubjectDistance          = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.Image.BatteryLevel", num, den))          exif->fBatteryLevelR            = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.Photo.FocalPlaneXResolution", num, den)) exif->fFocalPlaneXResolution    = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.Photo.FocalPlaneYResolution", num, den)) exif->fFocalPlaneYResolution    = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.GPSInfo.GPSAltitude", num, den))         exif->fGPSAltitude              = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.GPSInfo.GPSDOP", num, den))              exif->fGPSDOP                   = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.GPSInfo.GPSSpeed", num, den))            exif->fGPSSpeed                 = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.GPSInfo.GPSTrack", num, den))            exif->fGPSTrack                 = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.GPSInfo.GPSImgDirection", num, den))     exif->fGPSImgDirection          = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.GPSInfo.GPSDestBearing", num, den))      exif->fGPSDestBearing           = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.GPSInfo.GPSDestDistance", num, den))     exif->fGPSDestDistance          = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.GPSInfo.GPSLatitude", num, den, 0))      exif->fGPSLatitude[0]           = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.GPSInfo.GPSLatitude", num, den, 1))      exif->fGPSLatitude[1]           = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.GPSInfo.GPSLatitude", num, den, 2))      exif->fGPSLatitude[2]           = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.GPSInfo.GPSLongitude", num, den, 0))     exif->fGPSLongitude[0]          = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.GPSInfo.GPSLongitude", num, den, 1))     exif->fGPSLongitude[1]          = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.GPSInfo.GPSLongitude", num, den, 2))     exif->fGPSLongitude[2]          = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.GPSInfo.GPSTimeStamp", num, den, 0))     exif->fGPSTimeStamp[0]          = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.GPSInfo.GPSTimeStamp", num, den, 1))     exif->fGPSTimeStamp[1]          = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.GPSInfo.GPSTimeStamp", num, den, 2))     exif->fGPSTimeStamp[2]          = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.GPSInfo.GPSDestLatitude", num, den, 0))  exif->fGPSDestLatitude[0]       = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.GPSInfo.GPSDestLatitude", num, den, 1))  exif->fGPSDestLatitude[1]       = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.GPSInfo.GPSDestLatitude", num, den, 2))  exif->fGPSDestLatitude[2]       = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.GPSInfo.GPSDestLongitude", num, den, 0)) exif->fGPSDestLongitude[0]      = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.GPSInfo.GPSDestLongitude", num, den, 1)) exif->fGPSDestLongitude[1]      = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.GPSInfo.GPSDestLongitude", num, den, 2)) exif->fGPSDestLongitude[2]      = dng_urational(num, den);

            // Integer Tags

            if (meta->getExifTagLong("Exif.Photo.ExposureProgram", val))                exif->fExposureProgram          = (uint32)val;
            if (meta->getExifTagLong("Exif.Photo.ISOSpeedRatings", val))                exif->fISOSpeedRatings[0]       = (uint32)val;
            if (meta->getExifTagLong("Exif.Photo.MeteringMode", val))                   exif->fMeteringMode             = (uint32)val;
            if (meta->getExifTagLong("Exif.Photo.LightSource", val))                    exif->fLightSource              = (uint32)val;
            if (meta->getExifTagLong("Exif.Photo.Flash", val))                          exif->fFlash                    = (uint32)val;
            if (meta->getExifTagLong("Exif.Photo.SensingMethod", val))                  exif->fSensingMethod            = (uint32)val;
            if (meta->getExifTagLong("Exif.Photo.FileSource", val))                     exif->fFileSource               = (uint32)val;
            if (meta->getExifTagLong("Exif.Photo.SceneType", val))                      exif->fSceneType                = (uint32)val;
            if (meta->getExifTagLong("Exif.Photo.CustomRendered", val))                 exif->fCustomRendered           = (uint32)val;
            if (meta->getExifTagLong("Exif.Photo.ExposureMode", val))                   exif->fExposureMode             = (uint32)val;
            if (meta->getExifTagLong("Exif.Photo.WhiteBalance", val))                   exif->fWhiteBalance             = (uint32)val;
            if (meta->getExifTagLong("Exif.Photo.SceneCaptureType", val))               exif->fSceneCaptureType         = (uint32)val;
            if (meta->getExifTagLong("Exif.Photo.GainControl", val))                    exif->fGainControl              = (uint32)val;
            if (meta->getExifTagLong("Exif.Photo.Contrast", val))                       exif->fContrast                 = (uint32)val;
            if (meta->getExifTagLong("Exif.Photo.Saturation", val))                     exif->fSaturation               = (uint32)val;
            if (meta->getExifTagLong("Exif.Photo.Sharpness", val))                      exif->fSharpness                = (uint32)val;
            if (meta->getExifTagLong("Exif.Photo.SubjectDistanceRange", val))           exif->fSubjectDistanceRange     = (uint32)val;
            if (meta->getExifTagLong("Exif.Photo.FocalLengthIn35mmFilm", val))          exif->fFocalLengthIn35mmFilm    = (uint32)val;
            if (meta->getExifTagLong("Exif.Photo.ComponentsConfiguration", val))        exif->fComponentsConfiguration  = (uint32)val;
            if (meta->getExifTagLong("Exif.Photo.PixelXDimension", val))                exif->fPixelXDimension          = (uint32)val;
            if (meta->getExifTagLong("Exif.Photo.PixelYDimension", val))                exif->fPixelYDimension          = (uint32)val;
            if (meta->getExifTagLong("Exif.Photo.FocalPlaneResolutionUnit", val))       exif->fFocalPlaneResolutionUnit = (uint32)val;
            if (meta->getExifTagLong("Exif.GPSInfo.GPSAltitudeRef", val))               exif->fGPSAltitudeRef           = (uint32)val;
            if (meta->getExifTagLong("Exif.GPSInfo.GPSDifferential", val))              exif->fGPSDifferential          = (uint32)val;

            long gpsVer1        = 0;
            long gpsVer2        = 0;
            long gpsVer3        = 0;
            long gpsVer4        = 0;
            meta->getExifTagLong("Exif.GPSInfo.GPSVersionID", gpsVer1, 0);
            meta->getExifTagLong("Exif.GPSInfo.GPSVersionID", gpsVer2, 1);
            meta->getExifTagLong("Exif.GPSInfo.GPSVersionID", gpsVer3, 2);
            meta->getExifTagLong("Exif.GPSInfo.GPSVersionID", gpsVer4, 3);

            long gpsVer         = 0;
            gpsVer             += gpsVer1 << 24;
            gpsVer             += gpsVer2 << 16;
            gpsVer             += gpsVer3 <<  8;
            gpsVer             += gpsVer4;
            exif->fGPSVersionID = (uint32)gpsVer;

            // Nikon Markernotes

            if (meta->getExifTagRational("Exif.Nikon3.Lens", num, den, 0))              exif->fLensInfo[0]              = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.Nikon3.Lens", num, den, 1))              exif->fLensInfo[1]              = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.Nikon3.Lens", num, den, 2))              exif->fLensInfo[2]              = dng_urational(num, den);
            if (meta->getExifTagRational("Exif.Nikon3.Lens", num, den, 3))              exif->fLensInfo[3]              = dng_urational(num, den);
            if (meta->getExifTagLong("Exif.Nikon3.ISOSpeed", val, 1))                   exif->fISOSpeedRatings[1]       = (uint32)val;

            str = meta->getExifTagString("Exif.Nikon3.SerialNO");
            if (!str.isEmpty()) str = str.replace(QLatin1String("NO="), QLatin1String(""));
            if (!str.isEmpty()) exif->fCameraSerialNumber.Set_ASCII(str.trimmed().toLatin1().constData());

            str = meta->getExifTagString("Exif.Nikon3.SerialNumber");
            if (!str.isEmpty()) exif->fCameraSerialNumber.Set_ASCII(str.toLatin1().constData());

            if (meta->getExifTagLong("Exif.Nikon3.ShutterCount", val))                  exif->fImageNumber              = (uint32)val;
            if (meta->getExifTagLong("Exif.NikonLd1.LensIDNumber", val))                exif->fLensID.Set_ASCII((QString::fromUtf8("%1").arg(val)).toLatin1().constData());
            if (meta->getExifTagLong("Exif.NikonLd2.LensIDNumber", val))                exif->fLensID.Set_ASCII((QString::fromUtf8("%1").arg(val)).toLatin1().constData());
            if (meta->getExifTagLong("Exif.NikonLd3.LensIDNumber", val))                exif->fLensID.Set_ASCII((QString::fromUtf8("%1").arg(val)).toLatin1().constData());
            if (meta->getExifTagLong("Exif.NikonLd2.FocusDistance", val))               exif->fSubjectDistance          = dng_urational((uint32)pow(10.0, val/40.0), 100);
            if (meta->getExifTagLong("Exif.NikonLd3.FocusDistance", val))               exif->fSubjectDistance          = dng_urational((uint32)pow(10.0, val/40.0), 100);
            str = meta->getExifTagString("Exif.NikonLd1.LensIDNumber");
            if (!str.isEmpty()) exif->fLensName.Set_ASCII(str.trimmed().toLatin1().constData());
            str = meta->getExifTagString("Exif.NikonLd2.LensIDNumber");
            if (!str.isEmpty()) exif->fLensName.Set_ASCII(str.trimmed().toLatin1().constData());
            str = meta->getExifTagString("Exif.NikonLd3.LensIDNumber");
            if (!str.isEmpty()) exif->fLensName.Set_ASCII(str.trimmed().toLatin1().constData());

            // Canon Markernotes

            if (meta->getExifTagLong("Exif.Canon.SerialNumber", val))                   exif->fCameraSerialNumber.Set_ASCII((QString::fromUtf8("%1").arg(val)).toLatin1().constData());
/*
            if (meta->getExifTagLong("Exif.CanonCs.LensType", val))                     exif->fLensID.Set_ASCII((QString::fromUtf8("%1").arg(val)).toLatin1().constData());
            if (meta->getExifTagLong("Exif.CanonCs.FlashActivity", val))                exif->fFlash                    = (uint32)val;
            if (meta->getExifTagLong("Exif.CanonFi.FileNumber", val))                   exif->fImageNumber              = (uint32)val;
            if (meta->getExifTagLong("Exif.CanonCs.MaxAperture", val))                  exif->fMaxApertureValue         = dng_urational(val, 100000);
*/
            if (meta->getExifTagLong("Exif.CanonCs.ExposureProgram", val))
            {
                switch (val)
                {
                    case 1:
                        exif->fExposureProgram = 2;
                        break;

                    case 2:
                        exif->fExposureProgram = 4;
                        break;

                    case 3:
                        exif->fExposureProgram = 3;
                        break;

                    case 4:
                        exif->fExposureProgram = 1;
                        break;

                    default:
                        break;
                }
            }

            if (meta->getExifTagLong("Exif.CanonCs.MeteringMode", val))
            {
                switch (val)
                {
                    case 1:
                        exif->fMeteringMode = 3;
                        break;

                    case 2:
                        exif->fMeteringMode = 1;
                        break;

                    case 3:
                        exif->fMeteringMode = 5;
                        break;

                    case 4:
                        exif->fMeteringMode = 6;
                        break;

                    case 5:
                        exif->fMeteringMode = 2;
                        break;

                    default:
                        break;
                }
            }

            long canonLensUnits = 1;
            if (meta->getExifTagRational("Exif.CanonCs.Lens", num, den, 2))             canonLensUnits                  = num;
            if (meta->getExifTagRational("Exif.CanonCs.Lens", num, den, 0))             exif->fLensInfo[1]              = dng_urational(num, canonLensUnits);
            if (meta->getExifTagRational("Exif.CanonCs.Lens", num, den, 1))             exif->fLensInfo[0]              = dng_urational(num, canonLensUnits);
            if (meta->getExifTagRational("Exif.Canon.FocalLength", num, den, 1))        exif->fFocalLength              = dng_urational(num, canonLensUnits);
            long canonLensType = 65535;
            if (meta->getExifTagLong("Exif.CanonCs.LensType", canonLensType))           exif->fLensID.Set_ASCII((QString::fromUtf8("%1").arg(canonLensType)).toLatin1().constData());
            str = meta->getExifTagString("Exif.Canon.LensModel");

            if      (!str.isEmpty())
            {
                exif->fLensName.Set_ASCII(str.trimmed().toLatin1().constData());
            }
            else if (canonLensType != 65535)
            {
                str = meta->getExifTagString("Exif.CanonCs.LensType");
                if (!str.isEmpty()) exif->fLensName.Set_ASCII(str.trimmed().toLatin1().constData());
            }

            str = meta->getExifTagString("Exif.Canon.OwnerName");
            if (!str.isEmpty()) exif->fOwnerName.Set_ASCII(str.trimmed().toLatin1().constData());

            str = meta->getExifTagString("Exif.Canon.FirmwareVersion");
            if (!str.isEmpty()) str = str.replace(QLatin1String("Firmware"), QLatin1String(""));
            if (!str.isEmpty()) str = str.replace(QLatin1String("Version"), QLatin1String(""));
            if (!str.isEmpty()) exif->fFirmware.Set_ASCII(str.trimmed().toLatin1().constData());

            str = meta->getExifTagString("Exif.CanonSi.ISOSpeed");
            if (!str.isEmpty()) exif->fISOSpeedRatings[1] = str.toInt();

            // Pentax Markernotes

            str = meta->getExifTagString("Exif.Pentax.LensType");

            if (!str.isEmpty())
            {
                exif->fLensName.Set_ASCII(str.trimmed().toLatin1().constData());
                exif->fLensName.TrimLeadingBlanks();
                exif->fLensName.TrimTrailingBlanks();
            }

            long pentaxLensId1 = 0;
            long pentaxLensId2 = 0;

            if ((meta->getExifTagLong("Exif.Pentax.LensType", pentaxLensId1, 0)) &&
                (meta->getExifTagLong("Exif.Pentax.LensType", pentaxLensId2, 1)))
            {
                exif->fLensID.Set_ASCII(QString::fromUtf8("%1").arg(pentaxLensId1, pentaxLensId2).toLatin1().constData());
            }

            // Olympus Makernotes

            str = meta->getExifTagString("Exif.OlympusEq.SerialNumber");
            if (!str.isEmpty()) exif->fCameraSerialNumber.Set_ASCII(str.trimmed().toLatin1().constData());

            str = meta->getExifTagString("Exif.OlympusEq.LensSerialNumber");
            if (!str.isEmpty()) exif->fLensSerialNumber.Set_ASCII(str.trimmed().toLatin1().constData());

            str = meta->getExifTagString("Exif.OlympusEq.LensModel");
            if (!str.isEmpty()) exif->fLensName.Set_ASCII(str.trimmed().toLatin1().constData());

            if (meta->getExifTagLong("Exif.OlympusEq.MinFocalLength", val))             exif->fLensInfo[0]              = dng_urational(val, 1);
            if (meta->getExifTagLong("Exif.OlympusEq.MaxFocalLength", val))             exif->fLensInfo[1]              = dng_urational(val, 1);

            // Panasonic Makernotes

            str = meta->getExifTagString("Exif.Panasonic.LensType");
            if (!str.isEmpty()) exif->fLensName.Set_ASCII(str.trimmed().toLatin1().constData());

            str = meta->getExifTagString("Exif.Panasonic.LensSerialNumber");
            if (!str.isEmpty()) exif->fLensSerialNumber.Set_ASCII(str.trimmed().toLatin1().constData());

            // Sony Makernotes

            if (meta->getExifTagLong("Exif.Sony2.LensID", val))
            {
                exif->fLensID.Set_ASCII(QString::fromUtf8("%1").arg(val).toLatin1().constData());
            }

            str = meta->getExifTagString("Exif.Photo.LensModel");

            if (str.isEmpty())
            {
                str = meta->getExifTagString("Exif.Sony2.LensID");
            }

            if (!str.isEmpty()) exif->fLensName.Set_ASCII(str.trimmed().toLatin1().constData());

            // -------------------------------------------

            if ((exif->fLensName.IsEmpty())          &&
                (exif->fLensInfo[0].As_real64() > 0) &&
                (exif->fLensInfo[1].As_real64() > 0))
            {
                QString     lensName;
                QTextStream stream(&lensName);
                double      dval = (double)exif->fLensInfo[0].n / (double)exif->fLensInfo[0].d;
                stream << QString::fromUtf8("%1").arg(dval, 0, 'f', 1);

                if (exif->fLensInfo[0].As_real64() != exif->fLensInfo[1].As_real64())
                {
                    dval = (double)exif->fLensInfo[1].n / (double)exif->fLensInfo[1].d;
                    stream << QString::fromUtf8("-%1").arg(dval, 0, 'f', 1);
                }

                stream << " mm";

                if (exif->fLensInfo[2].As_real64() > 0)
                {
                    dval = (double)exif->fLensInfo[2].n / (double)exif->fLensInfo[2].d;
                    stream << QString::fromUtf8(" 1/%1").arg(dval, 0, 'f', 1);
                }

                if ((exif->fLensInfo[3].As_real64() > 0) &&
                    (exif->fLensInfo[2].As_real64() != exif->fLensInfo[3].As_real64()))
                {
                    dval = (double)exif->fLensInfo[3].n / (double)exif->fLensInfo[3].d;
                    stream << QString::fromUtf8("-%1").arg(dval, 0, 'f', 1);
                }

                exif->fLensName.Set_ASCII(lensName.toLatin1().constData());
            }

            // Markernote backup.

            QByteArray mkrnts = meta->getExifTagData("Exif.Photo.MakerNote");

            if (!mkrnts.isEmpty())
            {
                qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Backup Makernote (" << mkrnts.size() << " bytes)" ;

                dng_memory_allocator memalloc1(gDefaultDNGMemoryAllocator);
                dng_memory_stream stream(memalloc1);
                stream.Put(mkrnts.data(), mkrnts.size());
                AutoPtr<dng_memory_block> block(host.Allocate(mkrnts.size()));
                stream.SetReadPosition(0);
                stream.Get(block->Buffer(), mkrnts.size());

                if ((d->identifyMake->make != QLatin1String("Canon")) &&
                    (d->identifyMake->make != QLatin1String("Panasonic")))
                {
                    negative->SetMakerNote(block);
                    negative->SetMakerNoteSafety(true);
                }
/*
                long mknOffset       = 0;
                QString mknByteOrder = meta->getExifTagString("Exif.MakerNote.ByteOrder");

                if ((meta->getExifTagLong("Exif.MakerNote.Offset", mknOffset)) && !mknByteOrder.isEmpty())
                {
                    dng_memory_stream streamPriv(memalloc);
                    streamPriv.SetBigEndian();

                    streamPriv.Put("Adobe", 5);
                    streamPriv.Put_uint8(0x00);
                    streamPriv.Put("MakN", 4);
                    streamPriv.Put_uint32(mkrnts.size() + mknByteOrder.size() + 4);
                    streamPriv.Put(mknByteOrder.toLatin1().constData(), mknByteOrder.size());
                    streamPriv.Put_uint32(mknOffset);
                    streamPriv.Put(mkrnts.data(), mkrnts.size());
                    AutoPtr<dng_memory_block> blockPriv(host.Allocate(streamPriv.Length()));
                    streamPriv.SetReadPosition(0);
                    streamPriv.Get(blockPriv->Buffer(), streamPriv.Length());
                    negative->SetPrivateData(blockPriv);
                }
*/
            }
        }

        if (d->backupOriginalRawFile)
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Backup Original RAW file (" << d->inputInfo.size() << " bytes)";

            QFileInfo originalFileInfo(inputFile());

            QFile originalFile(originalFileInfo.absoluteFilePath());

            if (!originalFile.open(QIODevice::ReadOnly))
            {
                qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Cannot open original RAW file to backup in DNG. Aborted...";
                d->clearMemory();

                return PROCESSFAILED;
            }

            QDataStream originalDataStream(&originalFile);

            quint32 forkLength = originalFileInfo.size();
            quint32 forkBlocks = (quint32)floor((forkLength + 65535.0) / 65536.0);

            QVector<quint32> offsets;
            quint32 offset     = (2 + forkBlocks) * sizeof(quint32);
            offsets.push_back(offset);

            QByteArray originalDataBlock;
            originalDataBlock.resize(CHUNK);

            QTemporaryFile compressedFile;

            if (!compressedFile.open())
            {
                qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Cannot open temporary file to write Zipped Raw data. Aborted...";
                d->clearMemory();

                return PROCESSFAILED;
            }

            QDataStream compressedDataStream(&compressedFile);

            for (quint32 block = 0 ; block < forkBlocks ; ++block)
            {
                int originalBlockLength = originalDataStream.readRawData(originalDataBlock.data(), CHUNK);

                QByteArray compressedDataBlock = qCompress((const uchar*)originalDataBlock.data(), originalBlockLength, -1);
                compressedDataBlock.remove(0, 4); // removes qCompress own header
                qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: compressed data block " << originalBlockLength << " -> " << compressedDataBlock.size();

                offset += compressedDataBlock.size();
                offsets.push_back(offset);

                compressedDataStream.writeRawData(compressedDataBlock.data(), compressedDataBlock.size());
            }

            dng_memory_allocator memalloc2(gDefaultDNGMemoryAllocator);
            dng_memory_stream tempDataStream(memalloc2);
            tempDataStream.SetBigEndian(true);
            tempDataStream.Put_uint32(forkLength);

            for (qint32 idx = 0 ; idx < offsets.size() ; ++idx)
            {
                tempDataStream.Put_uint32(offsets[idx]);
            }

            QByteArray compressedData;
            compressedData.resize(compressedFile.size());
            compressedFile.seek(0);
            compressedDataStream.readRawData(compressedData.data(), compressedData.size());
            tempDataStream.Put(compressedData.data(), compressedData.size());

            compressedFile.remove();
            originalFile.close();

            tempDataStream.Put_uint32(0);
            tempDataStream.Put_uint32(0);
            tempDataStream.Put_uint32(0);
            tempDataStream.Put_uint32(0);
            tempDataStream.Put_uint32(0);
            tempDataStream.Put_uint32(0);
            tempDataStream.Put_uint32(0);

            AutoPtr<dng_memory_block> block(host.Allocate(tempDataStream.Length()));
            tempDataStream.SetReadPosition(0);
            tempDataStream.Get(block->Buffer(), tempDataStream.Length());

            dng_md5_printer md5;
            md5.Process(block->Buffer(), block->LogicalSize());
            negative->SetOriginalRawFileData(block);
            negative->SetOriginalRawFileDigest(md5.Result());
            negative->ValidateOriginalRawFileDigest();
        }

        if (d->cancel)
        {
            d->clearMemory();
            return PROCESSCANCELED;
        }

        // -----------------------------------------------------------------------------------------

        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Build DNG Negative" ;

        // Assign Raw image data.

        negative->SetStage1Image(image);

        // Compute linearized and range mapped image

        negative->BuildStage2Image(host);

        // Compute demosaiced image (used by preview and thumbnail)

        negative->BuildStage3Image(host);

        negative->SynchronizeMetadata();
        negative->RebuildIPTC(true);

        if (d->cancel)
        {
            d->clearMemory();
            return PROCESSCANCELED;
        }

        // -----------------------------------------------------------------------------------------

        dng_preview_list previewList;
        dng_render negRender(host, *negative.Get());
        dng_jpeg_preview* jpeg_preview = new dng_jpeg_preview();

        if (d->previewMode != DNGWriter::NONE)
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: DNG preview image creation" ;

            dng_jpeg_preview* jpeg_preview  = new dng_jpeg_preview();
            jpeg_preview->fInfo.fApplicationName.Set_ASCII(QString::fromLatin1("digiKam").toLatin1().constData());
            jpeg_preview->fInfo.fApplicationVersion.Set_ASCII(digiKamVersion().toLatin1().constData());
            jpeg_preview->fInfo.fDateTime   = orgDateTimeInfo.Encode_ISO_8601();
            jpeg_preview->fInfo.fColorSpace = previewColorSpace_sRGB;

            negRender.SetMaximumSize(d->previewMode == MEDIUM ? 1280 : width);
            AutoPtr<dng_image> negImage(negRender.Render());
            dng_image_writer jpegWriter;
            jpegWriter.EncodeJPEGPreview(host, *negImage.Get(), *jpeg_preview, 5);
            AutoPtr<dng_preview> jp(static_cast<dng_preview*>(jpeg_preview));
            previewList.Append(jp);
        }

        if (d->cancel)
        {
            d->clearMemory();
            return PROCESSCANCELED;
        }

        // -----------------------------------------------------------------------------------------

        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: DNG thumbnail creation" ;

        dng_image_preview* thumbnail         = new dng_image_preview();
        thumbnail->fInfo.fApplicationName    = jpeg_preview->fInfo.fApplicationName;
        thumbnail->fInfo.fApplicationVersion = jpeg_preview->fInfo.fApplicationVersion;
        thumbnail->fInfo.fDateTime           = jpeg_preview->fInfo.fDateTime;
        thumbnail->fInfo.fColorSpace         = jpeg_preview->fInfo.fColorSpace;

        negRender.SetMaximumSize(256);
        thumbnail->fImage.Reset(negRender.Render());
        AutoPtr<dng_preview> tn(static_cast<dng_preview*>(thumbnail));
        previewList.Append(tn);

        if (d->cancel)
        {
            d->clearMemory();
            return PROCESSCANCELED;
        }

        // -----------------------------------------------------------------------------------------

        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Creating DNG file " << d->outputInfo.fileName() ;

        dng_image_writer writer;
        dng_file_stream filestream(QFile::encodeName(d->dngFilePath).constData(), true);

        writer.WriteDNG(host,
                        filestream,
                        *negative.Get(),
                        &previewList,
                        dngVersion_SaveDefault,
                        !d->jpegLossLessCompression
                       );

        // -----------------------------------------------------------------------------------------
        // Metadata makernote cleanup using Exiv2 for some RAW file types
        // See bug #204437 and #210371, and write XMP Sidecar if necessary
        // We disable writing to RAW files, see bug #381432
/*
        if (meta->load(d->dngFilePath))
        {
            if (d->inputInfo.suffix().toUpper() == QLatin1String("ORF"))
            {
                qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: cleanup makernotes using Exiv2" ;

                meta->setWriteDngFiles(true);
                meta->removeExifTag("Exif.OlympusIp.BlackLevel");
            }
            else
            {
                // Don't touch DNG file and create XMP sidecar depending of host application settings.
                meta->setWriteDngFiles(false);
            }

            meta->applyChanges();
        }
*/
        // -----------------------------------------------------------------------------------------
        // update modification time if desired

        if (d->updateFileDate)
        {
            QDateTime date = meta->getItemDateTime();

            qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Setting modification date from meta data: " << date.toString();

            DFileOperations::setModificationTime(d->dngFilePath, date);
        }
    }

    catch (const dng_exception& exception)
    {
        int ret = exception.ErrorCode();
        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: DNG SDK exception code (" << ret << "):" << d->dngErrorCodeToString(ret);
        d->clearMemory();

        return DNGSDKINTERNALERROR;
    }

    catch (...)
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: DNG SDK exception code unknow";
        d->clearMemory();

        return DNGSDKINTERNALERROR;
    }

    qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: DNG conversion complete...";
    d->clearMemory();

    return PROCESSCOMPLETE;
}

} // namespace Digikam
