UI System is modular and has 3 components : 

1.UI
2.Widget Controller (A middle man for data input/output and calculations)
3.Data

OneWay Dependency :

    UI<-Controller<-Ability System

I.e.:
UI knows about the controller, but controller does not know about UI.
Controllers know about the data, but it does not contain controller.
By doing this, the system is modular and expandable without dependencies and can easily change controllers for the widgets.