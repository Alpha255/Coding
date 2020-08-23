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

	ToolButton
	{
		id: main_ToolButton
		icon.source: "/Resources/Icons/blender.png"
		icon.width: 20
		icon.height: 20
		icon.color: "white"

		background: Rectangle
		{
			MouseArea 
			{
				hoverEnabled: true
			}

			color: main_ToolButton.hovered ? "#5177b2" : "transparent"
			width: 25
			height: 25
			radius: 3
			x: 5
			y: 5
		}
	}

	MenuBar 
	{
		id: menuBar
		height: 35
		leftPadding: main_ToolButton.width + 5

		Menu 
		{ 
			title: qsTr("File") 
			
			Action 
			{
				text: qsTr("&New")
			}
			Action 
			{
				text: qsTr("&Open")
			}
			Action 
			{
				text: qsTr("Open &Rencent")
			}
		}
		Menu { title: qsTr("Settings") }
		Menu { title: qsTr("About") }

		delegate: MenuBarItem 
		{
			id: menuBarItem

			contentItem: Text 
			{
	            text: menuBarItem.text
				font.family: "Segoe UI"
				font.pointSize: 9
				font.weight: Font.Normal
				color: "white"
				horizontalAlignment: Text.AlignLeft
				verticalAlignment: Text.AlignVCenter
				elide: Text.ElideRight
			}

			background: Rectangle 
			{
				color: menuBarItem.highlighted ? "#5177b2" : "transparent"
				height: 25
				width: menuBarItem.text.contentWidth / 2
				radius: 3
				y: 5
			}
		}

		background: Rectangle 
		{
			color: "transparent"
		}
	}
}
