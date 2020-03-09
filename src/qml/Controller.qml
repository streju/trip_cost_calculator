import QtQuick 2.0

Item {
    id: controller
    property string username : stateProvider.username
    property string groupname : stateProvider.groupname

    signal displayNewGroupCreation(bool state)
    signal displayMainAppWindow(bool state, string user)
    signal displayRegistrationWindow(bool state)
    signal displayGroupDetailView(bool state, string group)

    onDisplayNewGroupCreation: createNewGroupLoader.active = state
    onDisplayMainAppWindow: {
        stateProvider.username = user
        mainWindowLoader.active = state
    }
    onDisplayRegistrationWindow: registrationPageLoader.active = state
    onDisplayGroupDetailView: {
        stateProvider.groupname = group
        groupViewLoader.active = state
    }

    Loader{
        id: mainWindowLoader
        property var win;
        active: false
        onActiveChanged:{
            if (active == true){
                var component = Qt.createComponent("mainAppWindow.qml");
                win = component.createObject(applicationWindow);
                applicationWindow.hide();
                registrationPageLoader.active = false
                win.show();
            }
            else{
                win.hide()
                applicationWindow.show()
            }
        }
    }

    Loader{
        id: createNewGroupLoader
        property var win;
        active: false
        onActiveChanged:{
            if (active == true){
                var component = Qt.createComponent("NewGroupCreation.qml");
                win = component.createObject(controller);
                mainWindowLoader.win.hide()
                win.show();
            }
            else{
                win.hide()
                mainWindowLoader.win.show()
            }
        }
    }

    Loader{
        id: registrationPageLoader
        property var win;
        active: false
        onActiveChanged:{
            if (active == true){
                var component = Qt.createComponent("registration.qml");
                win = component.createObject(applicationWindow);
                applicationWindow.hide();
                win.show();
            }
            else{
                win.hide()
                if (mainWindowLoader.active === false){
                    applicationWindow.show()
                }
            }
        }
    }

    Loader{
        id: groupViewLoader
        property var win;
        active: false
        onActiveChanged:{
            if (active == true){
                var component = Qt.createComponent("GroupDetailedView.qml");
                win = component.createObject(controller);
                win.show();
                mainWindowLoader.win.hide()
            }
            else{
                win.hide()
                mainWindowLoader.win.show()
            }
        }
    }
}
