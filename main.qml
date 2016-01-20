import QtQuick 2.5
import Material 0.2

import Material.Extras 0.1
import QtQuick.Controls 1.4 as QTControls

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

        if (servers.length > 0){
            console.log("returning true")
            return true
        }
        else{
            console.log("returning false")
            return false
        }
    }

    function loadServerXml(){
        xmlResp = FileHandler.read("servers.xml")
        ServerHandler.setupServerList(xmlResp)
        return setupServComboBox()
    }

    function getServers(url) {
        var doc = new XMLHttpRequest();

        doc.onreadystatechange = function() {
            if (doc.readyState === XMLHttpRequest.DONE) {
                if (doc.responseText.length > 0){
                    if (doc.responseText.match("!!!:::(.*)=!!!").length > 1){
                        console.log("length > 1")
                        pageResp = doc.responseText.match("!!!:::(.*)=!!!")[1]
                        pageResp = Qt.atob(pageResp)
                        ServerHandler.setupServerList(pageResp)
                        setupServComboBox()
                        if (servers.length === 0)
                            loadServerXml()
                    }
                }
                else {
                    return false
                }
            }
        }
//        doc.setRequestHeader("Content-Encoding", "UTF-8");
        doc.open("GET", url);
        doc.send();

    }

    signal connectPressed()
    signal serverSelected()

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
        Tab {
            title: "Log"
            source:  Qt.resolvedUrl("LogPage.qml")
        }
    }


}
