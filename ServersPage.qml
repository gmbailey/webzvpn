import QtQuick 2.5
import QtQuick.Controls 1.3 as QuickControls
import Material 0.1
import Material.Extras 0.1

Page {
    id: serversPage

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
    }

    function loadServerXml(){
        xmlResp = FileHandler.read("servers.xml")
        ServerHandler.setupServerList(xmlResp)
        setupServComboBox()
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
            }
        }
        doc.setRequestHeader("Content-Encoding", "UTF-8");
        doc.send();

        ServerHandler.setupServerList(xmlResp)
        setupServComboBox()
    }

    Rectangle {
        width: 360
        height: 360

        ListModel { id: comboModel }

        Dialog {
            id: serverDialog
            width: Units.dp(300)
            text: "Servers"
            hasActions: true
            positiveButtonText: "Select Server"
            negativeButtonText: "Cancel"

            MenuField {
                id: comboServers
                model: comboModel
            }

            onAccepted: {
                console.log(servers[comboServers.selectedIndex].address)
                OvpnController.setServer(servers[comboServers.selectedIndex].address)
                mainWindow.serverSelected()
            }
        }
    }

    Component.onCompleted: {
        serversPage.loadServerXml()
        if (xmlResp.length == 0){
            serversPage.getServers("http://webzvpn.ru/servers.php")
            FileHandler.save(pageResp, "servers.xml")
        }
        serverDialog.show()
    }
}
