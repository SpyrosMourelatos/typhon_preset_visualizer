#include <utilities.hpp>

#include <google/protobuf/text_format.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection.h>
#include <google/protobuf/util/message_differencer.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/util/field_comparator.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

#include <QDataStream>
#include <QDebug>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>

#include <optional>
#include <utility>
#include <string>
#include <sstream>
namespace preset_helpers{

QByteArray utilities::preset_to_byteArray(const proto::Preset & preset){
        QByteArray arr;
        arr.resize(preset.ByteSizeLong());
        preset.SerializeToArray(arr.data(),arr.size());
        return arr;
}
std::optional<proto::Preset> utilities::byteArray_to_preset(const QByteArray & binary_arr){
    proto::Preset preset;
    if(!preset.ParseFromArray(binary_arr.data(), binary_arr.size())) 
        return std::nullopt;
    else return std::optional<proto::Preset>{std::move(preset)};
}

std::optional<std::string> utilities::validate_and_name(const QFileInfo & file_info){
        auto file=new QFile(file_info.absoluteFilePath());
        auto pr=preset_from_binFile(*file);
        delete file;
        if(pr){
            auto str=file_info.fileName();
            bool is_it_num{false};
            unsigned num=(str.left(3)).toInt(&is_it_num,10);
            str.remove(0,4).chop(4);
            if(QString::fromStdString(pr->name())==str
                && is_it_num && num==pr->id())
                return pr->name();
        }
    return std::nullopt;    
}
std::optional<proto::Preset> utilities::preset_from_binFile(QFile & file){
    if(file.open(QIODevice::ReadOnly)){
        QByteArray binary_arr=file.readAll();
        std::optional<proto::Preset> preset;
        preset=byteArray_to_preset(binary_arr);
        file.close();    
        return preset;  
    }
    return std::nullopt;
}
std::variant<proto::Preset,utilities::err> utilities::preset_from_binFile_w_diagnostics(QFile & file){
    if(file.open(QIODevice::ReadOnly)){
        QByteArray binary_arr=file.readAll();
        std::optional<proto::Preset> preset;
        preset=byteArray_to_preset(binary_arr);
        file.close();   
        if(!preset)
            return err::wrong_format;
        else return  std::move(*preset);
    }
    return err::cant_open;
}
bool utilities::preset_to_binFile(const proto::Preset & pr,const QString & directory_name){
    auto padded_str=QString::number(pr.id());
    padded_str=padded_str.rightJustified(3,'0');
    QFile file( directory_name+padded_str+"."+QString::fromUtf8(pr.name().c_str()) +".typhonpr");
    QByteArray binary_arr;
    binary_arr=preset_to_byteArray(pr);
    if (file.open(QIODevice::WriteOnly)){
        file.write(binary_arr);
        file.close();
        return true;
    }
    else{
        return false;
    }

}
bool utilities::preset_to_specific_file(const proto::Preset & preset,const QString & path){
    QFile file(path);
    std::string str;
    google::protobuf::TextFormat::PrintToString(preset,&str);
    if (file.open(QIODevice::WriteOnly)){
        auto bytebuff=QString::fromUtf8(str.c_str());
        QTextStream stream(&file);
        stream<<bytebuff;
        file.close();
        return true;
    }
    else return false;
}
std::optional<proto::Preset> utilities::preset_from_text( QFile & file)
{
    if(file.open(QIODevice::ReadOnly)){
        QTextStream stream(&file);
        std::string str=stream.readAll().toStdString();
        proto::Preset preset;
        google::protobuf::TextFormat::ParseFromString(str,&preset);
        file.close();
        return preset;
    }
    else return std::nullopt;
}


///PUBLICS

bool utilities::from_bin_to_text( QFile& f){
    QString name=f.fileName();
    name.replace(name.size()-8,8,"txt");
    auto p=preset_from_binFile(f);
    qDebug()<<QString::fromStdString( p->name());
    if(p){
        qDebug()<<"Reached\n";
        return preset_to_specific_file(*p,name);
    }
    else return false;
}
bool utilities::from_binFolder_to_textFolder( const QString & dirFullName){
    QDir source(dirFullName);
    QString dir_name=source.dirName();
    source.cdUp();
    QString commonpath=source.absolutePath();
    QDirIterator it(commonpath+"/"+dir_name, QStringList() << "*.typhonpr", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()){
        auto tmp=it.next();
        QFileInfo finfo(tmp);
        QFile f(tmp);
        auto name=finfo.fileName();
        name.replace(name.size()-8,8,"txt");
        QDir().mkpath(finfo.absolutePath()+"Text");
        auto p=preset_from_binFile(f);
        if(p){
            preset_to_specific_file(*p,finfo.absolutePath()+"Text"+"/"+name);
        }
        else return false;
    }
    return true;
}
auto utilities::compare_files(const QString & fileName1,const QString & fileName2) -> std::tuple<status,std::string> {
    QFile f1(fileName1);
    QFile f2(fileName2);
    std::optional<proto::Preset> p1{};
    if(f1.fileName().endsWith("typhonpr"))
        p1=preset_from_binFile(f1);
    else p1=preset_from_text(f1); 
    std::optional<proto::Preset> p2{};
    if(f2.fileName().endsWith("typhonpr"))
        p2=preset_from_binFile(f2);
    else p2=preset_from_text(f2); 
    if(p1 && p2){
        using namespace google::protobuf;
        
        const Descriptor * desc1       = p1->GetDescriptor();
        const Descriptor * desc2       = p2->GetDescriptor();
        int fieldCount= desc1->field_count();
        std::string  report_string;
        auto * field_comp=new util::DefaultFieldComparator();
        field_comp->set_float_comparison(util::DefaultFieldComparator::FloatComparison::APPROXIMATE);
        field_comp->SetDefaultFractionAndMargin(0.008,0.008);
        util::MessageDifferencer diff;
        diff.set_field_comparator(field_comp);
        auto * stream = new io::StringOutputStream(&report_string);
        diff.ReportDifferencesToString(&report_string);
        std::vector<const FieldDescriptor*> field_vector1;
        std::vector<const FieldDescriptor*> field_vector2;
        for(int i=0;i<fieldCount;i++)
        {
            field_vector1.push_back(desc1->field(i));
            field_vector2.push_back(desc2->field(i));
        }
        diff.CompareWithFields(*p1,*p2,field_vector1,field_vector2);
        qDebug()<<QString::fromStdString(report_string);
        if(report_string=="")
            return  std::tuple<status,std::string>{status::ok,report_string};
        else
            return  std::tuple<status,std::string>{status::different,report_string};
    }
    else return std::tuple<status,std::string>{status::file_not_read,"At least one of the files couldn't open or didn't contain a preset"};
}
std::string utilities::compare_folders(const QString & dir_name1,const QString & dir_name2){
    QString ret;
    QDir dir1(dir_name1);
    QStringList list1(dir1.entryList(QStringList(),QDir::Files));
    QDir dir2(dir_name2);
    QStringList list2(dir2.entryList(QStringList(),QDir::Files));
    if(list1.size()!=list2.size())
    {
        return "Files didn't have same ammount of elements";
    }
    for(unsigned i=0;i<list1.size();i++){

        QString filename1(dir1.path()+list1[i]);
        QString filename2(dir2.path()+list2[i]);
        auto partial_ret=compare_files(filename1,filename2);
        if(std::get<0>(partial_ret)!=status::ok)
            ret.append(list1[i]).append(list2[i]).append("\n");

    }
    qDebug()<<ret;
    return ret.toStdString();
    
}

}//namespace

