import QtQuick 2.4
import QtQuick.Layouts 1.1
import Material 0.2
import Material.ListItems 0.1 as ListItem
import Material.Extras 0.1
import QtQuick.Controls 1.4 as QTControls

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

            Item {
                id: updateSection
                width: parent.width
                Layout.preferredHeight: Units.dp(105)
                Layout.leftMargin: Units.dp(15)
                Layout.topMargin: Units.dp(15)
                Row {
                    spacing: Units.dp(25)
                    Icon {
                        id: updIcn
                        name: "action/update"
                        size: Units.dp(16)
                    }

                    Column {
                        width: Units.dp(125)
                        height: parent.height
                        Label {
                            text: "Updates"
                            font.pixelSize: Units.dp(12)
                        }

                        Item{
                            width: parent.width
                            height: Units.dp(15)
                        }

                        Label {
                            text: "Current"
                            color: Theme.light.textColor
                            id: curLbl
                        }
                        Label {
                            text: OvpnController.curVersion
                            color: Theme.light.subTextColor
                        }

                        Item{
                            width: parent.width
                            height: Units.dp(15)
                        }

                        Button {
                            id: updateButton
                            backgroundColor: Theme.backgroundColor
                            elevation: 1
                            text: "Update"
                            implicitHeight: Units.dp(25)

                            onClicked: {
                                OvpnController.launchUpdater()
                            }
                        }
                    }

                    Column {
                        width: Units.dp(125)
                        height: parent.height
                        Item{
                            width: parent.width
                            height: curLbl.height + Units.dp(15)
                        }
                        Label {
                            text: "Latest"
                            color: Theme.light.textColor
                        }
                        Label {
                            text: OvpnController.latestVersion//"0.3"
                            color: Theme.light.subTextColor
                        }
                    }
                }
            }

            Item {
                id: loginSection
                width: parent.width
                Layout.preferredHeight: Units.dp(105)
                Layout.leftMargin: Units.dp(15)
                Layout.topMargin: Units.dp(15)
                Row {
                    spacing: Units.dp(25)
                    Icon {
                        id: logIcn
                        name: "action/account_circle"
                        size: Units.dp(16)
                    }

                    Column {
                        width: Units.dp(125)
                        height: parent.height
                        Label {
                            text: "Login Info"
                            font.pixelSize: Units.dp(12)
                        }

                        Item{
                            width: parent.width
                            height: Units.dp(15)
                        }

                        Label {
                            text: "Login"
                            color: Theme.light.textColor
                            id: loginLbl
                        }
                        Label {
                            text: OvpnController.userName
                            color: Theme.light.subTextColor
                        }

                        Item{
                            width: parent.width
                            height: Units.dp(15)
                        }

                        Button {
                            id: loginButton
                            backgroundColor: Theme.backgroundColor
                            elevation: 1
                            text: "Change"
                            implicitHeight: Units.dp(25)
                            onClicked:{
                                changeLoginDialog.show()
                            }
                        }
                    }

                    Column {
                        width: Units.dp(125)
                        height: parent.height
                        Item{
                            width: parent.width
                            height: loginLbl.height + Units.dp(15)
                        }
                        Label {
                            text: "Password"
                            color: Theme.light.textColor
                        }
                        Label {
                            text: "********"
                            color: Theme.light.subTextColor
                        }
                    }
                }
            }

            Item {
                id: profileSection
                width: parent.width
                Layout.preferredHeight: Units.dp(105)
                Layout.leftMargin: Units.dp(15)
                Layout.topMargin: Units.dp(15)
                Row {
                    spacing: Units.dp(25)
                    Icon {
                        id: profileIcn
                        name: "action/settings_ethernet"
                        size: Units.dp(16)
                    }

                    Column {
                        width: Units.dp(125)
                        height: parent.height
                        Label {
                            text: "Profile"
                            font.pixelSize: Units.dp(12)
                        }

                        Item{
                            width: parent.width
                            height: Units.dp(15)
                        }

                        ListItem.SimpleMenu {
                            id: menuProfile
                            text: profileModel[selectedIndex]
                            model: profileModel
                            implicitHeight: Units.dp(20)
                            subText: ""
                        }
                    }
                }
            }

/*            Item{
                height: Units.dp(8)
            }

            Label {
                Layout.leftMargin: Units.dp(5)
                Layout.alignment: Qt.AlignLeft
                text: "Updates:"
                font.pixelSize: Units.dp(14)
                color: Theme.light.textColor
            }

            Label {
                Layout.leftMargin: Units.dp(10)
                text: "Current Version: " + OvpnController.curVersion
                font.pixelSize: Units.dp(12)
                color: Theme.light.subTextColor
            }
            Label {
                Layout.leftMargin: Units.dp(10)
                text: "Latest Version: "
                font.pixelSize: Units.dp(12)
                color: Theme.light.subTextColor
            }
            Button {
                Layout.leftMargin: Units.dp(10)
                id: updateButton
                backgroundColor: Theme.backgroundColor
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
                font.pixelSize: Units.dp(14)
                color: Theme.light.textColor
            }

            Button {
                Layout.leftMargin: Units.dp(10)
                backgroundColor: Theme.backgroundColor
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
                font.pixelSize: Units.dp(14)
            }
*/

    /*        MenuField{
                id: profileCombo
                Layout.leftMargin: Units.dp(12)
                Layout.preferredWidth: Units.dp(125)
                model: ["UDP 50005"]
            }*/

     /*       ListItem.SimpleMenu {
                id: menuProfile
                text: profileModel[selectedIndex]
                model: profileModel
                implicitHeight: Units.dp(20)
            }
*/
   /*         ListItem.Divider{
                Layout.fillWidth: parent
            }*/

            Item {
                id: otherSection
                width: parent.width
                Layout.preferredHeight: Units.dp(105)
                Layout.leftMargin: Units.dp(15)
                Layout.topMargin: Units.dp(15)
                Row {
                    spacing: Units.dp(25)
                    Icon {
                        id: otherIcn
                        name: "action/settings"
                        size: Units.dp(16)
                    }
                    Column{
                        spacing: Units.dp(8)
                        Label {
                            text: "Other Settings"
                            font.pixelSize: Units.dp(12)
                        }

                        Item{
                            width: parent.width
                            height: Units.dp(15)
                        }

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
                                color: Theme.light.textColor
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
                                color: Theme.light.textColor
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
                                color: Theme.light.textColor
                            }
                        }
                    }
                }
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

        ListItem.Standard{
            height: Units.dp(10)
        }

        TextField {
            id: usernameField
            placeholderText: "Username"
            floatingLabel: true
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
