import serial
import glob
import sys
import pygame
from PySide6.QtUiTools import QUiLoader
from PySide6.QtWidgets import *
from PySide6.QtCore import QFile, QIODevice
from PySide6.QtGui import *
import serial.tools.list_ports


class MyApp():
    def __init__(self):
        self.selectedJoystick = -1
        self.listCheckBox = []
        self.listLabel = []
        self.window = -1
        self.gridBool = QGridLayout()
        self.gridAna = QGridLayout()
        self.numberButton = 32
        self.numberAna = 8
        pass

    def connectSerial(self):
        print("serial")

    def listJoystick(self):
        pygame.joystick.init()
        joysticks = []
        for x in range(pygame.joystick.get_count()):
            joysticks.append(pygame.joystick.Joystick(x))
        return joysticks

    def listSerialPort(self):
        ports = serial.tools.list_ports.comports()
        listPorts = []
        for port, desc, hwid in sorted(ports):
            print("{}: {} [{}]".format(port, desc, hwid))
            listPorts.append(port)
        return listPorts

    def connectJoystick(self):
        print("joysticl")
        idJoystick = self.window.comboJoystick.itemData(
            self.window.comboJoystick.currentIndex())
        print(idJoystick)
        self.selectedJoystick = pygame.joystick.Joystick(idJoystick)
        self.selectedJoystick.init()
        self.checkButton()

    def checkButton(self):
        print("nb buttons " + str(self.selectedJoystick.get_numbuttons()))
        for index in range(self.numberButton):
            state = self.selectedJoystick.get_button(index)
            ck = self.listCheckBox[index]
            ck.setChecked(state)
            print("state index "+str(index) + ":"+str(state))

    def startWindow(self):

        app = QApplication(sys.argv)

        ui_file_name = "mainWindow.ui"
        ui_file = QFile(ui_file_name)
        if not ui_file.open(QIODevice.ReadOnly):
            print(f"Cannot open {ui_file_name}: {ui_file.errorString()}")
            sys.exit(-1)
        loader = QUiLoader()
        self.window = loader.load(ui_file)

        joysticks = self.listJoystick()
        for joystick in joysticks:
            print("joystick ")
            print(str(joystick.get_instance_id()) + "_" +
                  joystick.get_name()+"_" + str(joystick.get_guid()))
            self.window.comboJoystick.addItem(str(joystick.get_instance_id()) + "_" +
                                              joystick.get_name(), joystick.get_instance_id())
        listSerial = self.listSerialPort()
        for port in listSerial:
            self.window.comboSerial.addItem(port)
        self.window.btnSerial.clicked.connect(self.connectSerial)
        self.window.btnJoystick.clicked.connect(self.connectJoystick)

        for index in range(self.numberButton):
            ck = QCheckBox(str(index))
            self.listCheckBox.append(ck)
            self.gridBool.addWidget(ck, index-(index % 4), index % 4)

        self.window.contenuGrid.addLayout(self.gridBool)

        for index in range(self.numberAna):
            lab = QLabel(str(index))
            self.listLabel.append(lab)
            self.gridAna.addWidget(lab, index-(index % 2), index % 2)

        self.window.contenuAna.addLayout(self.gridAna)

        ui_file.close()
        if not self.window:
            print(loader.errorString())
            sys.exit(-1)
        self.window.show()

        sys.exit(app.exec_())


if __name__ == "__main__":
    myApp = MyApp()
    myApp.startWindow()
