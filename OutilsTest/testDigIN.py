from time import sleep
import serial
import serial.tools.list_ports
import glob
import sys


class MyApp():
    def __init__(self):
        self.listPorts=[]
        pass

    def listSerialPort(self):
        ports = serial.tools.list_ports.comports()
        self.listPorts = []
        for port, desc, hwid in sorted(ports):
            print("{}: {} [{}]".format(port, desc, hwid))
            self.listPorts.append(port)
    
    def createBoolString(self,id,direction):
        str=""
        for i in range(192):
            if direction:
                if i<=id:
                    str+="1,"
                else:
                    str+="0,"
            else:
                if i<=id:
                    str+="0,"
                else:
                    str+="1,"
        str=str[0:len(str)-1]
        return str
        
            
    def startPGM(self):
        self.listSerialPort()
        if len(self.listPorts)>0:
            ser = serial.Serial(self.listPorts[0])  # open serial port
            print(ser.name)         # check which port was really used
            if ser.isOpen():
            
                while True:  # TODO tester avec un x pour voir si ignorer
                    for i in range(192):
                        #print(i)
                        boolState=self.createBoolString(i,True)
                        #print (boolState)
                        data="{\"cmd\":\"digOutput\",\"data\":["+boolState+"]}!"
                        #print (data)
                        ser.write(data.encode('ascii'))
                        print(ser.read_all())
                    
                        sleep(0.3)
                    
                    sleep(20)
                    for i in range(192):
                        #print(i)
                        boolState=self.createBoolString(i,False)
                        #print (boolState)
                        data="{\"cmd\":\"digOutput\",\"data\":["+boolState+"]}!"
                        #print (data)
                        ser.write(data.encode('ascii'))
                        print(ser.read_all())
                    
                        sleep(0.3)
                    
                ser.close()             # close po
    

    
    
if __name__ == "__main__":
    myApp = MyApp()
    myApp.startPGM()
