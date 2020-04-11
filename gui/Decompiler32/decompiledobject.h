#ifndef DECOMPILEDOBJECT_H
#define DECOMPILEDOBJECT_H

#include <QIODevice>
#include <QObject>

#include <memory>

#include "objectmapper.h"

/// Represents a single decompiled object.
/** This is the base class for all types of decompile-able objects.
 * Could be an executable, a shared library, or something else. */
class DecompiledObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ get_name NOTIFY name_changed) ///< The name of the object.
    Q_PROPERTY(QString type READ get_type NOTIFY type_changed) ///< String describing the type of the object
public:
    /*! Create a decompiled object with the given file.
     * The file is the filename once it is copied to the project directory.
     * The filename is relative to the project directory.
     * @param file The filename to use for decompilation.
     * @param parent The qobject owner */
    explicit DecompiledObject(QString file, QObject *parent = nullptr);
    explicit DecompiledObject(QObject *parent = nullptr);   ///< Required for qmlRegisterType<DecompiledObject> to work

    static void qml_register(); ///< This registers this class with QML

    /*! Return the name of the object. */
    QString get_name() { return name; }
    /*! Return the type of the object. */
    QString get_type();

    /*! Give a device to the object that corresponds to a stream for the decompiled object.
     * @param ds The device to use for reading the bytes of the object to decompile. */
    void give_device(std::shared_ptr<QIODevice> ds);
signals:
    void name_changed();    ///< Triggered when the name changes.
    void type_changed();    ///< The type of the object changed

protected:
    QString name; ///< The name of the generated object
    QString extension; ///< The extension of the generated object (if applicable)
    QString type; ///< A string representing the type of the object. This is simply for convenience to the user.

    //QBuffer, QByteArray,
    std::shared_ptr<QIODevice> device; ///< The device used to read data from the object.
    std::shared_ptr<ObjectMapper> mapper; ///< The mapper that allows us to get data from the object file
};

#endif // DECOMPILEDOBJECT_H
