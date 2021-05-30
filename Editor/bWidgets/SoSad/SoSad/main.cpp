#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	QFile file(":/Styles/Flat");
	file.open(QFile::ReadOnly);
	QString style = QString::fromLatin1(file.readAll());
	a.setStyleSheet(style);
    MainWindow w;
	w.setWindowOpacity(0.7);
    w.show();
    return a.exec();
}
