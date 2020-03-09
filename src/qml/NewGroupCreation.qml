import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Dialogs 1.1
import Warnings 1.0

Window {
    id: groupCreationPage
    width: screen.width
    height: screen.height
    property string warning: warningIndicator.groupCreationWarning
    ColumnLayout{
        id: groupCreationLayout
        anchors.fill: parent
        ToolButton{
            text: "<"
            onClicked: displayNewGroupCreation(false)
            anchors.left: parent.left
            anchors.top: parent.top
        }
        TextField{
            id: newGroupName
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            placeholderText: qsTr("Entry group name")
            ToolTip.timeout: 5000
            ToolTip.visible: warning
            ToolTip.text: warning
            onTextChanged: backendController.validateGroupName(newGroupName.text)
        }
        GroupBox{
            title: qsTr("Group Users")
            Layout.alignment: Qt.AlignHCenter
            ColumnLayout{
                RowLayout{
                    ComboBox{
                        id: friendsBox
                        model: friendsModel
                        onCurrentTextChanged:{
                           console.log("text " + currentText)
                            displayText = currentText
                            if (existInModel(currentText))
                            {
                                console.log("User: " + currentText +  " already added")
                                return
                            }
                            addedFriendsModel.append({friendName: currentText})
                        }
                        textRole: "related_user"
                        displayText: "Quick add friend to group"
                        currentIndex: -1
                        delegate: ItemDelegate{
                            contentItem: Text {
                                horizontalAlignment: Text.AlignHCenter
                                text: model.related_user
                                color: "black"
                            }
                        }
                    }
                    TextField{
                        id: addUserToGroupBySearch
                        horizontalAlignment: Text.AlignHCenter
                        placeholderText: qsTr("Entry login to add")
                    }
                    Button{
                        id: addUserToGroupButton
                        text: qsTr("Assign user to group")
                        onClicked: {
                            if (backendController.validateEnteredUsername(addUserToGroupBySearch.text, WarningType.GROUP_CREATION)){
                                if (existInModel(addUserToGroupBySearch.text)){
                                    showErrDialog("User: " + addUserToGroupBySearch.text +  " already added")
                                    return
                                }
                                if (addUserToGroupBySearch.text === controller.username){
                                    showErrDialog("Unable to add yourself")
                                    return
                                }
                                addedFriendsModel.append({friendName: addUserToGroupBySearch.text})
                                return;
                            }
                            showErrDialog("Unable to find user: " + addUserToGroupBySearch.text)
                        }
                    }
                }
                Pane {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    ScrollView{
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        ListView {
                            Layout.alignment: Qt.AlignHCenter
                            Layout.fillWidth: true
                            id: addedFriendsList
                            property var elements: []
                            model: addedFriendsModel
                            delegate: delegateFriendsModel
                            spacing: 5
                        }
                        ListModel{id: addedFriendsModel}
                    }
                }
            }
        }
        Button {
            id: addNewGroupButton
            text: qsTr("Add new group")
            font.weight: Font.Light
            display: AbstractButton.TextBesideIcon
            flat: false
            focusPolicy: Qt.TabFocus
            Material.background: Material.Grey
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            onClicked: {
                if (backendController.addGroup(newGroupName.text, addedFriendsList.elements)) {
                    displayNewGroupCreation(false)
                }
            }
        }
    }
    Component {
        id: delegateFriendsModel
        RowLayout{
            Label {
                text: friendName
                Component.onCompleted: addedFriendsList.elements.push(friendName)
            }
            Button{
                text: qsTr("X")
                onClicked: {
                    addedFriendsModel.remove(friendName)
                    addedFriendsList.elements.remove(friendName)
                }
            }
        }
    }

    MessageDialog{
        id: errorDialog
        title: "Group creation info"
        text: warning
        icon: StandardIcon.Information
    }

    function existInModel(searchedFriendName){
        for (var i = 0; i < addedFriendsModel.count; i++) {
            if (addedFriendsModel.get(i).friendName === searchedFriendName) {
                return true
            }
        }
        return false
    }

    function showErrDialog(warning)
    {
        errorDialog.text = warning
        errorDialog.open()
    }

    onClosing: displayNewGroupCreation(false)
}
