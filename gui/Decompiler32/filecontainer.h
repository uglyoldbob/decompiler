#ifndef FILECONTAINER_H
#define FILECONTAINER_H

#include <QIODevice>
#include <QObject>

#include <memory>

/// A base class of any object that provides access to one or more files.
/** This class is useful to represent file containers such as zip, iso, bin/cue, etc.
 * Typically more than one file is made accessible. */
class FileContainer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ get_name NOTIFY name_changed);
    Q_PROPERTY(QStringList Files READ get_filenames NOTIFY filenames_changed) ///< A list of filenames that the container contains.
public:
    explicit FileContainer(QObject *parent = nullptr);
    static void qml_register(); ///< This registers a singleton instance of the class that QML is responsible for creating and owning.
    /*! \brief Examines an object using the specified device to try to determine what type of container it is.
     *  \param str The data stream for reading the contents of the object file.
     *  \return An instance of a FileContainer class if able to automatically determine the type
     */
    static std::shared_ptr<FileContainer> examine_object(std::shared_ptr<QIODevice> str);
    static void setup_examiners(void); ///< Setup all the examiners. Order is important.
    QStringList get_filenames(); ///< Return the list of contained filenames.
    /*! Returns the container name */
    QString get_name() { return name; }
signals:
    void filenames_changed(); ///< When the list of filenames changes.
    void name_changed(); ///< When the name of the container changes.

protected:
    QList <QString> filenames; ///< The list of contained filenames.
    QString name; ///< The name of the container
private:
    static std::vector<std::function<std::shared_ptr<FileContainer>(std::shared_ptr<QIODevice>)>> examiners; ///< The list of functions to examine a container.
};

#endif // FILECONTAINER_H
