import serial
import glob
import sys
import pygame
from PySide6.QtUiTools import QUiLoader
from PySide6.QtWidgets import QApplication
from PySide6.QtCore import QFile, QIODevice
import serial.tools.list_ports


def connectSerial():
    print("serial")


def listJoystick():
    pygame.joystick.init()
    joysticks = []
    for x in range(pygame.joystick.get_count()):
        joysticks.append(pygame.joystick.Joystick(x))
    return joysticks


def listSerialPort():
    ports = serial.tools.list_ports.comports()
    listPorts = []
    for port, desc, hwid in sorted(ports):
        print("{}: {} [{}]".format(port, desc, hwid))
        listPorts.append(port)
    return listPorts


def connectJoystick():
    print("joysticl")


if __name__ == "__main__":
    app = QApplication(sys.argv)

    ui_file_name = "mainWindow.ui"
    ui_file = QFile(ui_file_name)
    if not ui_file.open(QIODevice.ReadOnly):
        print(f"Cannot open {ui_file_name}: {ui_file.errorString()}")
        sys.exit(-1)
    loader = QUiLoader()
    window = loader.load(ui_file)

    joysticks = listJoystick()
    for joystick in joysticks:
        print("joystick ")
        print(str(joystick.get_instance_id()) + "_" +
              joystick.get_name()+"_" + str(joystick.get_guid()))
        window.comboJoystick.addItem(str(joystick.get_instance_id()) + "_" +
                                     joystick.get_name())
    listSerial = listSerialPort()
    for port in listSerial:
        window.comboSerial.addItem(port)
    window.btnSerial.clicked.connect(connectSerial)
    window.btnJoystick.clicked.connect(connectJoystick)

    ui_file.close()
    if not window:
        print(loader.errorString())
        sys.exit(-1)
    window.show()

    sys.exit(app.exec_())
