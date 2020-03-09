import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import Warnings 1.0

Window {
    id: groupDetailedView
    width: screen.width
    height: screen.height
    property string warning: warningIndicator.groupOperationsWarning
    ColumnLayout{
        width: parent.width
        height: parent.height
        Rectangle{
            id: groupInformation
            Layout.alignment: Qt.AlignTop
            Layout.fillWidth: true
            Layout.preferredHeight: parent.height*0.2
            color: "green"
            border.color: "white"
            border.width: 10
            radius: 10
            gradient: Gradient{
                GradientStop { position: 0.0; color: "greenyellow" }
                GradientStop { position: 1.0; color: "green" }
            }
            ToolButton{
                text: "<"
                onClicked: controller.displayGroupDetailView(false, "")
            }
            ColumnLayout{
                anchors{
                    horizontalCenter: parent.horizontalCenter
                    verticalCenter: parent.verticalCenter
                }
                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: "Group: " + stateProvider.groupname
                    font.pixelSize: Font.Normal
                }
                RowLayout{
                    Layout.topMargin: 10
                    Button{
                        id: addNewBillButton
                        text: qsTr("Add new bill")
                        Layout.preferredWidth: screen.width/10
                        Layout.fillWidth: true
                        onClicked: {
                            backendController.updateCurrencyRates()
                            addGroupActivityDialog.open()
                        }
                    }
                    Button{
                        id: addNewGroupUserButton
                        text: qsTr("Add user")
                        Layout.preferredWidth: screen.width/10
                        Layout.fillWidth: true
                        onClicked: addNewUserToGroupDialog.open()
                    }
                    Button{
                        id: removeFromGroupButton
                        text: qsTr("Remove from group")
                        Layout.fillWidth: true
                        onClicked: removeUserFromGroupDialog.open()
                    }
                    Button{
                        id: deleteGroupButton
                        text: qsTr("Delete group")
                        Layout.preferredWidth: screen.width/10
                        Layout.fillWidth: true
                        onClicked: deleteGroupDialog.open()
                    }
                }
            }
        }
        Rectangle{
            id: details
            Layout.fillWidth: true
            Layout.fillHeight: true
            SwipeView{
                 anchors.fill : parent
                 Page{
                    id: pageGroupUsersDetails
                    width: details.width
                    height: details.height
                    ListView{
                        id: groupUsersList
                        model: groupDetailsModel
                        anchors.fill : parent
                        spacing: 5
                        delegate: Rectangle{
                            radius: 15
                            anchors.horizontalCenter: parent.horizontalCenter
                            width: screen.width/3
                            height: 100
                            color: "grey"
                            Text {
                                id: userName
                                anchors{left: parent.left; verticalCenter: parent.verticalCenter}
                                text: model.related_user
                                leftPadding: 20
                                Component.onCompleted:{
                                    console.log("userName " + userName.text)
                                    modelUserToBill.append({userLogin: userName.text})
                                }
                            }
                            Text {
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.verticalCenter: parent.verticalCenter
                                color: model.value >= 0 ? "black" : "red"
                                text: qsTr("Current balance: " + model.value)
                            }
                            Button{
                                id: clearDebtButton
                                anchors{right: parent.right; verticalCenter: parent.verticalCenter}
                                text: qsTr("Clear balance")
                                onClicked: backendController.clearDebt(userName.text)
                            }
                        }
                    }
                }
                Page{
                    id: pageGroupActivity
                    width: groupDetailedView.width
                    height: details.height
                    ListView{
                        id: grActivitiesList
                        model: singleGroupActivitiesModel
                        anchors.fill : parent
                        spacing: 5
                        delegate: RowLayout{
                            anchors.horizontalCenter: parent.horizontalCenter
                            Image{source: "qrc:/media/media/icons/receipt-24px.svg"}
                            Rectangle{
                                height: 100
                                border.color: "black"
                                Text {
                                    id: activity
                                    text: model.description
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Dialog{
        id: addGroupActivityDialog
        title: qsTr("New " + stateProvider.groupname + " bill")
        standardButtons: StandardButton.Save | StandardButton.Cancel
        height: groupDetailedView.height/2
        width: groupDetailedView.width/2
        ColumnLayout{
            anchors.fill: parent
            TextField{
                id: descriptionText
                Layout.fillWidth: true
                placeholderText: "Add description"
                horizontalAlignment: Text.AlignHCenter
            }
            GroupBox{
                title: qsTr("Currency rate")
                Layout.alignment: Qt.AlignHCenter
                GridLayout{
                    columns: 3
                    rows: 2
                    Image{
                        source: "qrc:/media/media/img/usa_flag.png"
                    }
                    Image{
                        source: "qrc:/media/media/img/eu_flag.png"
                    }
                    Image{
                        source: "qrc:/media/media/img/uk_flag.png"
                    }
                    Text{
                        Layout.alignment: Qt.AlignHCenter
                        text: currencyService.usdRate.toFixed(4)
                    }
                    Text{
                        Layout.alignment: Qt.AlignHCenter
                        text: currencyService.eurRate.toFixed(4)
                    }
                    Text{
                        Layout.alignment: Qt.AlignHCenter
                        text: currencyService.gbpRate.toFixed(4)
                    }
                }
            }
            RowLayout{
                Layout.alignment: Qt.AlignHCenter
                CheckBox{
                    id: manualyEdit
                    checked: false
                    text: "Edit manualy"
                }
                TextField{
                    id: totalValue
                    readOnly: manualyEdit.checked
                    placeholderText: "Total value"
                }
            }
            ScrollView{
                height: addGroupActivityDialog.height/2
                Layout.fillWidth: true
                clip: true
                ListView{
                    Layout.alignment: Qt.AlignHCenter
                    id: listUserToBill
                    model: modelUserToBill
                    delegate: delegateUserToBillModel
                    Layout.fillWidth: true
                    spacing: 5
                }
                ListModel{
                    id: modelUserToBill
                }
            }

        }
        onButtonClicked: {
            if (clickedButton === StandardButton.Save) {
                if (!descriptionText){
                    showMsg("Description cannot be empty!")
                }
                if (manualyEdit.checked){
                    for (var child in listUserToBill.contentItem.children) {
                        if (listUserToBill.contentItem.children[child].isUserCountedInBill && listUserToBill.contentItem.children[child].acceptableValue)
                        {
                            console.log("login i: " + listUserToBill.contentItem.children[child].name)
                            console.log("value i: " + listUserToBill.contentItem.children[child].enteredValueStr)
                            backendController.registerUserBill(listUserToBill.contentItem.children[child].name,
                                listUserToBill.contentItem.children[child].enteredValueStr)
                        }
                    }
                    backendController.addBill(descriptionText.text)
                    return
                }
                backendController.addBillEqually(descriptionText.text, totalValue.text)
            }
        }
    }

    Dialog{
        id: addNewUserToGroupDialog
        title: qsTr("Add new user to " + controller.groupname)
        standardButtons: StandardButton.Ok | StandardButton.Cancel
        height: groupDetailedView.height/4
        width: groupDetailedView.width/4
        TextField{
            id: assingUserToGroupButton
            width: parent.width
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            placeholderText: "Please entry username to add"
        }
        onButtonClicked: {
            if (clickedButton === StandardButton.Ok) {
                modelUserToBill.clear()
                backendController.addUserToGroup(assingUserToGroupButton.text)
                if (warning) {showMsg(warning)}
                else {
                    showMsg("Succesfully added user: " + assingUserToGroupButton.text + " to group " + controller.groupname)
                }
            }
        }
    }

    Dialog{
        id: removeUserFromGroupDialog
        title: qsTr("Remove user")
        standardButtons: StandardButton.Apply | StandardButton.Cancel
        height: groupDetailedView.height/4
        width: groupDetailedView.width/4
        ComboBox{
            id: removeUserBox
            width: parent.width
            anchors{
                horizontalCenter: parent.horizontalCenter
                verticalCenter: parent.verticalCenter
           }
            model: groupDetailsModel
            currentIndex: -1
            textRole: "related_user"
            displayText: "Pick user to remove"
            delegate: ItemDelegate{
                width: removeUserBox.width
                contentItem: Text {
                    horizontalAlignment: Text.AlignHCenter
                    text: model.related_user
                    color: "black"
                }
            }
            onCurrentTextChanged: displayText = currentText
        }
        onButtonClicked: {
            if (clickedButton === StandardButton.Apply) {
                // TODO remove from group in c++
            }
        }
    }

    Component {
        id: delegateUserToBillModel
        RowLayout{
            Layout.fillWidth: true
            property string name: relatedLogin.text
            property string enteredValueStr: enteredValue.text
            property bool acceptableValue: enteredValue.acceptableInput
            property bool isUserCountedInBill: userInBillSwitch.checked
            Switch{
                id: userInBillSwitch
                checked: true
            }
            Label{
                id: relatedLogin
                text: userLogin
            }
            TextField{
                id: enteredValue
                clip: true
                readOnly: !manualyEdit.checked
                horizontalAlignment: Text.AlignHCenter
                validator: DoubleValidator{
                    bottom: 0
                    decimals: 2
                }
            }
        }
    }

    MessageDialog{
        id: msgDialog
        title: "Group " + controller.groupname + " info"
        icon: StandardIcon.Information
    }

    MessageDialog{
        id: deleteGroupDialog
        title: "Warning"
        text: "Are you sure that you want to delete group: " + controller.groupname + " permanently?"
        icon: StandardIcon.Question
        standardButtons: StandardButton.Yes | StandardButton.No
        onButtonClicked: {
            if (clickedButton === StandardButton.Yes) {
                backendController.deleteGroup()
                if (warning) {showMsg(warning)}
                else {
                    showMsg("Succesfully deleted group: " + controller.groupname)
                    controller.displayGroupDetailView(false, "")
                }
            }
        }
    }

    function showMsg(textToDisplay)
    {
        msgDialog.text = textToDisplay
        msgDialog.open()
    }

    onClosing: controller.displayGroupDetailView(false, "")
}
