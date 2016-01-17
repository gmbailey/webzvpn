import QtQuick 2.5
import Material 0.2

ApplicationWindow{

    title: "webzvpn"
    id: mainWindow
    minimumHeight: 620
    minimumWidth: 400
    maximumHeight: minimumHeight
    maximumWidth: minimumWidth

    visible: true

    theme {
        primaryColor: Palette.colors["blue"]["500"]
        primaryDarkColor: Palette.colors["blue"]["700"]
        accentColor: Palette.colors["red"]["A200"]
        tabHighlightColor: "white"
    }

    ListModel { id: comboModel }


    property string pageResp
    property string xmlResp
    property var servers: []

    Connections {
        target: FileHandler
    }

    Connections {
        target: ServerHandler
    }

    function setupServComboBox() {
        servers = ServerHandler.servers
        for (var k = 0; k < servers.length; k++){
            comboModel.append({"name": servers[k].name})
        }
        if (servers.length > 0)
            return true
        else
            return false
    }

    function loadServerXml(){
        xmlResp = FileHandler.read("servers.xml")
        ServerHandler.setupServerList(xmlResp)
        return setupServComboBox()
    }

    function getServers(url) {
        var doc = new XMLHttpRequest();
        doc.open("GET", url);
        doc.onreadystatechange = function() {
            if (doc.readyState === XMLHttpRequest.DONE) {
                if (doc.responseText.length > 0){
                    if (doc.responseText.match("!!!:::(.*)==!!!").length > 1){
                        pageResp = doc.responseText.match("!!!:::(.*)==!!!")[1]
                        pageResp = Qt.atob(pageResp)
                    }
                }
                else {
                    return false
                }
            }
        }
        doc.setRequestHeader("Content-Encoding", "UTF-8");
        doc.send();

        ServerHandler.setupServerList(xmlResp)
        return setupServComboBox()
    }

    signal connectPressed()
    signal serverSelected()

    onConnectPressed:{
        pageStack.pop(1)
    }

    onServerSelected:{
        pageStack.pop(1)
    }

    initialPage: TabbedPage {
        title: "webzvpn"
        Tab {
            title: "Status"
            source:Qt.resolvedUrl("ConnectPage.qml")

        }

        Tab {
            title: "Settings"
            source: Qt.resolvedUrl("SettingsPage.qml")
        }
    }
/*
    Component {
        id: tabDelegate
        Flickable {
            id: flickable
            anchors {
                left: parent.left
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

    Component {
        id: tabDelegate2
        Flickable {
            id: flickable

            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }
            clip: true
            contentHeight: Math.max(mainWindow.implicitHeight + 40, height)
            Loader {
                id: example2
                anchors.fill: parent
                asynchronous: true
                visible: status == Loader.Ready
                source: {
                    return Qt.resolvedUrl("SettingsPage.qml")

                }
            }

            ProgressCircle {
                anchors.centerIn: parent
                visible: example2.status == Loader.Loading
            }
        }
    }

    Component.onCompleted: {

    }
*/
}
