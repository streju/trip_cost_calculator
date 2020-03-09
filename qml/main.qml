import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Warnings 1.0
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.2

ApplicationWindow {
    id: applicationWindow
    Controller{id: controller}
    property string warning : warningIndicator.validationWarning
    visible: true
    width: screen.width
    height: screen.height
    title: qsTr("Trip cost calculator")
    Image{
        id: programLogo
        anchors.top: parent.top
        width: parent.width
        height: 0.3 * parent.height
        source: "qrc:/media/media/img/main_img.jpg"
        Text {
            id: programName
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: programLogo.top
            anchors.topMargin: 20
            text: qsTr("Trip cost calculator")
            font.bold: true
            font.italic: true
            font.family: "Helvetica"
            font.pointSize: Font.Normal
            minimumPointSize:20
            fontSizeMode: Text.Fit
            visible: false
        }
        LinearGradient{
            anchors.fill: programName
            source: programName
            gradient: Gradient {
                GradientStop { position: 0; color: "grey" }
                GradientStop { position: 0.5; color: "black" }
            }
        }
    }

    Rectangle {
        id: input
        anchors.top: programLogo.bottom
        width: parent.width
        height: parent.height - programLogo.height
        anchors {
            horizontalCenter: applicationWindow.horizontalCenter
        }
        color: "black"
        ColumnLayout {
            id: columnLayout
            width: parent.width
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            anchors {
                horizontalCenter: applicationWindow.horizontalCenter
            }
            anchors.fill: parent
            TextField {
                id: user
                width: applicationWindow.width
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.fillWidth: true
                placeholderText: qsTr("Entry your login")
                placeholderTextColor: "darkorange"
                color: "darkorange"
                background: Item {
                    Rectangle {
                        color: "darkorange"
                        height: 2
                        width: parent.width
                        anchors.bottom: parent.bottom
                    }
                }
                horizontalAlignment: Text.AlignHCenter
                ToolTip.timeout: 5000
                ToolTip.visible: warning
                ToolTip.text: warning
                onTextChanged: backendController.validateEnteredUsername(text, WarningType.VALIDATION)
            }

            TextField {
                id: passwordTxt
                width: applicationWindow.width
                Layout.fillWidth: true
                placeholderText: qsTr("Entry password")
                echoMode: TextInput.Password
                horizontalAlignment: Text.AlignHCenter
                placeholderTextColor: "darkorange"
                color: "darkorange"
                background: Item {
                    Rectangle {
                        color: "darkorange"
                        height: 2
                        width: parent.width
                        anchors.bottom: parent.bottom
                    }
                }
                onTextChanged: backendController.validateEnteredPassword(user.text, text)
            }

            Button {
                id: loginButton
                text: qsTr("Login")
                Layout.preferredWidth: applicationWindow.width / 2
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                display: AbstractButton.TextBesideIcon
                flat: false
                focusPolicy: Qt.TabFocus
                background: Rectangle{
                    anchors.fill: parent
                    gradient: Gradient{
                        GradientStop { position: 0.0; color: "orange" }
                        GradientStop { position: 1.0; color: "orangered" }
                    }
                }
                onClicked: {
                    if (backendController.logInUser(user.text, passwordTxt.text))
                    {
                        var login = user.text
                        clearInputs()
                        controller.displayMainAppWindow(true, login)
                        return;
                    }
                    console.log("Warning: " + warning)
                    wrnDialog.text = warning
                    wrnDialog.open()
                }
            }

            Button {
                id: registrationButton
                text: qsTr("Register new user")
                Layout.preferredWidth: applicationWindow.width / 2
                font.weight: Font.Light
                display: AbstractButton.TextBesideIcon
                flat: false
                focusPolicy: Qt.TabFocus
                background: Rectangle{
                    anchors.fill: parent
                    gradient: Gradient{
                        GradientStop { position: 0.0; color: "orange" }
                        GradientStop { position: 1.0; color: "orangered" }
                    }
                }
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                onClicked: controller.displayRegistrationWindow(true)
            }
            Button {
                id: exitButton
                width: contentItem.implicitWidth + leftPadding + rightPadding
                text: qsTr("EXIT")
                contentItem: Text {
                    color: "orange"
                    text: exitButton.text
                }
                Layout.alignment: Qt.AlignRight | Qt.AlignBottom
                background: Rectangle{
                    radius: 15
                    anchors.fill: parent
                    border.color: "orange"
                    color: "black"
                }
                onClicked: backendController.endApp()
            }
        }
    }

    MessageDialog{
        id: wrnDialog
        title: "Warning LogIn"
        icon: StandardIcon.Warning
    }

    function clearInputs(){
        user.text = ""
        passwordTxt.text = ""
    }
}
