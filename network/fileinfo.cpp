#include "fileinfo.h"
#include <QStringList>

namespace CloudDisk{
FileInfo::FileInfo(const QString& name,
                   const QString& parentDir,
                   bool isFolder,
                   size_t size)
    : _name(name),
      _path(parentDir),
      _size(size),
      _type(UNKNOWN)
{
    if(isFolder){
        _type = FOLDER;
    }
    else{
        getTypeFromFileName(name);
    }
}

void FileInfo::getTypeFromFileName(const QString& fileName)
{
    if(_type == 0){

    }
    const QString suffix = fileName.split(".").back();

    if(suffix.isEmpty()){
        _type = UNKNOWN;
    }
    else if(suffix == "txt"){
        _type = TXT;
    }
    else if(suffix == "png"){
        _type = PNG;
    }
    else if(suffix == "jpg"){
        _type = JPG;
    }
    else if(suffix == "mp4"){
        _type = MP4;
    }
    else if(suffix == "mkv"){
        _type = MKV;
    }
    else if(suffix == "zip"){
        _type = ZIP;
    }
    else{
        _type = UNKNOWN;
    }
}


}
