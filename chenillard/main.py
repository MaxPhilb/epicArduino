from asyncio.windows_events import NULL
import serial
import glob
import sys
import time
from PySide6.QtUiTools import QUiLoader
from PySide6.QtWidgets import *
from PySide6.QtCore import QFile, QIODevice, QObject, QThread
from PySide6.QtGui import *
import serial.tools.list_ports
from threading import Thread


class MyApp():
    def __init__(self):
      
        self.window = -1
        self.gridBool = QGridLayout()
        self.serial=NULL
        pass

    def connectSerial(self):
        if not self.serial:
            nomPort=self.window.comboSerial.currentText()
            self.serial=serial.Serial(nomPort,115200)
            print("serial")
            print(nomPort)
        if self.serial:
            self.window.startChen.setEnabled(True)
            self.window.etatSerial.setText("Port série connecté")
            self.window.btnSerial.setText("Deconnecter")


    def listSerialPort(self):
        ports = serial.tools.list_ports.comports()
        listPorts = []
        for port, desc, hwid in sorted(ports):
            print("{}: {} [{}]".format(port, desc, hwid))
            listPorts.append(port)
        return listPorts

    def createBoolString(self,id,state):
        str="";
        for i in range(32):
            if state:
                if i==id:
                    str+="1,";
                else:
                    str+="0,";
            else:
                str+="0,";
        str=str[0:len(str)-1];
        return str

    
    def pgmChen(self):
        for i in range(32):
            
            print(i)
            boolState=self.createBoolString(i,True)
            #print (boolState)
            data="{\"cmd\":\"digOutput\",\"data\":["+boolState+"]}!"
            print (data)
            self.serial.write(data.encode('ascii'))
           
            print(self.serial.read_all())
                    
            time.sleep(0.5)

            boolState=self.createBoolString(i,False)
            #print (boolState)
            data="{\"cmd\":\"digOutput\",\"data\":["+boolState+"]}!"
            print (data)
            self.serial.write(data.encode('ascii'))
            print(self.serial.read_all())
                    
            time.sleep(0.5)


    def startChen(self):
        new_thread=Thread(target=self.pgmChen)
        new_thread.start()
 
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
            if self.serial:
                self.serial.close()
            sys.exit(-1)
        self.window.show()
        if self.serial:
                self.serial.close()
        sys.exit(app.exec())
        


if __name__ == "__main__":
    myApp = MyApp()
    myApp.startWindow()
