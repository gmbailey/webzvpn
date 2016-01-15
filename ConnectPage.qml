import QtQuick 2.0
import Material 0.1

Page {
    id: connectPage

    function updateStatus(status) {
        switch (status) {
        case 0:
            statusText.text = "Disconnected"
            busy.indeterminate = false
            connect.enabled = true
            disconnect.enabled = false
            break
        case 1:
            statusText.text = "Connecting"
            busy.indeterminate = true
            connect.enabled = false
            disconnect.enabled = true
            break
        case 2:
            statusText.text = "Connected"
            busy.indeterminate = false
            connect.enabled = false
            disconnect.enabled = true
            break
        case 3:
            statusText.text = "Disconnecting"
            busy.indeterminate = true
            connect.enabled = false
            disconnect.enabled = false
            break
        }
    }

    Connections {
        target:OvpnController
        onStateChanged: {
            updateStatus(status)
        }
    }
    Column {
        id: column

        width: parent.width
        spacing: Theme.paddingLarge

        Row {
            spacing: Theme.paddingLarge
            anchors.horizontalCenter: parent.horizontalCenter
            Button {
                id: connect
                text: "Connect"
                enabled: true
                onClicked: {
                    OvpnController.logAppend('\n')
                    OvpnController.startConn()
                }
            }
            Button {
                id : disconnect
                text: "Disconnect"
                enabled: true
                onClicked: OvpnController.stopConn();
            }
        }

        Label {
            id: statusText
            text: "No status"
            color: Theme.highlightColor
            anchors.horizontalCenter: parent.horizontalCenter
            font.family: Theme.fontFamilyHeading
        }
        Row {
            spacing: Theme.paddingLarge
            anchors.horizontalCenter: parent.horizontalCenter
            ProgressCircle {
                id: busy
                anchors.verticalCenter: parent.verticalCenter
                indeterminate: false
                minimumValue: 0
                maximumValue: 100

            }
        }

        Rectangle {
            color: "transparent"
            border {
                color: Theme.highlightBackgroundColor
                width: 1
            }
            //radius: Theme.paddingSmall
            anchors.horizontalCenter: parent.horizontalCenter
            height: (24 * Theme.fontSizeTiny) + (2 * Theme.paddingLarge)
            width: parent.width - 2 * Theme.paddingLarge
            x: Theme.paddingLarge

            //TextEdit {
            Label {
                id: logOutput
                textFormat: Text.PlainText
                width: parent.width - 2 * Theme.paddingSmall
                height: parent.height - 0 * Theme.paddingSmall
                wrapMode: Text.WrapAnywhere
                font.pixelSize: Theme.fontSizeTiny * 0.6
                font.family: "Monospace"
                color: Theme.highlightColor
                visible: true
                text: OvpnController.logText
                verticalAlignment: Text.AlignBottom
                x: Theme.paddingSmall
                y: Theme.paddingSmall
                //readOnly: true
                clip: true
            }
        }
    }
}
