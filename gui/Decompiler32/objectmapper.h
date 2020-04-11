#ifndef OBJECTMAPPER_H
#define OBJECTMAPPER_H

#include <QObject>

#include <functional>
#include <memory>

/// Maps the contents of an object to memory.
/** This class is a helper that maps address space into the contents of the object file.
 */
class ObjectMapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString type READ get_type NOTIFY type_changed) ///< String describing the type of the object
public:
    explicit ObjectMapper(QObject *parent = nullptr);
    static void qml_register(); ///< This registers this class with QML
    /*! \brief Examines an object using the specified QDataStream to try to determine what type of object it is.
     *  \param str The data stream for reading the contents of the object file.
     *  \return An instance of an ObjectMapper class if able to automatically determine the type
     */
    static std::shared_ptr<ObjectMapper> examine_object(std::shared_ptr<QDataStream> str);
    static void setup_examiners(void); ///< Setup all the examiners
    /*! Return the type of the object. */
    QString get_type() { return type; }

    void set_type(QString t) { type = t; emit type_changed(); }

signals:
    void type_changed(); ///< The type of the object changed
protected:
    QString type; ///< A string representing the type of the object. This is simply for convenience to the user.
private:
    static std::vector<std::function<std::shared_ptr<ObjectMapper>(std::shared_ptr<QDataStream>)>> examiners; ///< The list of functions to examine an object.
};

#endif // OBJECTMAPPER_H
