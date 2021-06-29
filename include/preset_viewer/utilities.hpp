#pragma once 

#include <Resources/preset.pb.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>

#include <variant>
#include <optional>
#include <tuple>
#include <string>

namespace preset_helpers{
// the public methods are simple wrappers of
// the private that are targeted to be used as a command line tool
class utilities{   
    public:
    enum class status{
        file_not_read,
        different,
        ok
    };
    static std::tuple<status,std::string> compare_files(const QString & fileName1,const QString & fileName2);
    static std::string compare_folders(const QString & dir_name1,const QString & dir_name2);

    static bool from_binFolder_to_textFolder(const QString & dirName);
    static bool from_bin_to_text( QFile &);
    enum class err{
        cant_open,
        wrong_format
    };
    private:
    static std::optional<proto::Preset> preset_from_text( QFile & file);
    static bool preset_to_specific_file(const proto::Preset & preset,const QString & path);
    static std::optional<proto::Preset> preset_from_binFile(QFile & file);
    static std::variant<proto::Preset,err> preset_from_binFile_w_diagnostics( QFile & file);
    static bool preset_to_binFile(const proto::Preset & pr,const QString & path);
    static std::optional<std::string> validate_and_name(const QFileInfo & file_info);
    static QByteArray preset_to_byteArray(const proto::Preset & preset);
    static std::optional<proto::Preset> byteArray_to_preset(const QByteArray & binary_arr);
};
}//namespace preset_helpers