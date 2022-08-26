/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-02-22
 * Description : A text translator using web-services - Private members.
 *
 * Copyright (C) 2018-2022 by Hennadii Chernyshchyk <genaloner at gmail dot com>
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "donlinetranslator_p.h"

namespace Digikam
{

const QMap<DOnlineTranslator::Language, QString> DOnlineTranslator::Private::s_genericLanguageCodes =
{
    { DOnlineTranslator::Auto,                 QStringLiteral("auto")          },
    { DOnlineTranslator::Afrikaans,            QStringLiteral("af")            },
    { DOnlineTranslator::Albanian,             QStringLiteral("sq")            },
    { DOnlineTranslator::Amharic,              QStringLiteral("am")            },
    { DOnlineTranslator::Arabic,               QStringLiteral("ar")            },
    { DOnlineTranslator::Armenian,             QStringLiteral("hy")            },
    { DOnlineTranslator::Azerbaijani,          QStringLiteral("az")            },
    { DOnlineTranslator::Bashkir,              QStringLiteral("ba")            },
    { DOnlineTranslator::Basque,               QStringLiteral("eu")            },
    { DOnlineTranslator::Belarusian,           QStringLiteral("be")            },
    { DOnlineTranslator::Bengali,              QStringLiteral("bn")            },
    { DOnlineTranslator::Bosnian,              QStringLiteral("bs")            },
    { DOnlineTranslator::Bulgarian,            QStringLiteral("bg")            },
    { DOnlineTranslator::Cantonese,            QStringLiteral("yue")           },
    { DOnlineTranslator::Catalan,              QStringLiteral("ca")            },
    { DOnlineTranslator::Cebuano,              QStringLiteral("ceb")           },
    { DOnlineTranslator::Chichewa,             QStringLiteral("ny")            },
    { DOnlineTranslator::Corsican,             QStringLiteral("co")            },
    { DOnlineTranslator::Croatian,             QStringLiteral("hr")            },
    { DOnlineTranslator::Czech,                QStringLiteral("cs")            },
    { DOnlineTranslator::Danish,               QStringLiteral("da")            },
    { DOnlineTranslator::Dutch,                QStringLiteral("nl")            },
    { DOnlineTranslator::English,              QStringLiteral("en")            },
    { DOnlineTranslator::Esperanto,            QStringLiteral("eo")            },
    { DOnlineTranslator::Estonian,             QStringLiteral("et")            },
    { DOnlineTranslator::Fijian,               QStringLiteral("fj")            },
    { DOnlineTranslator::Filipino,             QStringLiteral("fil")           },
    { DOnlineTranslator::Finnish,              QStringLiteral("fi")            },
    { DOnlineTranslator::French,               QStringLiteral("fr")            },
    { DOnlineTranslator::Frisian,              QStringLiteral("fy")            },
    { DOnlineTranslator::Galician,             QStringLiteral("gl")            },
    { DOnlineTranslator::Georgian,             QStringLiteral("ka")            },
    { DOnlineTranslator::German,               QStringLiteral("de")            },
    { DOnlineTranslator::Greek,                QStringLiteral("el")            },
    { DOnlineTranslator::Gujarati,             QStringLiteral("gu")            },
    { DOnlineTranslator::HaitianCreole,        QStringLiteral("ht")            },
    { DOnlineTranslator::Hausa,                QStringLiteral("ha")            },
    { DOnlineTranslator::Hawaiian,             QStringLiteral("haw")           },
    { DOnlineTranslator::Hebrew,               QStringLiteral("he")            },
    { DOnlineTranslator::HillMari,             QStringLiteral("mrj")           },
    { DOnlineTranslator::Hindi,                QStringLiteral("hi")            },
    { DOnlineTranslator::Hmong,                QStringLiteral("hmn")           },
    { DOnlineTranslator::Hungarian,            QStringLiteral("hu")            },
    { DOnlineTranslator::Icelandic,            QStringLiteral("is")            },
    { DOnlineTranslator::Igbo,                 QStringLiteral("ig")            },
    { DOnlineTranslator::Indonesian,           QStringLiteral("id")            },
    { DOnlineTranslator::Irish,                QStringLiteral("ga")            },
    { DOnlineTranslator::Italian,              QStringLiteral("it")            },
    { DOnlineTranslator::Japanese,             QStringLiteral("ja")            },
    { DOnlineTranslator::Javanese,             QStringLiteral("jw")            },
    { DOnlineTranslator::Kannada,              QStringLiteral("kn")            },
    { DOnlineTranslator::Kazakh,               QStringLiteral("kk")            },
    { DOnlineTranslator::Khmer,                QStringLiteral("km")            },
    { DOnlineTranslator::Kinyarwanda,          QStringLiteral("rw")            },
    { DOnlineTranslator::Klingon,              QStringLiteral("tlh")           },
    { DOnlineTranslator::KlingonPlqaD,         QStringLiteral("tlh-Qaak")      },
    { DOnlineTranslator::Korean,               QStringLiteral("ko")            },
    { DOnlineTranslator::Kurdish,              QStringLiteral("ku")            },
    { DOnlineTranslator::Kyrgyz,               QStringLiteral("ky")            },
    { DOnlineTranslator::Lao,                  QStringLiteral("lo")            },
    { DOnlineTranslator::Latin,                QStringLiteral("la")            },
    { DOnlineTranslator::Latvian,              QStringLiteral("lv")            },
    { DOnlineTranslator::LevantineArabic,      QStringLiteral("apc")           },
    { DOnlineTranslator::Lithuanian,           QStringLiteral("lt")            },
    { DOnlineTranslator::Luxembourgish,        QStringLiteral("lb")            },
    { DOnlineTranslator::Macedonian,           QStringLiteral("mk")            },
    { DOnlineTranslator::Malagasy,             QStringLiteral("mg")            },
    { DOnlineTranslator::Malay,                QStringLiteral("ms")            },
    { DOnlineTranslator::Malayalam,            QStringLiteral("ml")            },
    { DOnlineTranslator::Maltese,              QStringLiteral("mt")            },
    { DOnlineTranslator::Maori,                QStringLiteral("mi")            },
    { DOnlineTranslator::Marathi,              QStringLiteral("mr")            },
    { DOnlineTranslator::Mari,                 QStringLiteral("mhr")           },
    { DOnlineTranslator::Mongolian,            QStringLiteral("mn")            },
    { DOnlineTranslator::Myanmar,              QStringLiteral("my")            },
    { DOnlineTranslator::Nepali,               QStringLiteral("ne")            },
    { DOnlineTranslator::Norwegian,            QStringLiteral("no")            },
    { DOnlineTranslator::Oriya,                QStringLiteral("or")            },
    { DOnlineTranslator::Papiamento,           QStringLiteral("pap")           },
    { DOnlineTranslator::Pashto,               QStringLiteral("ps")            },
    { DOnlineTranslator::Persian,              QStringLiteral("fa")            },
    { DOnlineTranslator::Polish,               QStringLiteral("pl")            },
    { DOnlineTranslator::Portuguese,           QStringLiteral("pt")            },
    { DOnlineTranslator::Punjabi,              QStringLiteral("pa")            },
    { DOnlineTranslator::QueretaroOtomi,       QStringLiteral("otq")           },
    { DOnlineTranslator::Romanian,             QStringLiteral("ro")            },
    { DOnlineTranslator::Russian,              QStringLiteral("ru")            },
    { DOnlineTranslator::Samoan,               QStringLiteral("sm")            },
    { DOnlineTranslator::ScotsGaelic,          QStringLiteral("gd")            },
    { DOnlineTranslator::SerbianCyrillic,      QStringLiteral("sr")            },
    { DOnlineTranslator::SerbianLatin,         QStringLiteral("sr-Latin")      },
    { DOnlineTranslator::Sesotho,              QStringLiteral("st")            },
    { DOnlineTranslator::Shona,                QStringLiteral("sn")            },
    { DOnlineTranslator::SimplifiedChinese,    QStringLiteral("zh-CN")         },
    { DOnlineTranslator::Sindhi,               QStringLiteral("sd")            },
    { DOnlineTranslator::Sinhala,              QStringLiteral("si")            },
    { DOnlineTranslator::Slovak,               QStringLiteral("sk")            },
    { DOnlineTranslator::Slovenian,            QStringLiteral("sl")            },
    { DOnlineTranslator::Somali,               QStringLiteral("so")            },
    { DOnlineTranslator::Spanish,              QStringLiteral("es")            },
    { DOnlineTranslator::Sundanese,            QStringLiteral("su")            },
    { DOnlineTranslator::Swahili,              QStringLiteral("sw")            },
    { DOnlineTranslator::Swedish,              QStringLiteral("sv")            },
    { DOnlineTranslator::Tagalog,              QStringLiteral("tl")            },
    { DOnlineTranslator::Tahitian,             QStringLiteral("ty")            },
    { DOnlineTranslator::Tajik,                QStringLiteral("tg")            },
    { DOnlineTranslator::Tamil,                QStringLiteral("ta")            },
    { DOnlineTranslator::Tatar,                QStringLiteral("tt")            },
    { DOnlineTranslator::Telugu,               QStringLiteral("te")            },
    { DOnlineTranslator::Thai,                 QStringLiteral("th")            },
    { DOnlineTranslator::Tongan,               QStringLiteral("to")            },
    { DOnlineTranslator::TraditionalChinese,   QStringLiteral("zh-TW")         },
    { DOnlineTranslator::Turkish,              QStringLiteral("tr")            },
    { DOnlineTranslator::Turkmen,              QStringLiteral("tk")            },
    { DOnlineTranslator::Udmurt,               QStringLiteral("udm")           },
    { DOnlineTranslator::Uighur,               QStringLiteral("ug")            },
    { DOnlineTranslator::Ukrainian,            QStringLiteral("uk")            },
    { DOnlineTranslator::Urdu,                 QStringLiteral("ur")            },
    { DOnlineTranslator::Uzbek,                QStringLiteral("uz")            },
    { DOnlineTranslator::Vietnamese,           QStringLiteral("vi")            },
    { DOnlineTranslator::Welsh,                QStringLiteral("cy")            },
    { DOnlineTranslator::Xhosa,                QStringLiteral("xh")            },
    { DOnlineTranslator::Yiddish,              QStringLiteral("yi")            },
    { DOnlineTranslator::Yoruba,               QStringLiteral("yo")            },
    { DOnlineTranslator::YucatecMaya,          QStringLiteral("yua")           },
    { DOnlineTranslator::Zulu,                 QStringLiteral("zu")            }
};

const QMap<DOnlineTranslator::Language, QString> DOnlineTranslator::Private::s_googleLanguageCodes =
{
    { DOnlineTranslator::Hebrew,               QStringLiteral("iw")            }
};

const QMap<DOnlineTranslator::Language, QString> DOnlineTranslator::Private::s_yandexLanguageCodes =
{
    { DOnlineTranslator::SimplifiedChinese,    QStringLiteral("zn")            },
    { DOnlineTranslator::Javanese,             QStringLiteral("jv")            }
};

const QMap<DOnlineTranslator::Language, QString> DOnlineTranslator::Private::s_bingLanguageCodes =
{
    { DOnlineTranslator::Auto,                 QStringLiteral("auto-detect")   },
    { DOnlineTranslator::Bosnian,              QStringLiteral("bs-Latn")       },
    { DOnlineTranslator::SerbianCyrillic,      QStringLiteral("sr-Cyrl")       },
    { DOnlineTranslator::SimplifiedChinese,    QStringLiteral("zh-Hans")       },
    { DOnlineTranslator::TraditionalChinese,   QStringLiteral("zh-Hant")       },
    { DOnlineTranslator::Hmong,                QStringLiteral("mww")           }
};

const QMap<DOnlineTranslator::Language, QString> DOnlineTranslator::Private::s_lingvaLanguageCodes =
{
    { DOnlineTranslator::SimplifiedChinese,    QStringLiteral("zh")            },
    { DOnlineTranslator::TraditionalChinese,   QStringLiteral("zh_HANT")       }
};

// --------------------------------------------------------------------------------------------

/*
 * NOTE: these lists of RFC3066 conversion map are mostly usee in AltLangStrEdit class.
 *       Any changes here must be also backported to the list used by AltLangStrEdit class.
 */

const QMap<QString, QString> DOnlineTranslator::Private::s_rfc3066LanguageCodesGeneric =
{
    { QLatin1String("af-ZA"),  QLatin1String("af")       },
    { QLatin1String("am-ET"),  QLatin1String("am")       },
//     { QLatin1String("")     ,  QLatin1String("apc")      },      // North Levantine Arabic: do not exists as 2 letters code
    { QLatin1String("ar-AE"),  QLatin1String("ar")       },
    { QLatin1String("ar-BH"),  QLatin1String("ar")       },
    { QLatin1String("ar-DZ"),  QLatin1String("ar")       },
    { QLatin1String("ar-EG"),  QLatin1String("ar")       },
    { QLatin1String("ar-IL"),  QLatin1String("ar")       },
    { QLatin1String("ar-IQ"),  QLatin1String("ar")       },
    { QLatin1String("ar-JO"),  QLatin1String("ar")       },
    { QLatin1String("ar-KW"),  QLatin1String("ar")       },
    { QLatin1String("ar-LB"),  QLatin1String("ar")       },
    { QLatin1String("ar-LY"),  QLatin1String("ar")       },
    { QLatin1String("ar-MA"),  QLatin1String("ar")       },
    { QLatin1String("ar-MR"),  QLatin1String("ar")       },
    { QLatin1String("ar-OM"),  QLatin1String("ar")       },
    { QLatin1String("ar-PS"),  QLatin1String("ar")       },
    { QLatin1String("ar-QA"),  QLatin1String("ar")       },
    { QLatin1String("ar-SA"),  QLatin1String("ar")       },
    { QLatin1String("ar-SD"),  QLatin1String("ar")       },
    { QLatin1String("ar-SO"),  QLatin1String("ar")       },
    { QLatin1String("ar-SY"),  QLatin1String("ar")       },
    { QLatin1String("ar-TD"),  QLatin1String("ar")       },
    { QLatin1String("ar-TN"),  QLatin1String("ar")       },
    { QLatin1String("ar-YE"),  QLatin1String("ar")       },
//     { QLatin1String("as-IN"),  QLatin1String("")         },
    { QLatin1String("az-AZ"),  QLatin1String("az")       },
    { QLatin1String("ba-RU"),  QLatin1String("ba")       },
    { QLatin1String("be-BY"),  QLatin1String("be")       },
    { QLatin1String("bn-BD"),  QLatin1String("bn")       },
    { QLatin1String("bn-IN"),  QLatin1String("bn")       },
    { QLatin1String("bn-SG"),  QLatin1String("bn")       },
//     { QLatin1String("bo-BT"),  QLatin1String("")         },
//     { QLatin1String("bo-CN"),  QLatin1String("")         },
    { QLatin1String("bs-BA"),  QLatin1String("bs")       },
    { QLatin1String("bg-BG"),  QLatin1String("bg")       },
//     { QLatin1String("br-FR"),  QLatin1String("")         },
//     { QLatin1String("")     ,  QLatin1String("yue")      },      // Yue Chinese: do not exists as 2 letters code
    { QLatin1String("ca-ES"),  QLatin1String("ca")       },
    { QLatin1String("ca-AD"),  QLatin1String("ca")       },
    { QLatin1String("ca-FR"),  QLatin1String("ca")       },
//     { QLatin1String("")     ,  QLatin1String("ceb")      },      // Cebuano: do not exists as 2 letters code
//    { QLatin1String("ch-GU"),  QLatin1String("")       },
//    { QLatin1String("ch-MP"),  QLatin1String("")       },
    { QLatin1String("co-FR"),  QLatin1String("co")       },
    { QLatin1String("cs-CZ"),  QLatin1String("cs")       },
    { QLatin1String("cy-GB"),  QLatin1String("cy")       },
    { QLatin1String("da-DK"),  QLatin1String("da")       },
    { QLatin1String("da-GL"),  QLatin1String("da")       },
    { QLatin1String("de-DE"),  QLatin1String("de")       },
    { QLatin1String("de-DK"),  QLatin1String("de")       },
    { QLatin1String("de-LI"),  QLatin1String("de")       },
    { QLatin1String("de-AT"),  QLatin1String("de")       },
    { QLatin1String("de-LU"),  QLatin1String("de")       },
    { QLatin1String("de-CH"),  QLatin1String("de")       },
    { QLatin1String("el-CY"),  QLatin1String("el")       },
    { QLatin1String("el-GR"),  QLatin1String("el")       },
    { QLatin1String("en-AG"),  QLatin1String("en")       },
    { QLatin1String("en-AI"),  QLatin1String("en")       },
    { QLatin1String("en-AS"),  QLatin1String("en")       },
    { QLatin1String("en-AU"),  QLatin1String("en")       },
    { QLatin1String("en-BB"),  QLatin1String("en")       },
    { QLatin1String("en-BM"),  QLatin1String("en")       },
    { QLatin1String("en-BN"),  QLatin1String("en")       },
    { QLatin1String("en-BS"),  QLatin1String("en")       },
    { QLatin1String("en-BW"),  QLatin1String("en")       },
    { QLatin1String("en-BZ"),  QLatin1String("en")       },
    { QLatin1String("en-CA"),  QLatin1String("en")       },
    { QLatin1String("en-CB"),  QLatin1String("en")       },
    { QLatin1String("en-CK"),  QLatin1String("en")       },
    { QLatin1String("en-CM"),  QLatin1String("en")       },
    { QLatin1String("en-DM"),  QLatin1String("en")       },
    { QLatin1String("en-ER"),  QLatin1String("en")       },
    { QLatin1String("en-ET"),  QLatin1String("en")       },
    { QLatin1String("en-FJ"),  QLatin1String("en")       },
    { QLatin1String("en-FK"),  QLatin1String("en")       },
    { QLatin1String("en-FM"),  QLatin1String("en")       },
    { QLatin1String("en-GD"),  QLatin1String("en")       },
    { QLatin1String("en-GH"),  QLatin1String("en")       },
    { QLatin1String("en-GI"),  QLatin1String("en")       },
    { QLatin1String("en-GM"),  QLatin1String("en")       },
    { QLatin1String("en-GU"),  QLatin1String("en")       },
    { QLatin1String("en-GY"),  QLatin1String("en")       },
    { QLatin1String("en-IE"),  QLatin1String("en")       },
    { QLatin1String("en-IL"),  QLatin1String("en")       },
    { QLatin1String("en-IN"),  QLatin1String("en")       },
    { QLatin1String("en-IO"),  QLatin1String("en")       },
    { QLatin1String("en-JM"),  QLatin1String("en")       },
    { QLatin1String("en-KR"),  QLatin1String("en")       },
    { QLatin1String("en-KI"),  QLatin1String("en")       },
    { QLatin1String("en-KN"),  QLatin1String("en")       },
    { QLatin1String("en-KY"),  QLatin1String("en")       },
    { QLatin1String("en-LC"),  QLatin1String("en")       },
    { QLatin1String("en-LR"),  QLatin1String("en")       },
    { QLatin1String("en-LS"),  QLatin1String("en")       },
    { QLatin1String("en-MP"),  QLatin1String("en")       },
    { QLatin1String("en-MS"),  QLatin1String("en")       },
    { QLatin1String("en-MT"),  QLatin1String("en")       },
    { QLatin1String("en-MU"),  QLatin1String("en")       },
    { QLatin1String("en-MW"),  QLatin1String("en")       },
    { QLatin1String("en-MY"),  QLatin1String("en")       },
    { QLatin1String("en-NA"),  QLatin1String("en")       },
    { QLatin1String("en-NF"),  QLatin1String("en")       },
    { QLatin1String("en-NG"),  QLatin1String("en")       },
    { QLatin1String("en-NR"),  QLatin1String("en")       },
    { QLatin1String("en-NU"),  QLatin1String("en")       },
    { QLatin1String("en-NZ"),  QLatin1String("en")       },
    { QLatin1String("en-PG"),  QLatin1String("en")       },
    { QLatin1String("en-PH"),  QLatin1String("en")       },
    { QLatin1String("en-PK"),  QLatin1String("en")       },
    { QLatin1String("en-PN"),  QLatin1String("en")       },
    { QLatin1String("en-PR"),  QLatin1String("en")       },
    { QLatin1String("en-PW"),  QLatin1String("en")       },
    { QLatin1String("en-RW"),  QLatin1String("en")       },
    { QLatin1String("en-SB"),  QLatin1String("en")       },
    { QLatin1String("en-SC"),  QLatin1String("en")       },
    { QLatin1String("en-SG"),  QLatin1String("en")       },
    { QLatin1String("en-SH"),  QLatin1String("en")       },
    { QLatin1String("en-SL"),  QLatin1String("en")       },
    { QLatin1String("en-SO"),  QLatin1String("en")       },
    { QLatin1String("en-SZ"),  QLatin1String("en")       },
    { QLatin1String("en-TC"),  QLatin1String("en")       },
    { QLatin1String("en-TK"),  QLatin1String("en")       },
    { QLatin1String("en-TO"),  QLatin1String("en")       },
    { QLatin1String("en-TT"),  QLatin1String("en")       },
    { QLatin1String("en-UG"),  QLatin1String("en")       },
    { QLatin1String("en-US"),  QLatin1String("en")       },
    { QLatin1String("en-VC"),  QLatin1String("en")       },
    { QLatin1String("en-VG"),  QLatin1String("en")       },
    { QLatin1String("en-VI"),  QLatin1String("en")       },
    { QLatin1String("en-VU"),  QLatin1String("en")       },
    { QLatin1String("en-WS"),  QLatin1String("en")       },
    { QLatin1String("en-ZA"),  QLatin1String("en")       },
    { QLatin1String("en-ZM"),  QLatin1String("en")       },
    { QLatin1String("en-ZW"),  QLatin1String("en")       },
    { QLatin1String("eo-FR"),  QLatin1String("eo")       },
    { QLatin1String("et-EE"),  QLatin1String("et")       },
    { QLatin1String("es-AR"),  QLatin1String("es")       },
    { QLatin1String("es-BO"),  QLatin1String("es")       },
    { QLatin1String("es-CL"),  QLatin1String("es")       },
    { QLatin1String("es-CQ"),  QLatin1String("es")       },
    { QLatin1String("es-CR"),  QLatin1String("es")       },
    { QLatin1String("es-CU"),  QLatin1String("es")       },
    { QLatin1String("es-DO"),  QLatin1String("es")       },
    { QLatin1String("es-EC"),  QLatin1String("es")       },
    { QLatin1String("es-ES"),  QLatin1String("es")       },
    { QLatin1String("es-GT"),  QLatin1String("es")       },
    { QLatin1String("es-HN"),  QLatin1String("es")       },
    { QLatin1String("es-MX"),  QLatin1String("es")       },
    { QLatin1String("es-NI"),  QLatin1String("es")       },
    { QLatin1String("es-PA"),  QLatin1String("es")       },
    { QLatin1String("es-PE"),  QLatin1String("es")       },
    { QLatin1String("es-PR"),  QLatin1String("es")       },
    { QLatin1String("es-PY"),  QLatin1String("es")       },
    { QLatin1String("es-SV"),  QLatin1String("es")       },
    { QLatin1String("es-US"),  QLatin1String("es")       },
    { QLatin1String("es-UY"),  QLatin1String("es")       },
    { QLatin1String("es-VE"),  QLatin1String("es")       },
    { QLatin1String("eu-ES"),  QLatin1String("eu")       },
    { QLatin1String("fa-IR"),  QLatin1String("fa")       },
//     { QLatin1String("")     ,  QLatin1String("fil")      },      // Filipino: do not exists as 2 letters code
    { QLatin1String("fi-FI"),  QLatin1String("fi")       },
    { QLatin1String("fj-FJ"),  QLatin1String("fj")       },
    { QLatin1String("fr-AD"),  QLatin1String("fr")       },
    { QLatin1String("fr-BF"),  QLatin1String("fr")       },
    { QLatin1String("fr-BI"),  QLatin1String("fr")       },
    { QLatin1String("fr-BJ"),  QLatin1String("fr")       },
    { QLatin1String("fr-BE"),  QLatin1String("fr")       },
    { QLatin1String("fr-CA"),  QLatin1String("fr")       },
    { QLatin1String("fr-CD"),  QLatin1String("fr")       },
    { QLatin1String("fr-CH"),  QLatin1String("fr")       },
    { QLatin1String("fr-CI"),  QLatin1String("fr")       },
    { QLatin1String("fr-CM"),  QLatin1String("fr")       },
    { QLatin1String("fr-DJ"),  QLatin1String("fr")       },
    { QLatin1String("fr-FR"),  QLatin1String("fr")       },
    { QLatin1String("fr-GA"),  QLatin1String("fr")       },
    { QLatin1String("fr-GB"),  QLatin1String("fr")       },
    { QLatin1String("fr-GF"),  QLatin1String("fr")       },
    { QLatin1String("fr-GN"),  QLatin1String("fr")       },
    { QLatin1String("fr-GP"),  QLatin1String("fr")       },
    { QLatin1String("fr-HT"),  QLatin1String("fr")       },
    { QLatin1String("fr-IT"),  QLatin1String("fr")       },
    { QLatin1String("fr-KM"),  QLatin1String("fr")       },
    { QLatin1String("fr-LB"),  QLatin1String("fr")       },
    { QLatin1String("fr-LU"),  QLatin1String("fr")       },
    { QLatin1String("fr-MC"),  QLatin1String("fr")       },
    { QLatin1String("fr-MG"),  QLatin1String("fr")       },
    { QLatin1String("fr-ML"),  QLatin1String("fr")       },
    { QLatin1String("fr-MQ"),  QLatin1String("fr")       },
    { QLatin1String("fr-NC"),  QLatin1String("fr")       },
    { QLatin1String("fr-PF"),  QLatin1String("fr")       },
    { QLatin1String("fr-PM"),  QLatin1String("fr")       },
    { QLatin1String("fr-RE"),  QLatin1String("fr")       },
    { QLatin1String("fr-RW"),  QLatin1String("fr")       },
    { QLatin1String("fr-SC"),  QLatin1String("fr")       },
    { QLatin1String("fr-TD"),  QLatin1String("fr")       },
    { QLatin1String("fr-TG"),  QLatin1String("fr")       },
    { QLatin1String("fr-VU"),  QLatin1String("fr")       },
    { QLatin1String("fr-WF"),  QLatin1String("fr")       },
    { QLatin1String("fr-YT"),  QLatin1String("fr")       },
    { QLatin1String("fy-NL"),  QLatin1String("fy")       },
    { QLatin1String("ga-GB"),  QLatin1String("ga")       },
    { QLatin1String("ga-IE"),  QLatin1String("ga")       },
    { QLatin1String("gd-GB"),  QLatin1String("gd")       },
    { QLatin1String("gl-ES"),  QLatin1String("gl")       },
//    { QLatin1String("gv-GB"),  QLatin1String("")          },
    { QLatin1String("gu-IN"),  QLatin1String("gu")       },
    { QLatin1String("ha-TD"),  QLatin1String("ha")       },
//     { QLatin1String("")     ,  QLatin1String("haw")      },      // Hawaiia: do not exists as 2 letters code
    { QLatin1String("he-IL"),  QLatin1String("he")       },
    { QLatin1String("hi-IN"),  QLatin1String("hi")       },
    { QLatin1String("hr-HR"),  QLatin1String("hr")       },
    { QLatin1String("hr-BA"),  QLatin1String("hr")       },
//     { QLatin1String("")     ,  QLatin1String("hmn")      },      // Hmong: do not exists as 2 letters code
    { QLatin1String("ht-FR"),  QLatin1String("ht")       },
    { QLatin1String("hu-HU"),  QLatin1String("hu")       },
    { QLatin1String("hu-SI"),  QLatin1String("hu")       },
    { QLatin1String("hy-AM"),  QLatin1String("hy")       },
    { QLatin1String("id-ID"),  QLatin1String("id")       },
    { QLatin1String("ig-NG"),  QLatin1String("ig")       },
    { QLatin1String("is-IS"),  QLatin1String("is")       },
//    { QLatin1String("ii-CN"),  QLatin1String("")         },
    { QLatin1String("it-CH"),  QLatin1String("it")       },
    { QLatin1String("it-HR"),  QLatin1String("it")       },
    { QLatin1String("it-IT"),  QLatin1String("it")       },
    { QLatin1String("it-SI"),  QLatin1String("it")       },
    { QLatin1String("it-SM"),  QLatin1String("it")       },
    { QLatin1String("ja-JP"),  QLatin1String("ja")       },
    { QLatin1String("jw-ID"),  QLatin1String("jw")       },
    { QLatin1String("ka-GE"),  QLatin1String("ka")       },
    { QLatin1String("kn-IN"),  QLatin1String("kn")       },
    { QLatin1String("kk-KZ"),  QLatin1String("kk")       },
//     { QLatin1String("kl-GL"),  QLatin1String("")         },
    { QLatin1String("km-KH"),  QLatin1String("km")       },
    { QLatin1String("ko-KR"),  QLatin1String("ko")       },
    { QLatin1String("ko-KP"),  QLatin1String("ko")       },
    { QLatin1String("ku-IR"),  QLatin1String("ku")       },
    { QLatin1String("ky-KG"),  QLatin1String("ky")       },
//    { QLatin1String("kw-GB"),  QLatin1String("")          },
    { QLatin1String("la-IT"),  QLatin1String("la")       },
//    { QLatin1String("ln-CD"),  QLatin1String("")          },
//    { QLatin1String("ln-CG"),  QLatin1String("")          },
    { QLatin1String("lo-LA"),  QLatin1String("lo")       },
    { QLatin1String("lb-LU"),  QLatin1String("lb")       },
    { QLatin1String("lt-LT"),  QLatin1String("lt")       },
    { QLatin1String("lv-LV"),  QLatin1String("lv")       },
    { QLatin1String("mg-MG"),  QLatin1String("mg")       },
    { QLatin1String("mi-NZ"),  QLatin1String("mi")       },
    { QLatin1String("mk-MK"),  QLatin1String("mk")       },
    { QLatin1String("ml-IN"),  QLatin1String("ml")       },
//     { QLatin1String("")     ,  QLatin1String("mhr")      },      // Meadow Mari language: do not exists as 2 letters code
    { QLatin1String("mn-MN"),  QLatin1String("mn")       },
    { QLatin1String("mn-CN"),  QLatin1String("mn")       },
    { QLatin1String("mr-IN"),  QLatin1String("mr")       },
//     { QLatin1String("")     ,  QLatin1String("mrj")      },      // Western Mari: do not exists as 2 letters code
    { QLatin1String("ms-BN"),  QLatin1String("ms")         },
    { QLatin1String("ms-MY"),  QLatin1String("ms")       },
    { QLatin1String("ms-SG"),  QLatin1String("ms")       },
    { QLatin1String("mt-MT"),  QLatin1String("mt")       },
    { QLatin1String("my-MM"),  QLatin1String("my")       },
//     { QLatin1String("nb-NO"),  QLatin1String("")         },
    { QLatin1String("ne-NP"),  QLatin1String("ne")       },
    { QLatin1String("nl-AN"),  QLatin1String("nl")       },
    { QLatin1String("nl-AW"),  QLatin1String("nl")       },
    { QLatin1String("nl-NL"),  QLatin1String("nl")       },
    { QLatin1String("nl-BE"),  QLatin1String("nl")       },
    { QLatin1String("nl-SR"),  QLatin1String("nl")       },
//     { QLatin1String("nn-NO"),  QLatin1String("")         },
    { QLatin1String("no-NO"),  QLatin1String("no")       },
//     { QLatin1String("ns-ZA"),  QLatin1String("")         },
    { QLatin1String("ny-ZM"),  QLatin1String("ny")       },
//     { QLatin1String("oc-FR"),  QLatin1String("")         },
    { QLatin1String("or-IN"),  QLatin1String("or")       },
//     { QLatin1String("")     ,  QLatin1String("pap")      },      // Papiamento: do not exists as 2 letters code
    { QLatin1String("pa-IN"),  QLatin1String("pa")       },
    { QLatin1String("pl-PL"),  QLatin1String("pl")       },
    { QLatin1String("pt-AO"),  QLatin1String("pt")         },
    { QLatin1String("pt-BR"),  QLatin1String("pt")         },
    { QLatin1String("pt-CV"),  QLatin1String("pt")         },
    { QLatin1String("pt-GW"),  QLatin1String("pt")         },
    { QLatin1String("pt-MZ"),  QLatin1String("pt")         },
    { QLatin1String("pt-PT"),  QLatin1String("pt")       },
    { QLatin1String("pt-ST"),  QLatin1String("pt")       },
    { QLatin1String("ps-AF"),  QLatin1String("ps")       },
//     { QLatin1String("")     ,  QLatin1String("otq")      },      // Otomi Querétaro:  do not exists as 2 letters code
    { QLatin1String("ro-RO"),  QLatin1String("ro")       },
    { QLatin1String("ru-RU"),  QLatin1String("ru")       },
    { QLatin1String("rw-RW"),  QLatin1String("rw")       },
//     { QLatin1String("sa-IN"),  QLatin1String("")         },
    { QLatin1String("sd-PK"),  QLatin1String("sd")       },
//     { QLatin1String("se-FI"),  QLatin1String("")         },
//     { QLatin1String("se-NO"),  QLatin1String("")         },
//     { QLatin1String("se-SE"),  QLatin1String("")         },
    { QLatin1String("si-LK"),  QLatin1String("si")       },
    { QLatin1String("sk-SK"),  QLatin1String("sk")       },
    { QLatin1String("sl-SI"),  QLatin1String("sl")       },
    { QLatin1String("sm-WS"),  QLatin1String("sm")       },
    { QLatin1String("sn-ZW"),  QLatin1String("sn")       },
    { QLatin1String("so-SO"),  QLatin1String("so")       },
    { QLatin1String("sq-AL"),  QLatin1String("sq")       },
    { QLatin1String("sr-BA"),  QLatin1String("sr")       },
    { QLatin1String("sr-YU"),  QLatin1String("sr")       },
    { QLatin1String("sr-SP"),  QLatin1String("sr-Latin") },
//     { QLatin1String("ss-SZ")     ,  QLatin1String("")       },
//     { QLatin1String("ss-ZA")     ,  QLatin1String("")       },
    { QLatin1String("st-LS"),  QLatin1String("st")       },
    { QLatin1String("su-ID"),  QLatin1String("su")       },
    { QLatin1String("sv-FI"),  QLatin1String("sv")       },
    { QLatin1String("sv-SE"),  QLatin1String("sv")       },
    { QLatin1String("sw-KE"),  QLatin1String("sw")       },
    { QLatin1String("sw-TZ"),  QLatin1String("sw")       },
    { QLatin1String("ta-IN"),  QLatin1String("ta")       },
    { QLatin1String("ta-SG"),  QLatin1String("ta")       },
    { QLatin1String("te-IN"),  QLatin1String("te")       },
    { QLatin1String("tg-TJ"),  QLatin1String("tg")       },
    { QLatin1String("th-TH"),  QLatin1String("th")       },
    { QLatin1String("tk-TM"),  QLatin1String("tk")       },
    { QLatin1String("tl-PH"),  QLatin1String("tl")       },
    { QLatin1String("to-TO"),  QLatin1String("to")       },
//     { QLatin1String("")     ,  QLatin1String("tlh")      },      // Klingon tlhIngan-Hol: do not exists as 2 letters code
//     { QLatin1String("")     ,  QLatin1String("tlh-Qaak") },      // Klingon tlhIngan-Hol: do not exists as 2 letters code
//     { QLatin1String("tn-ZA"),  QLatin1String("")         },
    { QLatin1String("tr-BG"),  QLatin1String("tr")       },
    { QLatin1String("tr-CY"),  QLatin1String("tr")       },
    { QLatin1String("tr-TR"),  QLatin1String("tr")       },
    { QLatin1String("tt-RU"),  QLatin1String("tt")       },
    { QLatin1String("ty-PF"),  QLatin1String("ty")       },
//     { QLatin1String("")     ,  QLatin1String("udm")      },      // Udmurt: do not exists as 2 letters code
    { QLatin1String("ug-CN"),  QLatin1String("ug")       },
    { QLatin1String("uk-UA"),  QLatin1String("uk")       },
    { QLatin1String("ur-IN"),  QLatin1String("ur")       },
    { QLatin1String("ur-PK"),  QLatin1String("ur")       },
    { QLatin1String("uz-UZ"),  QLatin1String("uz")       },
    { QLatin1String("vi-VN"),  QLatin1String("vi")       },
//     { QLatin1String("wo-SN"),  QLatin1String("")         },
    { QLatin1String("xh-ZA"),  QLatin1String("xh")       },
    { QLatin1String("yi-DE"),  QLatin1String("yi")       },
    { QLatin1String("yo-NG"),  QLatin1String("yo")       },
//     { QLatin1String("")     ,  QLatin1String("yua")      },      // Maya Yucatec: do not exists as 2 letters code
    { QLatin1String("zh-CN"),  QLatin1String("zh-CN")    },         // Chinese (Simplified, PRC)
    { QLatin1String("zh-TW"),  QLatin1String("zh-TW")    },         // Chinese (Traditional, Taiwan)
    { QLatin1String("zu-ZA"),  QLatin1String("zu")       }
};

// --------------------------------------------------------------------------------------------

const QMap<QString, QString> DOnlineTranslator::Private::s_rfc3066LanguageCodesGoogle =
{
    { QLatin1String("iw-IL"),  QLatin1String("iw")       }          //                                            NOTE: Google translator only.
};

// --------------------------------------------------------------------------------------------

const QMap<QString, QString> DOnlineTranslator::Private::s_rfc3066LanguageCodesYandex =
{
    { QLatin1String("jv-ID"),  QLatin1String("jv")       },         //                                            NOTE: Yandex translator only
    { QLatin1String("zn-CN"),  QLatin1String("zn")       }          //                                            NOTE: Yandex translator only
};

// --------------------------------------------------------------------------------------------

const QMap<QString, QString> DOnlineTranslator::Private::s_rfc3066LanguageCodesBing =
{
    { QLatin1String("bs-BG"),  QLatin1String("bs-Latn")  },         //                                            NOTE: Bing translator only
//     { QLatin1String("")     ,  QLatin1String("mww")      },      // Hmong: do not exists as 2 letters code   ; NOTE: Bing translator only
    { QLatin1String("sr-RS"),  QLatin1String("sr-Cyrl")  },         //                                            NOTE: Bing translator only
    { QLatin1String("zh-CN"),  QLatin1String("zh-Hans")  },         // SimplifiedChinese                        ; NOTE: Bing translator only
    { QLatin1String("zh-TW"),  QLatin1String("zh-Hant")  }          // TraditionalChinese                       ; NOTE: Bing translator only
};

// --------------------------------------------------------------------------------------------

const QMap<QString, QString> DOnlineTranslator::Private::s_rfc3066LanguageCodesLingva =
{
    { QLatin1String("zh-TW"),  QLatin1String("zh_HANT")  },         // TraditionalChinese                       ; NOTE: Lingva translator only
    { QLatin1String("zh-MO"),  QLatin1String("zh")       },         // Chinese (Traditional, Macao S.A.R.)      ; NOTE: Lingva translator only
    { QLatin1String("zh-HK"),  QLatin1String("zh")       },         // Chinese (Traditional, Hong Kong S.A.R.)  ; NOTE: Lingva translator only
    { QLatin1String("zh-SG"),  QLatin1String("zh")       }          // Chinese (Simplified, Singapore)          ; NOTE: Lingva translator only
};

// --------------------------------------------------------------------------------------------

DOnlineTranslator::Private::Private(DOnlineTranslator* const parent)
    : stateMachine  (new QStateMachine(parent)),
      networkManager(new QNetworkAccessManager(parent)),
      libreUrl      (QLatin1String("https://translate.argosopentech.com")),
      lingvaUrl     (QLatin1String("https://lingva.ml"))
{
}

// --------------------------------------------------------------------------------------------

const QMap<DOnlineTts::Emotion, QString> DOnlineTts::Private::s_emotionCodes =
{
    { Neutral, QStringLiteral("neutral") },
    { Good,    QStringLiteral("good")    },
    { Evil,    QStringLiteral("evil")    }
};

const QMap<DOnlineTts::Voice, QString> DOnlineTts::Private::s_voiceCodes =
{
    { Zahar,   QStringLiteral("zahar")   },
    { Ermil,   QStringLiteral("ermil")   },
    { Jane,    QStringLiteral("jane")    },
    { Oksana,  QStringLiteral("oksana")  },
    { Alyss,   QStringLiteral("alyss")   },
    { Omazh,   QStringLiteral("omazh")   }
};

} // namespace Digikam
