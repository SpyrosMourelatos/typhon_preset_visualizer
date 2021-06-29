#include "preset_viewer.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    preset_viewer w;
    w.show();
    return a.exec();
}
