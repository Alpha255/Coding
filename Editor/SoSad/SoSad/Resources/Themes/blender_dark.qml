import QtQuick 2.12
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.12
import QtQuick.Window 2.0

ApplicationWindow 
{
    id: window
    width: 1280
    height: 720
    visible: true
    title: "SoSad"
	minimumWidth: 640
	minimumHeight: 480
	color: "#232323"

	MenuBar 
	{
		id: menuBar
		height: 35
		x: 32
		spacing: -8

		Menu 
		{ 
			title: qsTr("File") 
			
			Action
			{
			    text: qsTr("&New")
			    shortcut: "Ctrl+O"
			}
		}
		Menu { title: qsTr("Settings") }
		Menu { title: qsTr("About") }

		delegate: MenuBarItem 
		{
			id: menuBarItem

			contentItem: Text 
			{
			    id: caption
	            text: menuBarItem.text
				font.family: "Segoe UI"
				font.pointSize: 9
				font.weight: Font.Normal
				color: "white"
			}

			background: Rectangle 
			{
				color: menuBarItem.highlighted ? "#5177b2" : "transparent"
				height: 25
				width: caption.width + 16
				radius: 3
				x: 4
				y: 5
			}
		}

		background: Rectangle 
		{
			color: "transparent"
		}
	}
}
