/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#pragma once

#include "attlib.h"

#include "digikam_export.h"

namespace Marble
{

/**
 * Definitions of Astrolib Functions
 */

DIGIKAM_EXPORT double ddd(int d, int m, double s);  // deg, min, sec -> decimal degrees
DIGIKAM_EXPORT void dms(double dd, int& d, int& m, double& s);  // dec deg -> deg, min, sec
DIGIKAM_EXPORT double mjd(int day, int month, int year, double hour);  // modified Julian date
DIGIKAM_EXPORT double julcent(double mjuld);                 // Julian centuries since 2000.0
DIGIKAM_EXPORT void caldat(double mjd, int& day, int& month, int& year, double& hour);
DIGIKAM_EXPORT double DefTdUt(int yr);    // default value for TDT - UT in seconds
DIGIKAM_EXPORT double lsidtim(double jd, double lambda, double ep2);   // Sidereal Time

DIGIKAM_EXPORT double eps(double t);   //   obliquity of ecliptic
DIGIKAM_EXPORT Vec3 eclequ(double t, Vec3& r1);   //  ecliptic -> equatorial
DIGIKAM_EXPORT Vec3 equecl(double t, Vec3& r1);   //  equatorial -> ecliptic
DIGIKAM_EXPORT Mat3 pmatecl(double t1, double t2);   // ecl. precession
DIGIKAM_EXPORT Mat3 pmatequ(double t1, double t2);   // equ. precession
DIGIKAM_EXPORT Mat3 nutmat(double t, double& ep2, bool hipr = false);  // nutation (equatorial)
DIGIKAM_EXPORT Mat3 nutecl(double t, double& ep2);   // nutation matrix (ecliptic)
DIGIKAM_EXPORT Mat3 PoleMx(double xp, double yp);    // Polar motion matrix
DIGIKAM_EXPORT Vec3 aberrat(double t, Vec3& ve);    //  aberration

DIGIKAM_EXPORT Vec3 GeoPos(double jd, double ep2, double lat, double lng, double ht);
DIGIKAM_EXPORT Vec3 GeoPos(double jd, double ep2, double lat, double lng, double ht,
                           double xp, double yp);
DIGIKAM_EXPORT Vec3 EquHor(double jd, double ep2, double lat, double lng, Vec3 r);
DIGIKAM_EXPORT Vec3 HorEqu(double jd, double ep2, double lat, double lng, Vec3 r);
DIGIKAM_EXPORT void AppPos(double jd, double ep2, double lat, double lng, double ht,
                           int solsys, const Vec3& r, double& azim, double& elev, double& dist);
DIGIKAM_EXPORT void AppRADec(double jd, double ep2, double lat, double lng,
                             double azim, double elev, double& ra, double& dec);
DIGIKAM_EXPORT double Refract(double h, double p = 1015.0, double t = 15.0);  // refraction

DIGIKAM_EXPORT double eccanom(double man, double ecc);   // eccentric anomaly
DIGIKAM_EXPORT double hypanom(double mh, double ecc);    // hyperbolic anomaly
DIGIKAM_EXPORT void ellip(double gm, double t0, double t, double a, double ecc,
                          double m0, Vec3& r1, Vec3& v1); // elliptic state vector
DIGIKAM_EXPORT void hyperb(double gm, double t0, double t, double a, double ecc,
                           Vec3& r1, Vec3& v1);  // hyperbolic state vector
DIGIKAM_EXPORT void parab(double gm, double t0, double t, double q, double ecc,
                          Vec3& r1, Vec3& v1);  // elliptic state vector
DIGIKAM_EXPORT void kepler(double gm, double t0, double t, double m0, double a, double ecc,
                           double ran, double aper, double inc, Vec3& r1, Vec3& v1);
DIGIKAM_EXPORT void oscelm(double gm, double t, Vec3& r1, Vec3& v1,
                           double& t0, double& m0, double& a, double& ecc,
                           double& ran, double& aper, double& inc);

DIGIKAM_EXPORT Vec3 QuickSun(double t);    // low precision position of the Sun at time t

class DIGIKAM_EXPORT Sun200      // Calculating the Sun in epoch J2000.0 coordinates
{
public:

    Sun200();
    Vec3 position(double t);    // position of the Sun
    void state(double t, Vec3& rs, Vec3& vs);   // State vector of the Sun

private:

    double c3[9], s3[9];
    double c[9], s[9];
    double m2, m3, m4, m5, m6;
    double d, a, uu, tt;
    double cl, sl, cb, sb;
    double u, v, dl, dr, db;

    void addthe(double c1, double s1, double c2, double s2,
                double& cc, double& ss);
    void term(int i1, int i, int it, double dlc, double dls, double drc,
              double drs, double dbc, double dbs);
    void pertven();
    void pertmar();
    void pertjup();
    void pertsat();
    void pertmoo();
};

class DIGIKAM_EXPORT Moon200     // Calculating the position of the Moon in J2000.0
{
public:

    Moon200();
    Vec3 position(double t);    // position of the Moon

private:

    double dgam, dlam, n, gam1c, sinpi;
    double l0, l, ls, f, d, s;
    double dl0, dl, dls, df, dd, ds;
    double co[13][4];
    double si[13][4];

    void addthe(double c1, double s1, double c2, double s2,
                double& c, double& s);
    double sinus(double phi);
    void long_periodic(double t);
    void minit(double t);
    void term(int p, int q, int r, int s, double& x, double& y);
    void addsol(double coeffl, double coeffs, double coeffg,
                double coeffp, int p, int q, int r, int s);
    void solar1();
    void solar2();
    void solar3();
    void addn(double coeffn, int p, int q, int r, int s,
              double& n, double& x, double& y);
    void solarn(double& n);
    void planetary(double t);
};

class DIGIKAM_EXPORT Eclipse      // Eclipse Calculations
{
public:

    Eclipse();
    int solar(double jd, double tdut, double& phi, double& lamda);
    void maxpos(double jd, double tdut, double& phi, double& lamda);
    void penumd(double jd, double tdut, Vec3& vrm, Vec3& ves,
                double& dpn, double& pang);
    void umbra(double jd, double tdut, Vec3& vrm, Vec3& ves,
               double& dpn, double& pang);
    double duration(double jd, double tdut, double& width);
    Vec3 GetRSun();     // get Earth - Sun vector in Earth radii
    Vec3 GetRMoon();    // get Earth - Moon vector in Earth radii
    double GetEp2();    // get the ep2 value
    int lunar(double jd, double tdut);

private:

    Sun200   sun;
    Moon200  moon;
    Vec3     rs, rm;     // position of the Sun and the Moon
    Vec3     eshadow;    // unit vector in direction of shadow
    Vec3     rint;       // intersection shadow axis - Earth surface
    double   t;          // time in Julian Centuries
    double   ep2;        // correction for Apparent Sideral Time
    double   d_umbra;    // diameter of umbra in Earth Radii
    double   d_penumbra; // diameter of penumbra in Earth Radii

private:

    void equ_sun_moon(double jd, double tdut);
};

} // namespace Marble
