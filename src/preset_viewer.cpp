#include "preset_viewer.h"
#include "./ui_preset_viewer.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <utilities.hpp>

preset_viewer::preset_viewer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::preset_viewer)
{
    ui->setupUi(this);
    ui->button3->setVisible(false);
    ui->button4->setVisible(false);
}

bool preset_viewer::on_button0_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                docs,
                                                tr("Binaries(*.typhonpr)"));
    auto file=new QFile(fileName);
    if(preset_helpers::utilities::from_bin_to_text(*file)) //cmd command
    {
        ui->info->setText("Succeeded file written at:\n"+fileName.replace(fileName.size()-8,8,"txt"));
    }
    else ui->info->setText("Failed :(");
    
    return true;
}
bool preset_viewer::on_button1_clicked()
{
    qDebug()<<"It clicked";
    QString dir_name = QFileDialog::getExistingDirectory(this, "Select a preset folder ",
                                            docs,
                                            QFileDialog::ShowDirsOnly);
    if(preset_helpers::utilities::from_binFolder_to_textFolder(dir_name)){
        ui->info->setText("Succeeded ,files places at subfolders with suffix Text");
    }
    else ui->info->setText("Failed :(");
    return true;
}
bool preset_viewer::on_button2_clicked()
{
    QString fileName1 = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                docs
                                                );
    QString fileName2 = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                docs
                                               );
    auto res=preset_helpers::utilities::compare_files(fileName1,fileName2);
    std::string info=std::get<1>(res);
    log=QString::fromStdString(info);
    ui->info->setText(QString::fromStdString(info));
    ui->button4->setVisible(true);
    return true;
}
bool preset_viewer::on_button3_clicked()
{
    QString dir_name1 = QFileDialog::getExistingDirectory(this, "Select a preset folder ",
                                            docs,
                                            QFileDialog::ShowDirsOnly);
    QString dir_name2 = QFileDialog::getExistingDirectory(this, "Select a preset folder ",
                                            docs,
                                            QFileDialog::ShowDirsOnly);
    preset_helpers::utilities::compare_folders(dir_name1,dir_name2);
    return true;
}
preset_viewer::~preset_viewer()
{
    delete ui;
}


bool preset_viewer::on_button4_clicked()
{
    QFile f(docs+"/log.txt");
    if(f.open(QIODevice::WriteOnly)){
        f.write(log.toUtf8());
    }
    ui->button4->setVisible(false);
    return true;
}
