#ifndef MARIOEDITOR_H
#define MARIOEDITOR_H

#include <QtWidgets/QMainWindow>
#include "ui_marioeditor.h"

class MarioEditor : public QMainWindow
{
	Q_OBJECT

public:
	MarioEditor(QWidget *parent = 0);
	~MarioEditor();

private:
	Ui::MarioEditorClass ui;
};

#endif // MARIOEDITOR_H
