#ifndef CONTAINERBINCUE_H
#define CONTAINERBINCUE_H

#include "filecontainer.h"

#include <memory>

class ContainerBinCue : public FileContainer
{
    Q_OBJECT
public:
    explicit ContainerBinCue(QObject *parent = nullptr);
    static std::shared_ptr<FileContainer> examine_object(std::shared_ptr<QIODevice> str); ///< Examine a given file (the cue file) and return an object if it is valid for this class.
signals:

};

#endif // CONTAINERBINCUE_H
