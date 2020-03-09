import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import Warnings 1.0

Page {
    id: friendsPage
    Layout.fillHeight: true
    Layout.fillWidth: true
    background: Rectangle {color: "black"}

    ScrollView{
        width: parent.width
        height: parent.height
        clip: true
        ListView{
            anchors.horizontalCenter: parent.horizontalCenter
            model: friendsModel
            spacing: 5
            delegate: Rectangle{
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width
                color: "transparent"
                height: 100
                Rectangle{
                    id:rect
                    radius: 15
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width/2
                    height: parent.height
                    gradient: Gradient{
                        GradientStop { position: 0.0; color: "lightgreen" }
                        GradientStop { position: 1.0; color: "green" }
                    }
                    border.color: "white"
                    ColumnLayout{
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        Text {
                            id: friendText
                            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                            color: "black"
                            text: model.related_user
                            font.pixelSize: 20
                        }
                        Text {
                            property double balance: friendsModel.getBalanceWithFriend(model.related_user)
                            Layout.alignment: Qt.AlignHCenter
                            font.pixelSize: 20
                            text: "BALANCE: " + balance
                            color: balance >= 0 ? "black" : "red"
                            rightPadding: 10
                        }
                    }
                }
                RoundButton{
                    text: "X"
                    anchors.left: rect.right
                    height: 100
                    width: 100
                    onClicked: backendController.removeFriend(friendText.text)
                }
            }
        }
    }

    Dialog {
        id: addFriendDialog
        title: "Add friend"
        standardButtons: Dialog.Ok | Dialog.Cancel
        TextField {
            id: friendNameInput
            property bool loginExist: true
            horizontalAlignment: TextInput.AlignHCenter
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width
            placeholderText: qsTr("Please entry friend login")
            ToolTip.timeout: 5000
            ToolTip.visible: !loginExist
            ToolTip.text: qsTr("User " + text + " not exist")
            onTextChanged: loginExist = backendController.validateEnteredUsername(friendNameInput.text, WarningType.VALIDATION)
        }
        MessageDialog{
            id: addFriendResultMsg
            property bool result
            title: "Add Friend Info"
            width: implicitWidth
            height: implicitHeight
            text: result ? qsTr("Succesfully add friend " + friendNameInput.text) : qsTr("Failed to add friend " + friendNameInput.text)
            icon: result ? StandardIcon.Information : StandardIcon.Warning
        }
        onButtonClicked: {
            if (clickedButton === StandardButton.Ok) {
                if (backendController.validateEnteredUsername(friendNameInput.text, WarningType.VALIDATION)){
                    addFriendResultMsg.result = backendController.addFriend(friendNameInput.text)
                }
                else{
                    addFriendResultMsg.result = false
                    console.log("Cannot add friend. Login " + friendNameInput.text + " does not exist")
                }
                addFriendResultMsg.open()
            }
        }
    }

    footer: Button{
        id: addFriendButton
        width:parent.width
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        display: AbstractButton.TextBesideIcon
        text: "Add friend"
        Material.background: Material.BlueGrey
        icon.source: "qrc:/media/media/icons/person_add-24px.svg"
        onClicked: {
            addFriendDialog.open()
        }
    }
}
