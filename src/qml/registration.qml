import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import Warnings 1.0

Window {
    id: registrationPage
    width: screen.width
    height: screen.height
    property string warning : warningIndicator.registrationWarning
    ColumnLayout {
        id: columnLayout
        width: parent.width
        height: parent.height
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        anchors.fill: parent
        Rectangle{
            gradient: Gradient{
                GradientStop { position: 0.0; color: "greenyellow" }
                GradientStop { position: 1.0; color: "green" }
            }
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop
            height: registrationPage.height/20
            ToolButton{
                text: "<"
                onClicked: controller.displayRegistrationWindow(false)
            }
        }
        TextField {
            id: newUser
            Layout.fillWidth: true
            placeholderText: qsTr("Entry new login")
            horizontalAlignment: Text.AlignHCenter
            ToolTip.timeout: 5000
            ToolTip.text: warning
            ToolTip.visible: warning
            onTextChanged: backendController.validateEnteredUsername(text, WarningType.REGISTRATION)
        }
        TextField {
            id: password
            Layout.fillWidth: true
            placeholderText: qsTr("Entry password")
            echoMode: TextInput.Password
            horizontalAlignment: Text.AlignHCenter
            onTextChanged: backendController.validateNewUserPassword(password.text)
        }
        Button {
            id: loginButton
            text: qsTr("Add new user")
            Layout.alignment: Qt.AlignHCenter
            display: AbstractButton.TextBesideIcon
            flat: false
            focusPolicy: Qt.TabFocus
            Material.background: Material.Orange
            onClicked: {
                if (backendController.validateNewUserPassword(password.text) &&
                    backendController.validateEnteredUsername(newUser.text, WarningType.REGISTRATION))
                {
                    backendController.addNewUser(newUser.text, password.text)
                    controller.displayMainAppWindow(true, newUser.text)
                    return;
                }
                warningDialog.open()
            }
        }
        MessageDialog{
            id: warningDialog
            title: "Warning"
            text: "Unable to add new user"
            informativeText: registrationPage.warning
            icon: StandardIcon.Warning
        }
    }
    onClosing: displayRegistrationWindow(false)
}
