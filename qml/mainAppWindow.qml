import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Material 2.2

ApplicationWindow {
    id: mainPage
    width: screen.width
    height: screen.height
    background: Rectangle{
        color: "black"
    }

    header: ToolBar{
        background: Rectangle {
            color: "darkorange"
        }
        RowLayout{
            anchors.fill: parent
            ToolButton{
                Text {
                    anchors{
                        horizontalCenter: parent.horizontalCenter
                        verticalCenter: parent.verticalCenter
                    }
                    text: qsTr("<")
                    color: "black"
                }
                onClicked: displayMainAppWindow(false, "")
            }
            Label {
                text: qsTr("User: " + controller.username + "     Total balance: " + groupModel.userTotalBalance) + " PLN"
                font.pixelSize: Qt.application.font.pixelSize * 2
                padding: 10
                color: "black"
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }
            ToolButton{
                id: menuButton
                Text {
                    anchors{
                        horizontalCenter: parent.horizontalCenter
                        verticalCenter: parent.verticalCenter
                    }
                    text: qsTr("⋮")
                    color: "black"
                }
                onClicked: menu.popup()
            }
        }
    }

    Menu{
        id: menu
        MenuItem{
            text: "Logout"
            onClicked: displayMainAppWindow(false, "")
        }
        MenuItem{
            text: "DeleteAccount"
            onClicked: deleteAccountDialog.open()
        }
        MenuItem{
            text: "Exit app"
            onClicked: backendController.endApp()
        }
    }

    Dialog {
        id: paswdValidationDelete
        title: "Validate your password"
        standardButtons: StandardButton.Ok | StandardButton.Cancel
        Column {
            anchors.fill: parent
            Text {
                text: "To delete account please entry password of user: " + controller.username
            }
            TextField {
                id: paswdInput
                echoMode: TextInput.Password
                placeholderText: qsTr("Entry password")
            }
        }
        MessageDialog{
            id: deleteResultMsg
            property bool result : false
            title: "Delete User Info"
            text: result ? qsTr("Succesfully deleted user " + controller.username) : qsTr("Failed to delete user " + controller.username)
            icon: result ? StandardIcon.Information : StandardIcon.Critical
        }
        onButtonClicked: {
            if (clickedButton === StandardButton.Ok) {
                if (backendController.validateEnteredPassword(controller.username, paswdInput.text))
                {
                    console.log("call delete of user " + controller.username)
                    deleteResultMsg.result = backendController.deleteUser(controller.username)
                    if (eleteResultMsg.result === true){displayMainAppWindow(false, "")}
                }
                deleteResultMsg.open()
            }
        }
    }

    MessageDialog{
        id: deleteAccountDialog
        title: "Warning"
        text: "Are you sure that you want to delete your account permanently?"
        icon: StandardIcon.Question
        standardButtons: StandardButton.Yes | StandardButton.No
        onButtonClicked: {
            if (clickedButton === StandardButton.Yes) {
                paswdValidationDelete.open()
            }
        }
    }

    ColumnLayout{
        id: tabColumns
        width: parent.width
        height: parent.height
        TabBar {
            id: tabBar
            Layout.alignment: Qt.AlignTop
            Layout.fillWidth: true
            currentIndex: swipeView.currentIndex
            height: tabColumns.height
            background: Rectangle {
                color: "black"
            }
            TabButton {
                text: qsTr("Your Groups")
                background: Rectangle {
                    color: tabBar.currentIndex == 0 ? "darkorange" : "sandybrown"
                }
            }
            TabButton {
                text: qsTr("Friends")
                background: Rectangle {
                    color: tabBar.currentIndex == 1 ? "darkorange" : "sandybrown"
                }
            }
            TabButton {
                text: qsTr("Last Activities")
                background: Rectangle {
                    color: tabBar.currentIndex == 2 ? "darkorange" : "sandybrown"
                }
            }
        }
        SwipeView {
            id: swipeView
            Layout.fillHeight: true
            Layout.fillWidth: true
            currentIndex: tabBar.currentIndex
            GroupsPage {
            }
            FriendsPage {
            }
            LastActivityPage {
            }
        }
    }
    onClosing: displayMainAppWindow(false, "")
}
