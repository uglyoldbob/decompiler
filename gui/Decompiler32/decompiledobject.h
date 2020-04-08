#ifndef DECOMPILEDOBJECT_H
#define DECOMPILEDOBJECT_H

#include <QObject>

/// Represents a single decompiled object.
/** This is the base class for all types of decompile-able objects.
 * Could be an executable, a shared library, or something else. */
class DecompiledObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ get_name NOTIFY name_changed) ///< The name of the object.
public:
    /*! Create a decompiled object with the given file.
     * The file is the filename once it is copied to the project directory.
     * The filename is relative to the project directory.
     * @param file The filename to use for decompilation.
     * @param parent The qobject owner */
    explicit DecompiledObject(QString file, QObject *parent = nullptr);
    explicit DecompiledObject(QObject *parent = nullptr);   ///< Required for qmlRegisterType<DecompiledObject> to work

    /*! Return the name of the object. */
    QString get_name() { return name; }
signals:
    void name_changed();    ///< Triggered when the name changes.

protected:
    QString name; ///< The name of the generated object
    QString extension; ///< The extension of the generated object (if applicable)
};

#endif // DECOMPILEDOBJECT_H
