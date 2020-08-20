#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
	initGUI();
    ui.setupUi(this);
}

void MainWindow::initGUI()
{
	auto menu = menuBar()->addMenu(tr("&File"));
	auto toolbar = addToolBar(tr("File"));
	auto icon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
	auto act = new QAction(icon, tr("&New"), this);
	menu->addAction(act);
	toolbar->addAction(act);
}
