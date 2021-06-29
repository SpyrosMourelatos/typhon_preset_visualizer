#ifndef PRESET_VIEWER_H
#define PRESET_VIEWER_H

#include <QMainWindow>
#include <QStandardPaths>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class preset_viewer; }
QT_END_NAMESPACE

class preset_viewer : public QMainWindow
{
    Q_OBJECT

public:
    preset_viewer(QWidget *parent = nullptr);
    ~preset_viewer();

private slots:
    bool on_button0_clicked();
    bool on_button1_clicked();
    bool on_button2_clicked();
    bool on_button3_clicked();
    bool on_button4_clicked();

private:
    QString log{};
    const QString docs= QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    Ui::preset_viewer *ui;
};
#endif // PRESET_VIEWER_H
