#ifndef DECOMPILEPROJECT_H
#define DECOMPILEPROJECT_H

#include <QFile>
#include <QObject>
#include <QQmlListProperty>
#include <QUrl>
#include <QVector>

#include "decompiledobject.h"
#include "directoryitem.h"

/// Represents a single decompile project.
/** All files are contained within a single directory.
 * The object folder contains all of the input files that were decompiled.
 */
class DecompileProject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<DecompiledObject> Objects READ get_objects NOTIFY objects_changed) ///< The list of decompile objects
    Q_PROPERTY(QList<QObject*> FileSysModel READ get_filesys NOTIFY filesys_changed) ///< The virtual filesystem for the project
public:
    explicit DecompileProject(QObject *parent = nullptr);
    static void qml_register(); ///< This registers a singleton instance of the class that QML is responsible for creating and owning.

    QQmlListProperty<DecompiledObject> get_objects(); ///< Return a list of decompiled objects.
    int ObjectsCount() const; ///< Returns the number of decompiled objects.
    DecompiledObject *ObjectsIndex(int) const; ///< Returns the specified index of decompiled objects.

    /*! Returns the virtual filesystem */
    QList<QObject *> get_filesys();
public slots:
    /*! Add the specified object to the project for decompilation
     *  @param filename The name of the file to use. Will be copied to the object folder.
     *  @param rpath The path that the output file belongs in. Empty string for root of object folder.
     *  @param rel_name The name to use when copying to the object folder. */
    void add_object(QString filename, QString rpath, QString rel_name);
    /*! Add the specified object to the project for decompilation. File is placed in the objects folder directly.
     *  @param filename The name of the file to use. Will be copied to the object folder. */
    void add_object(QUrl filename);

    /*! Add the specified container to the project for decompilation
     *  @param filename The name of the file to use. Will be copied to the container folder.
     *  @param rpath The path that the output file belongs in. Empty string for root of container folder.
     *  @param rel_name The name to use when copying to the container folder. */
    void add_container(QString filename, QString rpath, QString rel_name);
    /*! Add the specified object to the project for decompilation. File is placed in the containers folder directly.
     *  @param filename The name of the file to use. Will be copied to the container folder. */
    void add_container(QUrl filename);
signals:
    void objects_changed(); ///< Triggered when the list of decompiled objects changes.
    void filesys_changed(); ///< When the virtual filesystem changes.
private:
    QString project_directory;  ///< The directory where all project files are stored.
    QVector <DecompiledObject*> objects; ///< All of the decompiled objects.
    std::vector<std::shared_ptr<QIODevice>> devices; ///< A vector of io devices used in the project
    QList<DirectoryItem*> filesys; ///< The virtual filesystem for the project

    static int s_ObjectsCount(QQmlListProperty<DecompiledObject>*); ///< Calls the member function ObjectsCount
    static DecompiledObject *s_ObjectsIndex(QQmlListProperty<DecompiledObject>*, int); ///< Calls the member function ObjectsIndex
};

#endif // DECOMPILEPROJECT_H
