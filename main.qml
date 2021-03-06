import QtQuick 2.5
import Material 0.2
import Material.Extras 0.1

ApplicationWindow{

    title: "Webz VPN"
    id: mainWindow
    minimumHeight: Units.dp(630)
    minimumWidth: Units.dp(400)
    maximumHeight: minimumHeight
    maximumWidth: minimumWidth

    //C++ Binding
    signal servButtonPressed()

    visible: true

    theme {
        primaryColor: Palette.colors["blue"]["500"]
        primaryDarkColor: Palette.colors["blue"]["700"]
        accentColor: Palette.colors["lightBlue"]["500"]

        tabHighlightColor: "white"
    }

    ListModel { id: comboModel }

    property string pageResp
    property string xmlResp
    property var servers: []

    property var sectionComponents: [ "ConnectPage", "SettingsPage", "LogPage" ]
    property var sectionTitles: [ "Status", "Settings", "Log" ]

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
            ServerHandler.changeState(0)
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
                        FileHandler.save(pageResp, "servers.xml")
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
        id: tabbed
        title: "Webz VPN"
        /*Tab {
            title: "Status"
            source:Qt.resolvedUrl("qrc:/ConnectPage.qml")
        }

        Tab {
            title: "Settings"
            source: Qt.resolvedUrl("SettingsPage.qml")
        }
        Tab {
            title: "Log"
            source:  Qt.resolvedUrl("LogPage.qml")
        }*/

        Repeater {
            model: sectionComponents

            delegate: Tab {
                title: sectionTitles[index]
                asynchronous: true
                sourceComponent: Component {// Qt.resolvedUrl("%.qml").arg(modelData)
                    Loader {
                        source: Qt.resolvedUrl("%.qml").arg(modelData)
                        asynchronous: true
                        //visible: status == Loader.Ready
                    }
                }
            }
        }

    }
}
