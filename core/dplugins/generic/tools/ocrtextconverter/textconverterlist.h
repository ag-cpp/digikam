#ifndef DIGIKAM_TEXT_CONVERTER_LIST_H
#define DIGIKAM_TEXT_CONVERTER_LIST_H

// Qt includes

#include <QUrl>

// Local includes

#include "ditemslist.h"

using namespace Digikam;

namespace DigikamGenericTextConverterPlugin
{

class TextConverterListViewItem;

class TextConverterList : public DItemsList
{
    Q_OBJECT

public:

    /**
     * List the different columns in a list.
     */
    enum FieldType
    {
        RAWFILENAME      = DItemsListView::User1,
        TARGETFILENAME   = DItemsListView::User2,
        STATUS           = DItemsListView::User3,
        IDENTIFICATION   = DItemsListView::User4 
    };

public:

    explicit TextConverterList(QWidget* const parent = nullptr);
    ~TextConverterList();


public Q_SLOTS:

    void slotAddImages(const QList<QUrl>& list)    override; 

protected Q_SLOTS:

    void slotRemoveItems()                         override;
};

// -------------------------------------------------------------------------

class TextConverterListViewItem : public DItemsListViewItem
{

public:

    explicit TextConverterListViewItem(DItemsListView* const view, const QUrl& url);
    ~TextConverterListViewItem();

    void setDestFileName(const QString& str);
    QString destFileName()  const;

    void setIdentity(const QString& str);
    QString identity()      const;

    void setStatus(const QString& str);

    QString destPath()      const;
    
private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericTextConverterPlugin


#endif // DIGIKAM_TEXT_CONVERTER_LIST_H
