#include <QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QSettings>
#include <QIcon>
#include <QFont>
///#include <QtQuickControls2/QQuickStyle>

int main(int argc, char* argv[])
{	
	QGuiApplication app(argc, argv);
	QGuiApplication::setWindowIcon(QIcon(":Images/mainIcon"));
	//QIcon::setThemeName("blender_dark");
	///QQuickStyle::setStyle("Material");

	QQmlApplicationEngine engine;
	engine.load(QUrl("qrc:/Themes/blender_dark"));
	Q_ASSERT(!engine.rootObjects().isEmpty());

	return app.exec();
}