#ifndef DECOMPILEPROJECT_H
#define DECOMPILEPROJECT_H

#include <QObject>
#include <QQmlListProperty>
#include <QVector>

#include "decompiledobject.h"

/// Represents a single decompile project.
/** All files are contained within a single directory.
 * The object folder contains all of the input files that were decompiled.
 */
class DecompileProject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<DecompiledObject> Objects READ get_objects NOTIFY objects_changed) ///< The list of decompile objects
    Q_PROPERTY(QQmlListProperty<QString> Things READ get_things NOTIFY things_changed) ///< broken string list property
    Q_PROPERTY(QStringList bob READ get_l NOTIFY l_changed) ///< working string list property
public:
    explicit DecompileProject(QObject *parent = nullptr);
    static void qml_register(); ///< This registers a singleton instance of the class that QML is responsible for creating and owning.

    int ThingsCount() const;
    QString *ThingsIndex(int) const;
    QQmlListProperty<QString> get_things();

    QStringList get_l();

    QQmlListProperty<DecompiledObject> get_objects(); ///< Return a list of decompiled objects.
    int ObjectsCount() const; ///< Returns the number of decompiled objects.
    DecompiledObject *ObjectsIndex(int) const; ///< Returns the specified index of decompiled objects.
public slots:
    /*! Add the specified object to the project for decompilation
     *  @param filename The name of the file to use. Will be copied to the object folder.
     *  @param rel_name The name to use when copying to the object folder. Do not prepend with anything. "folder_name/blab" is acceptable. */
    void add_object(QString filename, QString rel_name);
    /*! Add the specified object to the project for decompilation. File is placed in the objects folder directly.
     *  @param filename The name of the file to use. Will be copied to the object folder. */
    void add_object(QString filename);
signals:
    void objects_changed(); ///< Triggered when the list of decompiled objects changes.
    void things_changed();
    void l_changed();
private:
    QString project_directory;  ///< The directory where all project files are stored.
    QList <DecompiledObject*> objects; ///< All of the decompiled objects.
    QVector <QString *> things;
    QList <QString> bob;
    static int s_ThingsCount(QQmlListProperty<QString>* list);
    static QString *s_ThingsIndex(QQmlListProperty<QString>* list, int i);

    static int s_ObjectsCount(QQmlListProperty<DecompiledObject>*); ///< Calls the member function ObjectsCount
    static DecompiledObject *s_ObjectsIndex(QQmlListProperty<DecompiledObject>*, int); ///< Calls the member function ObjectsIndex
};

#endif // DECOMPILEPROJECT_H
