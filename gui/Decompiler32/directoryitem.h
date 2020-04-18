#ifndef DIRECTORYITEM_H
#define DIRECTORYITEM_H

#include <QObject>

/// An item that belongs in a directory. Can be a file or another directory
class DirectoryItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ get_name NOTIFY name_changed) ///< The name of this item
public:
    explicit DirectoryItem(QObject *parent = nullptr);
    static void qml_register(); ///< This registers a singleton instance of the class that QML is responsible for creating and owning.
    /*! Return the name of the item. */
    QString get_name () { return name; }
    /*! Temporary function to set the name of the object*/
    void set_name(QString n){ name = n; }
signals:
    void name_changed();
protected:
    QString name;
};

#endif // DIRECTORYITEM_H
