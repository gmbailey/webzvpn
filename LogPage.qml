import QtQuick 2.4
import QtQuick.Controls 1.4 as QTControls
import Material 0.2

Item {
    property string logLine

    Connections {
        target:OvpnController
        onLogTxtChanged: {
            logField.append(logTxt)
        }
    }

    View {
        anchors {
            fill: parent
            margins: Units.dp(4)
        }

        elevation: 1

        QTControls.TextArea{
            anchors.fill: parent
            anchors.margins: 10
            id: logField
            readOnly: true
            text: OvpnController.logText
        }
    }
}
