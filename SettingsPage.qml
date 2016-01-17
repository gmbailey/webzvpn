import QtQuick 2.4
import QtQuick.Layouts 1.1
import Material 0.2
import Material.ListItems 0.1 as ListItem
import Material.Extras 0.1

Item {
    property var profileModel: ["UDP 50005"]

    function restoreSettings(){
        autoStartSwitch.checked = OvpnController.autoStart
        autoReconnectSwitch.checked = OvpnController.autoReconnect
        autoConnectSwitch.checked = OvpnController.autoConnect
    }


    View {
        anchors {
            fill: parent
            margins: Units.dp(4)
        }

        elevation: 1

        ColumnLayout {
            id: settingsColumn
            anchors.fill: parent
            spacing: Units.dp(2)

            Label {
                Layout.leftMargin: Units.dp(5)
                Layout.alignment: Qt.AlignLeft

                text: "Updates:"
                font.pixelSize: Units.dp(10)
            }

            Label {
                Layout.leftMargin: Units.dp(10)
                text: "Current Version: " + OvpnController.curVersion
                font.pixelSize: Units.dp(12)
            }
            Label {
                Layout.leftMargin: Units.dp(10)
                text: "Latest Version: "
                font.pixelSize: Units.dp(12)
            }
            Button {
                Layout.leftMargin: Units.dp(10)
                id: updateButton
                backgroundColor: Palette.colors["blue"]["700"]
                elevation: 1
                text: "Update"
                implicitHeight: Units.dp(25)
            }

            ListItem.Divider{
                Layout.fillWidth: parent
            }

            Label {
                Layout.leftMargin: Units.dp(5)
                text: "Login Info:"
                font.pixelSize: Units.dp(10)
            }

            Button {
                Layout.leftMargin: Units.dp(10)
                backgroundColor: Palette.colors["blue"]["700"]
                elevation: 1
                text: "Change"
                implicitHeight: Units.dp(25)
                onClicked:{
                    changeLoginDialog.show()
                }
            }

            ListItem.Divider{
                Layout.fillWidth: parent
            }

            Label {
                Layout.leftMargin: Units.dp(5)
                text: "Profiles:"
                font.pixelSize: Units.dp(10)
            }


    /*        MenuField{
                id: profileCombo
                Layout.leftMargin: Units.dp(12)
                Layout.preferredWidth: Units.dp(125)
                model: ["UDP 5005"]
            }*/


            ListItem.SimpleMenu {
                id: menuProfile
                text: profileModel[selectedIndex]
                model: profileModel
                implicitHeight: Units.dp(20)
            }

            ListItem.Divider{
                Layout.fillWidth: parent
            }

            Column{
                spacing: Units.dp(8)
                Row {
                    spacing: Units.dp(10)
                    Switch {
                        id: autoStartSwitch
                        checked: true
                        height: Units.dp(15)
                        color: Palette.colors["blue"]["200"]
                        onClicked: {
                            console.log("autoLaunch : " + checked)
                            OvpnController.autoStart = checked
                        }
                    }
                    Label {
                        id: autoLaunchLbl
                        text: "Auto-launch app on startup"
                    }
                }

                Row {
                    spacing: Units.dp(10)
                    Switch {
                        id: autoConnectSwitch
                        checked: true
                        height: Units.dp(15)
                        color: Palette.colors["blue"]["200"]
                        onClicked: {
                            console.log("reconnect : " + checked)
                            OvpnController.autoConnect = checked
                        }
                    }
                    Label {
                        id: autoConnLbl
                        text: "Auto-connect when app starts"
                    }
                }

                Row {
                    spacing: Units.dp(10)
                    Switch {
                        id: autoReconnectSwitch
                        checked: true
                        height: Units.dp(15)
                        color: Palette.colors["blue"]["200"]
                        onClicked: {
                            console.log("autoConnect : " + checked)
                            OvpnController.autoReconnect = checked
                        }
                    }
                    Label {
                        id: autoReconnLbl
                        text: "Re-connect when connection drops"
                    }
                }
            }

            ListItem.Divider{
                Layout.fillWidth: parent
            }

            Label {
                Layout.leftMargin: Units.dp(5)
                text: "Logs:"
                font.pixelSize: Units.dp(10)
            }

            Item {
                Layout.preferredHeight: Units.dp(3)
            }

            Button {
                Layout.leftMargin: Units.dp(10)
                backgroundColor: Palette.colors["blue"]["700"]
                elevation: 1
                text: "Show Logs"
                implicitHeight: Units.dp(25)
            }


            Item{
                Layout.fillHeight: parent
            }
        }
    }

    Dialog {
        id: changeLoginDialog
        width: Units.dp(300)
        title: "Change Login"
        hasActions: true
        positiveButtonText: "Save"
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
            if (usernameField.getText(0, usernameField.length) > 0 && passwordField.getText(0, passwordField.length) > 0){
                OvpnController.setUserName(usernameField.getText(0, usernameField.length))
                OvpnController.setUserPass(passwordField.getText(0, passwordField.length))
            }
        }
    }

    Component.onCompleted: {
        restoreSettings()
    }
}
