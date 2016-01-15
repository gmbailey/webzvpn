import QtQuick 2.5
import Material 0.1

ApplicationWindow
{
    title: "webzvpn"
    id: mainWindow
    width: 400
    height: 600

    visible: true

    theme {
        primaryColor: Palette.colors["blue"]["500"]
        primaryDarkColor: Palette.colors["blue"]["700"]
        accentColor: Palette.colors["red"]["A200"]
        tabHighlightColor: "white"
    }

    signal connectPressed()
    signal serverSelected()

    onConnectPressed:{
        pageStack.pop(1)
    }

    onServerSelected:{
        pageStack.pop(1)
    }

//    Component.onCompleted:{
//        pageStack.push(Qt.createComponent("ConnectPage.qml"))
//        pageStack.push(Qt.createComponent("ConnectPage.qml"))
//    }

    initialPage: TabbedPage {
        Tab {
            title: "Status"
            sourceComponent: tabDelegate

        }

        Tab {
            title: "Settings"
        }
    }

    Component {
        id: tabDelegate
        Flickable {
            id: flickable
            anchors {
                left: sidebar.right
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }
            clip: true
            contentHeight: Math.max(mainWindow.implicitHeight + 40, height)
            Loader {
                id: example
                anchors.fill: parent
                asynchronous: true
                visible: status == Loader.Ready
                source: {
                    return Qt.resolvedUrl("ConnectPage.qml")
                }
            }

            ProgressCircle {
                anchors.centerIn: parent
                visible: example.status == Loader.Loading
            }
        }

    }

}
