/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-03-22
 * Description : a MediaWiki C++ interface
 *
 * SPDX-FileCopyrightText: 2011-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2011      by Alexandre Mendes <alex dot mendes1988 at gmail dot com>
 * SPDX-FileCopyrightText: 2011      by Hormiere Guillaume <hormiere dot guillaume at gmail dot com>
 * SPDX-FileCopyrightText: 2011      by Manuel Campomanes <campomanes dot manuel at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

// Qt includes

#include <QList>
#include <QObject>
#include <QString>
#include <QUrl>
#include <QTest>

// KDE includes

#include <kjob.h>

// Local includes

#include "fakeserver/fakeserver.h"
#include "mediawiki_iface.h"
#include "mediawiki_querysiteinfousergroups.h"
#include "mediawiki_usergroup.h"

using MediaWiki::Iface;
using MediaWiki::QuerySiteinfoUsergroups;
using MediaWiki::UserGroup;

Q_DECLARE_METATYPE(QList<UserGroup>)

class Q_DECL_HIDDEN QuerySiteinfoUsergroupsTest : public QObject
{
    Q_OBJECT

public:

    explicit QuerySiteinfoUsergroupsTest(QObject* const parent = nullptr)
    {
        Q_UNUSED(parent);
        usergroupsCount = 0;
    }

public Q_SLOTS:

    void usergroupsHandle(const QList<UserGroup> & usergroups)
    {
        ++usergroupsCount;
        usergroupsResults = usergroups;
    }

private Q_SLOTS:

    void init()
    {
        usergroupsCount = 0;
    }

    void testResult()
    {
        QFETCH(QString, scenario);
        QFETCH(bool, includeNumber);
        QFETCH(int, error);
        QFETCH(QList<UserGroup>, results);

        FakeServer fakeserver;
        fakeserver.setScenario(scenario);
        fakeserver.startAndWait();

        Iface MediaWiki(QUrl(QStringLiteral("http://127.0.0.1:12566")));        // krazy:exclude=insecurenet
        QuerySiteinfoUsergroups* const job = new QuerySiteinfoUsergroups(MediaWiki);

        job->setIncludeNumber(includeNumber);

        connect(job, SIGNAL(usergroups(QList<UserGroup>)),
                this, SLOT(usergroupsHandle(QList<UserGroup>)));

        job->exec();

        QList<FakeServer::Request> requests = fakeserver.getRequest();
        QCOMPARE(requests.size(), 1);

        FakeServer::Request request = requests[0];
        QCOMPARE(request.agent, MediaWiki.userAgent());
        QCOMPARE(request.type, QStringLiteral("GET"));

        if (includeNumber)
        {
            QCOMPARE(request.value, QStringLiteral("/?format=xml&action=query&meta=siteinfo&siprop=usergroups&sinumberingroup"));
        }
        else
        {
            QCOMPARE(request.value, QStringLiteral("/?format=xml&action=query&meta=siteinfo&siprop=usergroups"));
        }

        QCOMPARE(job->error(), error);

        QCOMPARE(usergroupsCount, 1);

        QCOMPARE(usergroupsResults, results);

        QVERIFY(fakeserver.isAllScenarioDone());
    }

    void testResult_data()
    {
        QTest::addColumn<QString>("scenario");
        QTest::addColumn<bool>("includeNumber");
        QTest::addColumn<int>("error");
        QTest::addColumn< QList<UserGroup> >("results");

        UserGroup ug1,ug2,ug3;
        ug1.setName(QStringLiteral("name_1"));
        ug1.setRights(QList<QString>());
        ug2.setName(QStringLiteral("name_2"));
        ug2.setRights(QList<QString>());
        ug3.setName(QStringLiteral("name_3"));

        QTest::newRow("No group")
                << QStringLiteral("<?xml version=\"1.0\"?><api><query><usergroups></usergroups></query></api>")
                << false
                << int(KJob::NoError)
                << QList<UserGroup>();

        QTest::newRow("One group with no right")
                << QStringLiteral("<?xml version=\"1.0\"?><api><query><usergroups><group name=\"name_1\"/></usergroups></query></api>")
                << false
                << int(KJob::NoError)
                << (QList<UserGroup>() << ug1);

        ug1.setRights(ug1.rights() << QStringLiteral("permission_1_1"));
        QTest::newRow("One group with one right")
                << QStringLiteral("<?xml version=\"1.0\"?><api><query><usergroups><group name=\"name_1\"><rights><permission>permission_1_1</permission></rights></group></usergroups></query></api>")
                << false
                << int(KJob::NoError)
                << (QList<UserGroup>() << ug1);

        ug1.setRights(ug1.rights() << QStringLiteral("permission_1_2"));
        QTest::newRow("One group with two rights")
                << QStringLiteral("<?xml version=\"1.0\"?><api><query><usergroups><group name=\"name_1\"><rights><permission>permission_1_1</permission><permission>permission_1_2</permission></rights></group></usergroups></query></api>")
                << false
                << int(KJob::NoError)
                << (QList<UserGroup>() << ug1);

        ug1.setRights(QList<QString>());
        QTest::newRow("Two groups with group one no right and group two no right")
                << QStringLiteral("<?xml version=\"1.0\"?><api><query><usergroups><group name=\"name_1\"></group><group name=\"name_2\" /></usergroups></query></api>")
                << false
                << int(KJob::NoError)
                << (QList<UserGroup>() << ug1 << ug2);

        ug2.setRights(ug2.rights() << QStringLiteral("permission_2_1"));
        QTest::newRow("Two groups with group one no right and group two one right")
                << QStringLiteral("<?xml version=\"1.0\"?><api><query><usergroups><group name=\"name_1\"></group><group name=\"name_2\"><rights><permission>permission_2_1</permission></rights></group></usergroups></query></api>")
                << false
                << int(KJob::NoError)
                << (QList<UserGroup>() << ug1 << ug2);

        ug2.setRights(ug2.rights() << QStringLiteral("permission_2_2"));
        QTest::newRow("Two groups with group one no right and group two two rights")    // krazy:exclude=spelling
                << QStringLiteral("<?xml version=\"1.0\"?><api><query><usergroups><group name=\"name_1\"></group><group name=\"name_2\"><rights><permission>permission_2_1</permission><permission>permission_2_2</permission></rights></group></usergroups></query></api>")
                << false
                << int(KJob::NoError)
                << (QList<UserGroup>() << ug1 << ug2);

        ug1.setRights(ug1.rights() << QStringLiteral("permission_1_1"));
        ug2.setRights(QList<QString>());
        QTest::newRow("Two groups with group one one right and group two no right")     // krazy:exclude=spelling
                << QStringLiteral("<?xml version=\"1.0\"?><api><query><usergroups><group name=\"name_1\"><rights><permission>permission_1_1</permission></rights></group><group name=\"name_2\"><rights></rights></group></usergroups></query></api>")
                << false
                << int(KJob::NoError)
                << (QList<UserGroup>() << ug1 << ug2);

        ug2.setRights(ug2.rights() << QStringLiteral("permission_2_1"));
        QTest::newRow("Two groups with group one one right and group two one right")    // krazy:exclude=spelling
                << QStringLiteral("<?xml version=\"1.0\"?><api><query><usergroups><group name=\"name_1\"><rights><permission>permission_1_1</permission></rights></group><group name=\"name_2\"><rights><permission>permission_2_1</permission></rights></group></usergroups></query></api>")
                << false
                << int(KJob::NoError)
                << (QList<UserGroup>() << ug1 << ug2);

        ug2.setRights(ug2.rights() << QStringLiteral("permission_2_2"));
        QTest::newRow("Two groups with group one one right and group two two rights")   // krazy:exclude=spelling
                << QStringLiteral("<?xml version=\"1.0\"?><api><query><usergroups><group name=\"name_1\"><rights><permission>permission_1_1</permission></rights></group><group name=\"name_2\"><rights><permission>permission_2_1</permission><permission>permission_2_2</permission></rights></group></usergroups></query></api>")
                << false
                << int(KJob::NoError)
                << (QList<UserGroup>() << ug1 << ug2);

        ug1.setRights(ug1.rights() << QStringLiteral("permission_1_2"));
        ug2.setRights(QList<QString>());
        QTest::newRow("Two groups with group one two rights and group two no right")
                << QStringLiteral("<?xml version=\"1.0\"?><api><query><usergroups><group name=\"name_1\"><rights><permission>permission_1_1</permission><permission>permission_1_2</permission></rights></group><group name=\"name_2\"><rights /></group></usergroups></query></api>")
                << false
                << int(KJob::NoError)
                << (QList<UserGroup>() << ug1 << ug2);

        ug2.setRights(ug2.rights() << QStringLiteral("permission_2_1"));
        QTest::newRow("Two groups with group one two rights and group two one right")
                << QStringLiteral("<?xml version=\"1.0\"?><api><query><usergroups><group name=\"name_1\"><rights><permission>permission_1_1</permission><permission>permission_1_2</permission></rights></group><group name=\"name_2\"><rights><permission>permission_2_1</permission></rights></group></usergroups></query></api>")
                << false
                << int(KJob::NoError)
                << (QList<UserGroup>() << ug1 << ug2);

        ug2.setRights(ug2.rights() << QStringLiteral("permission_2_2"));
        QTest::newRow("Two groups with group one two rights and group two two rights")  // krazy:exclude=spelling
                << QStringLiteral("<?xml version=\"1.0\"?><api><query><usergroups><group name=\"name_1\"><rights><permission>permission_1_1</permission><permission>permission_1_2</permission></rights></group><group name=\"name_2\"><rights><permission>permission_2_1</permission><permission>permission_2_2</permission></rights></group></usergroups></query></api>")
                << false
                << int(KJob::NoError)
                << (QList<UserGroup>() << ug1 << ug2);

        QTest::newRow("No group with include number")
                << QStringLiteral("<?xml version=\"1.0\"?><api><query><usergroups></usergroups></query></api>")
                << true
                << int(KJob::NoError)
                << QList<UserGroup>();

        ug1.setRights(QList<QString>());
        ug1.setNumber(0);
        QTest::newRow("One group with include number")
                << QStringLiteral("<?xml version=\"1.0\"?><api><query><usergroups><group name=\"name_1\" number=\"0\"/></usergroups></query></api>")
                << true
                << int(KJob::NoError)
                << (QList<UserGroup>() << ug1);

        ug1.setNumber(41);
        ug2.setRights(QList<QString>());
        ug2.setNumber(12543);
        QTest::newRow("Two groups with include number")
                << QStringLiteral("<?xml version=\"1.0\"?><api><query><usergroups><group name=\"name_1\" number=\"41\"></group><group name=\"name_2\" number=\"12543\" /></usergroups></query></api>")
                << true
                << int(KJob::NoError)
                << (QList<UserGroup>() << ug1 << ug2);

        ug1.setRights(QList<QString>()<< QStringLiteral("permission_1_1"));
        ug1.setNumber(1781);
        ug2.setRights(QList<QString>() << QStringLiteral("permission_2_1") << QStringLiteral("permission_2_2") << QStringLiteral("permission_2_3"));
        ug2.setNumber(10989982);
        ug3.setRights(QList<QString>() << QStringLiteral("permission_3_1") << QStringLiteral("permission_3_2"));
        ug3.setNumber(6);
        QTest::newRow("Three groups with rights and include number")
                << QStringLiteral("<?xml version=\"1.0\"?><api><query><usergroups><group name=\"name_1\" number=\"1781\"><rights><permission>permission_1_1</permission></rights></group><group name=\"name_2\" number=\"10989982\"><rights><permission>permission_2_1</permission><permission>permission_2_2</permission><permission>permission_2_3</permission></rights></group><group name=\"name_3\" number=\"6\"><rights><permission>permission_3_1</permission><permission>permission_3_2</permission></rights></group></usergroups></query></api>")
                << true
                << int(KJob::NoError)
                << (QList<UserGroup>() << ug1 << ug2 << ug3);
    }

private:

    int              usergroupsCount;
    QList<UserGroup> usergroupsResults;
};

QTEST_MAIN(QuerySiteinfoUsergroupsTest)

#include "querysiteinfousergroups_utest.moc"
