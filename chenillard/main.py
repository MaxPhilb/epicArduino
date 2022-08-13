from asyncio.windows_events import NULL
from operator import index
import serial
import glob
import sys
import time
from PySide6.QtUiTools import QUiLoader
from PySide6.QtWidgets import *
from PySide6.QtCore import QFile, QIODevice, QObject, QThread, Signal
from PySide6.QtGui import *
import traceback
import serial.tools.list_ports
from threading import Thread

class MySignal(QObject):
        sig = Signal(str)

class chenThread(QThread):

    avancement = Signal(object)

    def __init__(self,serial):
        QThread.__init__(self)
        self.serial=serial
        self.signal =MySignal()

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
            
            #print(i)
            boolState=self.createBoolString(i,True)
            #print (boolState)
            data="{\"cmd\":\"digOutput\",\"data\":["+boolState+"]}!"
            print (data)
            self.serial.write(data.encode('ascii'))
            self.signal.sig.emit(data)
            #print(self.serial.read_all())
                    
            time.sleep(0.5)

            boolState=self.createBoolString(i,False)
            #print (boolState)
            data="{\"cmd\":\"digOutput\",\"data\":["+boolState+"]}!"
            print (data)
            self.serial.write(data.encode('ascii'))
            #print(self.serial.read_all())
            self.signal.sig.emit(data)
            time.sleep(0.5)
        


    def run(self):
       self.pgmChen()




    

class MyApp():
    def __init__(self):
      
        self.window = -1
        self.gridBool = QGridLayout()
        self.serial=NULL
        self.listBtn=[]
        self.buttons=[]
        self.threadChen=NULL
        pass

    def connectSerial(self,checked):
        if checked:
            if not self.serial:
                nomPort=self.window.comboSerial.currentText()
                self.serial=serial.Serial(nomPort,115200)
                #print("serial")
                #print(nomPort)
            if self.serial:
                self.window.startChen.setEnabled(True)
                self.window.etatSerial.setText("Port série connecté")
                self.window.btnSerial.setText("Deconnecter")
                for btn in self.buttons.buttons():
                    btn.setEnabled(True)
            else:
                self.window.etatSerial.setText("Connexion impossible")

        else:
            self.serial.close()
            self.window.etatSerial.setText("Port série deconnecté")
            self.window.btnSerial.setText("Connecter")
            self.serial=NULL
            self.window.startChen.setEnabled(False)
            for btn in self.buttons.buttons():
                    btn.setEnabled(False)


    def listSerialPort(self):
        ports = serial.tools.list_ports.comports()
        listPorts = []
        for port, desc, hwid in sorted(ports):
            #print("{}: {} [{}]".format(port, desc, hwid))
            listPorts.append(port)
        return listPorts

    

    def createBoolStringExclusive(self,id,state):
        str="";
        for i in range(32):
            if i==id:
                if state:
                    str+="1,";
                else:
                    str+="0,";
            else:
                str+="x,";
        str=str[0:len(str)-1];
        return str
    
    def started(self):
        #print('Continuous batch started')
        self.window.btnSerial.setEnabled(False)
        self.window.startChen.setEnabled(False)
        for btn in self.buttons.buttons():
                btn.setEnabled(False)

    def finished(self):
        #print('Continuous batch stopped')
        self.window.btnSerial.setEnabled(True)
        self.window.startChen.setEnabled(True)
        for btn in self.buttons.buttons():
                btn.setEnabled(True)



    def startChen(self):

        self.threadChen = chenThread(self.serial)
        #print("init ok")
        self.threadChen.started.connect(self.started)
        self.threadChen.finished.connect(self.finished)
        #self.threadChen.terminated.connect(self.terminated)
        self.threadChen.signal.sig.connect(self.on_data_ready)
        if self.serial:
            self.threadChen.start()
       
    
   
        

    def on_data_ready(self, data):
        print (data)
        self.window.retourCmd.insertPlainText(data+"\n")
        self.window.retourCmd.moveCursor(QTextCursor.End)
    

    def clickBtnGrid(self,index,checked):
        boolState=self.createBoolStringExclusive(index-1,checked)
        
        data="{\"cmd\":\"digOutput\",\"data\":["+boolState+"]}!"
        print (data)
        self.serial.write(data.encode('ascii'))
           
        #print(self.serial.read_all())
        self.window.retourCmd.insertPlainText(data+"\n")

           
 
    def startWindow(self):

        app = QApplication(sys.argv)

        ui_file_name = "mainWindow.ui"
        ui_file = QFile(ui_file_name)
        if not ui_file.open(QIODevice.ReadOnly):
            #print(f"Cannot open {ui_file_name}: {ui_file.errorString()}")
            sys.exit(-1)
        loader = QUiLoader()
        self.window = loader.load(ui_file)

        listSerial = self.listSerialPort()
        for port in listSerial:
            self.window.comboSerial.addItem(port)

        self.window.btnSerial.setCheckable(True)
        self.window.btnSerial.toggled.connect(self.connectSerial)
      
        self.buttons = QButtonGroup(self.window)
        self.buttons.setExclusive(False)
        for row in range(4):
            for column in range(8):
                index=(row*8)+(column+1)
                btn=QPushButton(str(index), self.window)
                btn.setEnabled(False)
                btn.setCheckable(True)
                self.buttons.addButton(btn,index)
                self.window.gridBtn.addWidget(btn,row,column)
        
        self.buttons.idToggled.connect(self.clickBtnGrid)

       
        self.window.startChen.clicked.connect(self.startChen)

    
        ui_file.close()
        if not self.window:
            #print(loader.errorString())
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

