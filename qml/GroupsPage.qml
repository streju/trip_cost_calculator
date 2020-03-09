import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.2
import QtQuick.Controls.Material 2.2

Page {
    id: groupPage
    Layout.fillHeight: true
    Layout.fillWidth: true
    background: Rectangle {color: "black"}
    ScrollView{
        width: parent.width
        height: parent.height
        clip: true
        ListView{
            anchors.horizontalCenter: parent.horizontalCenter
            model: groupModel
            delegate: Rectangle{
                id: groupInfo
                anchors.horizontalCenter: parent.horizontalCenter
                color: "transparent"
                Rectangle{
                    height: 1
                    width: parent.width
                    anchors.bottom: parent.bottom
                    color: "darkorange"
                }
                width: parent.width
                height: 100
                MouseArea{
                    anchors.fill: groupInfo
                    onClicked: controller.displayGroupDetailView(true, groupName.text)
                }
                Text {
                    id: groupName
                    anchors{
                        horizontalCenter: parent.horizontalCenter
                        verticalCenter: parent.verticalCenter
                    }
                    font.pixelSize: 20
                    text: model.group_name
                    color: "darkorange"
                }
                Text {
                    anchors{
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }
                    font.pixelSize: 20
                    text: "BALANCE: " + model.value
                    color: model.value >= 0 ? "green" : "red"
                    rightPadding: 10
                }
            }
        }
    }

    footer:RowLayout{
        Rectangle{
            width: parent.width/2
        }
        Button{
            id: addNewGroupButton
            icon.source: "qrc:/media/media/icons/add-24px.svg"
            text: qsTr("Create new group")
            checked: true
            display: AbstractButton.TextBesideIcon
            Material.background: Material.Red
            width: parent.width/2
            Layout.alignment: Qt.AlignRight
            onClicked: displayNewGroupCreation(true)
        }
    }
}
