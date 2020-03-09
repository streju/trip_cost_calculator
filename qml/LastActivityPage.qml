import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.2
import QtQuick.Controls.Material 2.2

Page {
    id: lastActivityPage
    width: screen.width
    height: screen.height
    background: Rectangle{color: "black"}
    ScrollView{
        width: parent.width
        height: parent.height
        clip: true
        ListView{
            height: parent.height
            model: activitiesModel
            spacing: 5
            bottomMargin: 10
            delegate: Rectangle{
                anchors.horizontalCenter: parent.horizontalCenter
                width: screen.width
                radius: 25
                height: 100
                gradient: Gradient{
                    GradientStop { position: 0.0; color: "lightblue" }
                    GradientStop { position: 1.0; color: "steelblue" }
                }
                Text {
                    anchors{
                        horizontalCenter: parent.horizontalCenter
                        verticalCenter: parent.verticalCenter
                    }
                    text: "Data: " + model.data + " Description: " + model.description
                    font.pointSize: 10
                    minimumPointSize:5
                    fontSizeMode: Text.Fit
                }
            }
        }
    }
}
