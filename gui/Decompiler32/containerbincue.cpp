#include "containerbincue.h"

ContainerBinCue::ContainerBinCue(QObject *parent) : FileContainer(parent)
{

}

std::shared_ptr<FileContainer> ContainerBinCue::examine_object(std::shared_ptr<QIODevice> str)
{
    str->seek(0);
    unsigned short signature;
    str->read((char*)&signature, 2);
    std::shared_ptr<FileContainer> ret;
    if (signature == 2)
    {
        ContainerBinCue *tmp = new ContainerBinCue();
        ret.reset(tmp);

        str->seek(0);
    }
    return ret;
}
