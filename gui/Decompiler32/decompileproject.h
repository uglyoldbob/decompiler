#ifndef DECOMPILEPROJECT_H
#define DECOMPILEPROJECT_H

#include <QObject>
#include <QQmlListProperty>
#include <QUrl>
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
public:
    explicit DecompileProject(QObject *parent = nullptr);
    static void qml_register(); ///< This registers a singleton instance of the class that QML is responsible for creating and owning.

    QQmlListProperty<DecompiledObject> get_objects(); ///< Return a list of decompiled objects.
    int ObjectsCount() const; ///< Returns the number of decompiled objects.
    DecompiledObject *ObjectsIndex(int) const; ///< Returns the specified index of decompiled objects.
public slots:
    /*! Add the specified object to the project for decompilation
     *  @param filename The name of the file to use. Will be copied to the object folder.
     *  @param rpath The path that the output file belongs in. Empty string for root of object folder.
     *  @param rel_name The name to use when copying to the object folder. */
    void add_object(QString filename, QString rpath, QString rel_name);
    /*! Add the specified object to the project for decompilation. File is placed in the objects folder directly.
     *  @param filename The name of the file to use. Will be copied to the object folder. */
    void add_object(QUrl filename);
signals:
    void objects_changed(); ///< Triggered when the list of decompiled objects changes.
private:
    QString project_directory;  ///< The directory where all project files are stored.
    QVector <DecompiledObject*> objects; ///< All of the decompiled objects.

    static int s_ObjectsCount(QQmlListProperty<DecompiledObject>*); ///< Calls the member function ObjectsCount
    static DecompiledObject *s_ObjectsIndex(QQmlListProperty<DecompiledObject>*, int); ///< Calls the member function ObjectsIndex
};

#endif // DECOMPILEPROJECT_H
