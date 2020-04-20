#ifndef DIRECTORYITEM_H
#define DIRECTORYITEM_H

#include <QObject>

/// An item that belongs in a directory. Can be a file or another directory
class DirectoryItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ get_name NOTIFY name_changed) ///< The name of this item
    Q_PROPERTY(int size READ get_size NOTIFY size_changed) ///< The size of the item
    Q_PROPERTY(bool IsDir READ get_is_dir NOTIFY dir_changed) ///< Defines if the item is a directory
    Q_PROPERTY(QList<QObject*> DirContents READ get_dir_contents NOTIFY dir_contents_changed) ///< The files contained in this directory (if it is a directory).
public:
    explicit DirectoryItem(QObject *parent = nullptr);
    static void qml_register(); ///< This registers a singleton instance of the class that QML is responsible for creating and owning.
    /*! Return the name of the item. */
    QString get_name () { return name; }
    /*! Return the size of the item. Maybe not meaningful for directories. */
    int get_size() { return size; }
    /*! Temporary function to set the name of the object*/
    void set_name(QString n){ name = n; }
    void set_parent_directory(QList<DirectoryItem*>* p); ///< Sets the parent directory
    void set_directory(); ///< Makes this a directory instead of a plain file
    /*! Returns true when this item is a directory. */
    bool get_is_dir() { return is_dir; }
    /*! Return the list of directory contents, if it is a directory. */
    QList<QObject *> get_dir_contents();
    void add_dir_item(DirectoryItem* i); ///< Adds an item to the list of directory items, if this is a directory.
signals:
    void name_changed(); ///< The name changed
    void size_changed(); ///< The size changed
    void dir_changed(); ///< The is_dir flag changed
    void dir_contents_changed(); ///< The contents of the directory changed (if this item is a directory)
protected:
    QString name;
    bool is_dir;
    QList<DirectoryItem*> dir_elements;
    QList<DirectoryItem*>* parent;
    int size;
};

#endif // DIRECTORYITEM_H
