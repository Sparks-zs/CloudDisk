#ifndef FILEINFO_H
#define FILEINFO_H

#include <QString>

namespace CloudDisk{

class FileInfo
{
public:
    enum FileType{
        UNKNOWN = 0,
        FOLDER,
        TXT,
        PNG,
        JPG,
        MP4,
        MKV,
        ZIP
    };

    FileInfo() {}

    FileInfo(const QString& name,
             const QString& path,
             bool isFolder,
             size_t size);

    void set_name(const QString& name) { _name = name; }
    const QString& name() const { return _name; }

    void set_path(const QString& parentDir) { _path = parentDir; }
    const QString& path() const { return _path; }

    int type() const { return _type; }

    void set_size(size_t size) { if(size >= 0 ) _size = size; }
    size_t size() const { return _size; }

private:
    void getTypeFromFileName(const QString& fileName);

    QString _name;
    QString _path;
    size_t _size;
    int _type;
};

}

#endif // FILEINFO_H
