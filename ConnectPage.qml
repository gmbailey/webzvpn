import QtQuick 2.4
import Material 0.2

import QtQuick.Layouts 1.1
import Material.ListItems 0.1 as ListItem
import QtQuick.Controls 1.3 as QuickControls
import Material.Extras 0.1

Page {
    id: connectPage

    property bool isLoading: false
    property bool serverLoading: false
    property bool displayStatus: true
    property int loginState: -1             //0 login invalid, 1 login valid: credentials found
    property int serverIndex
    property int controlStatus

    function findIndex(text){
        for(var i = 0; i < servers.length; i++){
            if (servers[i].name === text){
                serverIndex = i
                return
            }
        }
    }

    function checkLogin() {
        console.log("in checkLogin")
        if (OvpnController.userName.length === 0 || OvpnController.userPass.length === 0){
            loginDialog.show()
        }
        else {
            loginState = 1
        }
    }

    function autoConnect() {
        console.log("in autoConnect")
        if (OvpnController.autoConnect){
            checkLogin()
            if (loginState == 1)
                OvpnController.startConn()
        }
    }

    function updateStatus(ovState) {
        switch (ovState) {
        case 0:
            statusText.text = "Disconnected"
            isLoading = false
            connect.enabled = true
            connectText.text = "Connect"
            Theme.primaryColor = Palette.colors["blue"]["500"]
            Theme.primaryDarkColor = Palette.colors["blue"]["700"]
            break
        case 1:
            statusText.text = "Starting"
            isLoading = true

            connectText.text = "Cancel"
            break
        case 2:
            statusText.text = "Authorizing"
            isLoading = true

            connectText.text = "Cancel"
            break
        case 3:
            statusText.text = "Auth Failed"
            isLoading = false
            connectText.text = "Connect"
            break
        case 4:
            statusText.text = "Connecting"
            isLoading = true

            connectText.text = "Cancel"
            break
        case 5:
            statusText.text = "Wait"
            isLoading = true
            break
        case 6:
            statusText.text = "Reconnecting"
            isLoading = true
            connectconnectText.text = "Cancel"
            break
        case 7:
            statusText.text = "Getting Config"
            break;
        case 8:
            statusText.text = "Assigning IP"
            break;
        case 9:
            statusText.text = "TCP Connect"
            break;
        case 10:
            statusText.text = "Connected"
            isLoading = false
            connect.enabled = true
            connectText.text = "Disconnect"
            Theme.primaryColor = Palette.colors["lightGreen"]["500"]
            Theme.primaryDarkColor = Palette.colors["lightGreen"]["700"]
            break
        case 11:
            statusText.text = "Disconnecting"
            isLoading = true
            connect.enabled = false
            connectText.text = "Connect"
            Theme.primaryColor = Palette.colors["red"]["500"]
            Theme.primaryDarkColor = Palette.colors["red"]["700"]
            break
        case 12:
            isLoading = true
            connect.enabled = false
            break
        case 13:
            loginDialog.show()
            controlStatus = 1
            break
        case 14:
            controlStatus = 2
            if (mainWindow.servers.length > 0)
                serverDialog.show()
            else{
                serverLoading = true
                serverDialog.show()
                mainWindow.servButtonPressed()
            }
            break
        }

    }

    function serversRetrieval(curState) {
        switch (curState) {
        case 0: //DONE
            serverLoading = false
            console.log("done")
            serverDialog.show()
            break
        case 1: //RETRIEVING
            console.log("loading")
            break
        case 2: //PROCESSED
            mainWindow.setupServComboBox()
            break
        }
    }

    function versionRetrieval(curState) {
        switch (curState) {
        case 0: //LATEST
            isLoading = false
            console.log("latest version")
            break
        case 1: //CHECKING
            isLoading = true
            console.log("checking version")
            break
        case 2: //OUTDATED
            console.log("out of date")
            isLoading = false
            updateDialog.show()
            break
        }
    }

    Connections {
        target:OvpnController
        onStateChanged: {
            console.log("status: " + ovState)
            updateStatus(ovState)
        }
    }

    Connections {
        target: ServerHandler
        onStateChanged: {
            console.log("state: " + curState)
            serversRetrieval(curState)
        }
    }

    Connections {
        target:OvpnController
        onVerStateChanged: {
            console.log("verState: " + curState)
            versionRetrieval(curState)
        }
    }

    ColumnLayout{
        id: column
        anchors.fill: parent
        spacing: Units.dp(1)
        anchors.margins: Units.dp(5)
//        spacing: 1

        Row {
            id: buttonRow
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: Units.dp(4)

            Button {
                id: connect
                width: Units.dp(200)
                height: Units.dp(50)
                backgroundColor: Theme.accentColor
                elevation: 1
                //text: "Connect"
                enabled: true
                onClicked: {
                    if (statusText.text == "Disconnected"){
                        OvpnController.initConnection()
                        //Check for selected server
                        /*
                        if (OvpnController.serverName === ""){
                            if (mainWindow.servers.length > 0)
                                serverDialog.show()
                            else
                                mainWindow.servButtonPressed()
                        }

                        //    serverDialog.show()

                        //Check Login First
                        checkLogin()
                        console.log("in connect: login = " + loginState)

                        if (loginState == 1)
                            OvpnController.startConn()*/
                    }
                    else {
                        OvpnController.stopConn()
                    }
                }
                Label {
                    id: serverNameLbl
                    font.family: "Roboto"
                    font.pixelSize: Units.dp(12)
                    color: "white"
                    anchors.centerIn: parent
                    anchors.verticalCenterOffset: Units.dp(10)
                    text: OvpnController.serverName
                }
                Label {
                    id: connectText
                    font.family: "Roboto"
                    font.weight: Font.DemiBold
                    font.pixelSize: Units.dp(18)
                    text: "Connect"
                    anchors.verticalCenterOffset: Units.dp(-4)
                    anchors.centerIn: parent
                    color: "white"
                }
            }

            Button {
                id : serverSelection
                width: Units.dp(50)
                height: Units.dp(50)
                backgroundColor: Theme.accentColor
                elevation: 1
                enabled: true

                Icon {
                    anchors.centerIn: parent
                    name: "awesome/map_marker"
                    size: Units.dp(24)
                    color: Theme.dark.iconColor
                }

                onClicked: {
                    if (mainWindow.servers.length > 0)
                        serverDialog.show()
                    else{
                        serverLoading = true
                //        serverDialog.show()
                        mainWindow.servButtonPressed()
                    }
                }
            }
        }

        Row {
            spacing: Units.dp(8)
            anchors.horizontalCenter: parent.horizontalCenter
            ProgressCircle {
                id: busy
                anchors.verticalCenter: parent.verticalCenter
                visible: connectPage.isLoading
            }
        }

        ListItem.Divider{
            Layout.fillWidth: parent
        }

        //Status
        ListItem.Standard {
        //    showDivider: true

            content:
                Label {
                id: statusText
                text: "Disconnected"
                anchors.centerIn: parent
                font.family: "Roboto"
                font.weight: Font.DemiBold
                font.pixelSize: Units.dp(20)
            }
        }

        ListItem.Divider{
            Layout.fillWidth: parent
        }

        //Server Load Display
        ListItem.Standard {
            id: serverLoad
            interactive: false
     //       showDivider: true
            Icon {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: Units.dp(20)
                name: "action/info"
                size: Units.dp(24)
            }
            content: Column{
                anchors.centerIn: parent
                Label {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "Server Load"
                    font.family: "Roboto"
                    font.weight: Font.Bold
                    font.pixelSize: Units.dp(10)
                }
                Label {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: OvpnController.serverLoad
                    style: "title"
                }
            }
        }

        ListItem.Divider{
            Layout.fillWidth: parent
        }

        //Server IP Display
        ListItem.Standard {
            id: serverIp
            interactive: false
   //         showDivider: true
            Icon {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: Units.dp(20)
                name: "awesome/lock"
                size: Units.dp(24)
            }
            content: Column{
                anchors.centerIn: parent
                Label {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "IP Address"
                    font.family: "Roboto"
                    font.weight: Font.Bold
                    font.pixelSize: Units.dp(10)
                }
                Label {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: OvpnController.newIp
                    style: "title"
                }
            }
        }

        ListItem.Divider{
            Layout.fillWidth: parent
        }

        //Profile display
        ListItem.Standard {
            id: profile
            interactive: false
 //           showDivider: true
            Icon {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: Units.dp(20)
                name: "awesome/globe"
                size: Units.dp(24)
            }
            content: Column{
                anchors.centerIn: parent
                Label {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "Profile"
                    font.family: "Roboto"
                    font.weight: Font.Bold
                    font.pixelSize: Units.dp(10)
                }
                Label {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "UDP " + OvpnController.port
                    style: "title"
                }
            }
        }
        ListItem.Divider{
            Layout.fillWidth: parent
        }
    }

    Dialog {
        id: serverDialog
        width: Units.dp(300)
        title: "Servers"
        hasActions: true
        positiveButtonText: "Select Server"
        negativeButtonText: "Cancel"

        ProgressCircle {
            id: prog
            anchors.horizontalCenter: parent.horizontalCenter
            visible: connectPage.serverLoading
        }

        QuickControls.ExclusiveGroup {
            id: optionGroup
        }

        Repeater {
            id: myList

            RowLayout {
                CircleImage {
                    source: Qt.resolvedUrl("qrc:/flags/flags/" + servers[index].flag + ".png")
                }

                RadioButton {
                    text: modelData
                    exclusiveGroup: optionGroup
                    onClicked: {
                        findIndex(text)
                    }
                }

            }
            model: comboModel
        }

        onAccepted: {
            OvpnController.setServer(servers[serverIndex].address)
            OvpnController.setServerName(servers[serverIndex].name)
            OvpnController.setServerLoad(servers[serverIndex].load)
            if(controlStatus === 2){
                OvpnController.initConnection()
            }
        }

        onRejected: {
            if(controlStatus === 2){
                OvpnController.cancelInitConn()
            }
        }

    }

    Dialog {
        id: loginDialog
        width: Units.dp(300)
        title: "Login"
        hasActions: true
        positiveButtonText: "Login"
        negativeButtonText: "Cancel"

        ListItem.Standard{
            height: Units.dp(10)
        }

        TextField {
            id: usernameField
            placeholderText: "Username"
            floatingLabel: true
            //anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
        }

        ListItem.Standard{
            height: Units.dp(5)
        }

        TextField {
            id: passwordField
            placeholderText: "Password"
            floatingLabel: true
            echoMode: TextInput.Password
            helperText: "Enter the password."
            //anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
        }

        CheckBox{
            text: "Remember Me"
            checked: OvpnController.rememberLogin

            onClicked:{
                console.log("remember me clicked")
                OvpnController.rememberLogin = checked
            }
        }

        onAccepted: {
            if(controlStatus === 1){
                if (usernameField.length > 0 && passwordField.length > 0){
                    OvpnController.setUserName(usernameField.getText(0, usernameField.length))
                    OvpnController.setUserPass(passwordField.getText(0, passwordField.length))
                }

                OvpnController.initConnection()
            }
            else {
                console.log("in onAccepted: " + usernameField.length + " " + passwordField.length)
                if (usernameField.length > 0 && passwordField.length > 0){
                    OvpnController.setUserName(usernameField.getText(0, usernameField.length))
                    OvpnController.setUserPass(passwordField.getText(0, passwordField.length))
                    loginState = 1
                }
                else
                    loginState = 0
            }
        }

        onRejected: {
            if(controlStatus === 1){
                OvpnController.cancelInitConn()
            }
            else {
                loginState = 0
            }
        }
    }

    Dialog {
        id: updateDialog
        width: Units.dp(300)
        title: "Update"
        hasActions: true
        positiveButtonText: "Update"
        negativeButtonText: "Cancel"

        text: "Update Available"

        onAccepted: {
            OvpnController.launchUpdater()
        }

    }

    Component.onCompleted: {
        /*
          Server Retrieval will be done upon the click of the servers button. AutoConnect temporarily disabled
        */
  /*      if (!mainWindow.getServers("http://webzvpn.ru/servers.php")){
            debugLbl.text = "in this"
            if (!mainWindow.loadServerXml()){
                debugLbl.text = "tried to get xml"
                console.log("Failed to get servers! TODO: Retry server retrieval")
            }
        }*/

   //     mainWindow.getServers("http://webzvpn.ru/servers.php")

        //Attempt Auto-Connect
   //     autoConnect()
        OvpnController.checkVersion()
    }
}
