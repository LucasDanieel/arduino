import keyboard
import threading
import os
import time

import firebase_admin
from firebase_admin import credentials, firestore, db

import serial
import serial.tools.list_ports
import json
import datetime

class serialApp ():
    def __init__(self):
        self.serialPort = serial.Serial()

    def connectSerial(self):
        try:
            self.serialPort.open()
        except:
            print("Houve um erro ao abrir a serial")
            os._exit(0)

    def readSerial(self):
        dataRead = self.serialPort.readline().decode("utf-8").replace("'", '"')
        dataRead = dataRead.replace("\r\n", "")
        if dataRead[0] == "{" and dataRead[len(dataRead)-1] == "}":
            newJson = json.loads(dataRead)
            newJson["datetime"] = str(datetime.datetime.now())
            return newJson
        
        return False

    def sendSerial(self, s):
        if(self.serialPort.is_open):
            dataSend = str(s)
            self.serialPort.write(dataSend.encode())
            self.serialPort.reset_output_buffer()

    def closeSerial(self):
        self.serialPort.close()

class firebase ():
    def __init__(self):
        cred = credentials.Certificate("arduino-8cd69-firebase-adminsdk-j19t5-cdbfb66f93.json")
        firebase_admin.initialize_app(cred)
        self.db = firestore.client()

    def mandar_dados(self, dados):
        doc_ref = self.db.collection(u'plantacao').document()
        doc_ref.set(dados)


ser = serialApp()

ser.serialPort.port = "COM3"
ser.serialPort.baudrate = 9600
ser.connectSerial()

fb = firebase()

def serial_arduino_quit():
    while True:
        time.sleep(0.01)
        if keyboard.is_pressed('q'):
            os._exit(0)

threading.Thread(target=serial_arduino_quit).start()

while(True):
    dados = ser.readSerial()
    if dados:
        print(dados)
        fb.mandar_dados(dados)