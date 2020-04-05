#ifndef DECOMPILEPROJECT_H
#define DECOMPILEPROJECT_H

#include <QObject>
#include <QQmlListProperty>
#include <QVector>

class DecompileProject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<QString> Things READ get_things NOTIFY things_changed)
    Q_PROPERTY(QStringList bob READ get_l NOTIFY l_changed)
public:
    explicit DecompileProject(QObject *parent = nullptr);
    static void qml_register();

    int ThingsCount() const;
    QString *ThingsIndex(int) const;
    QQmlListProperty<QString> get_things();

    QStringList get_l();

signals:
    void things_changed();
    void l_changed();
private:
    QVector <QString *> things;
    QList <QString> bob;
    static int s_ThingsCount(QQmlListProperty<QString>* list);
    static QString *s_ThingsIndex(QQmlListProperty<QString>* list, int i);
};

#endif // DECOMPILEPROJECT_H
