import QtQuick 2.4
import Material 0.2

import QtQuick.Layouts 1.1
import Material.ListItems 0.1 as ListItem

Page {
    id: connectPage

    property bool isLoading: false
    property bool displayStatus: true
    property int loginState: -1             //0 login invalid, 1 login valid: credentials found

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
            connect.text = "Connect"
            mainWindow.theme.primaryColor = Palette.colors["blue"]["500"]
            break
        case 1:
            statusText.text = "Starting"
            isLoading = true
            connect.enabled = true
            connect.text = "Cancel"
            break
        case 2:
            statusText.text = "Authorizing"
            isLoading = true
            connect.enabled = true
            connect.text = "Cancel"
            break
        case 3:
            statusText.text = "Auth Failed"
            isLoading = false
            connect.text = "Connect"
            break

        case 4:
            statusText.text = "Connecting"
            isLoading = true
            connect.enabled = true
            connect.text = "Cancel"
            break
        case 5:
            statusText.text = "Connected"
            isLoading = false
            connect.enabled = true
            connect.text = "Disconnect"
            mainWindow.theme.primaryColor = Palette.colors["green"]["500"]
            break
        case 6:
            statusText.text = "Disconnecting"
            isLoading = true
            connect.enabled = false
            connect.text = "Connect"
            mainWindow.theme.primaryColor = Palette.colors["red"]["500"]
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

    Column{
        id: column
        anchors.fill: parent
        anchors.margins: Units.dp(5)
        spacing: 3

        Row {
            id: buttonRow
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: Units.dp(4)

            Button {
                id: connect
                width: Units.dp(200)
                height: Units.dp(50)
                backgroundColor: Palette.colors["blue"]["300"]
                elevation: 1
                text: "Connect"
                enabled: true
                onClicked: {
                    if (statusText.text == "Disconnected"){
                        checkLogin()
                        console.log("in connect: login = " + loginState)
                        if (loginState == 1)
                            OvpnController.startConn()
                    }
                    else {
                        OvpnController.stopConn()
                    }
                }
                Label {
                    id: serverNameLbl
                    anchors.centerIn: parent
                    anchors.verticalCenterOffset: Units.dp(15)
                    text: OvpnController.serverName
                }
            }

            Button {
                id : serverSelection
                width: Units.dp(50)
                height: Units.dp(50)
                backgroundColor: Palette.colors["blue"]["300"]
                elevation: 1
                enabled: true
                Icon {
                    anchors.centerIn: parent
                    name: "awesome/map_marker"
                    size: Units.dp(24)
                }

                onClicked: {
                    serverDialog.show()
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

        //Status
        ListItem.Standard {
            showDivider: true
            content:
                Label {
                id: statusText
                text: "Disconnected"
                anchors.centerIn: parent
                font.family: "Roboto"
                font.weight: Font.DemiBold
                font.pixelSize: Units.dp(24)
            }
        }

        //Server Load Display
        ListItem.Standard {
            id: serverLoad
            interactive: false
            showDivider: true
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

        //Server IP Display
        ListItem.Standard {
            id: serverIp
            interactive: false
            showDivider: true
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

        //Profile display
        ListItem.Standard {
            id: profile
            interactive: false
            showDivider: true
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
    }

    Dialog {
        id: serverDialog
        width: Units.dp(300)
        title: "Servers"
        hasActions: true
        positiveButtonText: "Select Server"
        negativeButtonText: "Cancel"

        MenuField {
            id: comboServers
            width: Units.dp(200)
            model: comboModel
        }

/*            ListItem.Standard {
            text: "United Kingdom"
            action: CircleImage {
                anchors.fill: parent
                source: Qt.resolvedUrl("/flags/flags/GB.png")
            }
        }*/

        onAccepted: {
            console.log(servers[comboServers.selectedIndex].address)
            OvpnController.setServer(servers[comboServers.selectedIndex].address)
            OvpnController.setServerName(servers[comboServers.selectedIndex].name)
            OvpnController.setServerLoad(servers[comboServers.selectedIndex].load)
        }
    }

    Dialog {
        id: loginDialog
        width: Units.dp(300)
        title: "Login"
        hasActions: true
        positiveButtonText: "Login"
        negativeButtonText: "Cancel"

        TextField {
            id: usernameField
            placeholderText: "Username"
            floatingLabel: true
            anchors.horizontalCenter: parent.horizontalCenter
        }

        TextField {
            id: passwordField
            placeholderText: "Password"
            floatingLabel: true
            echoMode: TextInput.Password
            helperText: "Enter the password."
            anchors.horizontalCenter: parent.horizontalCenter
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
            console.log("in onAccepted: " + usernameField.length + " " + passwordField.length)
            if (usernameField.length > 0 && passwordField.length > 0){
                OvpnController.setUserName(usernameField.getText(0, usernameField.length))
                OvpnController.setUserPass(passwordField.getText(0, passwordField.length))
                loginState = 1
            }
            else
                loginState = 0
        }

        onRejected: {
            loginState = 0
        }
    }


    Component.onCompleted: {

  /*      if (!mainWindow.getServers("http://webzvpn.ru/servers.php")){
            debugLbl.text = "in this"
            if (!mainWindow.loadServerXml()){
                debugLbl.text = "tried to get xml"
                console.log("Failed to get servers! TODO: Retry server retrieval")
            }
        }*/

        mainWindow.getServers("http://webzvpn.ru/servers.php")

        //Attempt Auto-Connect
        autoConnect()
    }
}
