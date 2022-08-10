import serial
import glob
import sys
from PySide6.QtUiTools import QUiLoader
from PySide6.QtWidgets import *
from PySide6.QtCore import QFile, QIODevice
from PySide6.QtGui import *
import serial.tools.list_ports


class MyApp():
    def __init__(self):
      
        self.window = -1
        self.gridBool = QGridLayout()
        pass

    def connectSerial(self):
        print("serial")


    def listSerialPort(self):
        ports = serial.tools.list_ports.comports()
        listPorts = []
        for port, desc, hwid in sorted(ports):
            print("{}: {} [{}]".format(port, desc, hwid))
            listPorts.append(port)
        return listPorts

    def startChen(self):
        print(" lancer routine");
 
    def startWindow(self):

        app = QApplication(sys.argv)

        ui_file_name = "mainWindow.ui"
        ui_file = QFile(ui_file_name)
        if not ui_file.open(QIODevice.ReadOnly):
            print(f"Cannot open {ui_file_name}: {ui_file.errorString()}")
            sys.exit(-1)
        loader = QUiLoader()
        self.window = loader.load(ui_file)

        listSerial = self.listSerialPort()
        for port in listSerial:
            self.window.comboSerial.addItem(port)
        self.window.btnSerial.clicked.connect(self.connectSerial)
      

        self.window.startChen.clicked.connect(self.startChen)

    
        ui_file.close()
        if not self.window:
            print(loader.errorString())
            sys.exit(-1)
        self.window.show()

        sys.exit(app.exec_())


if __name__ == "__main__":
    myApp = MyApp()
    myApp.startWindow()
