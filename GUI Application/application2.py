# -*- coding: utf-8 -*-
"""
Created on Fri Nov 12 15:47:13 2021

@author: baciu
"""
import image_header_app2
import serial.tools.list_ports
import ctypes
import pyqtgraph as pg

from PyQt5 import QtWidgets, uic
from process_serial_frames import SerialFrame

appSerialHandler = None
plotUpdateFlag = True
deviceDisconnected = True
activeChannel = 0x02
currentValue = 0x05

class App2_Gui(QtWidgets.QMainWindow):

    def __init__(self, *args, **kwargs):
        super(App2_Gui, self).__init__(*args, **kwargs)
        
        #Load the UI Page
        uic.loadUi('app2_gui.ui', self)
        self.setFixedSize(self.size())
        self.setWindowTitle("COMPARE PPG SOLUTIONS")
        self.serialHandler = None

        self.connectButton.clicked.connect(self.serial_connect)
        self.disconnectButton.clicked.connect(self.serial_disconnect)
        self.pauseButton.clicked.connect(self.pause_plot_update)
        
        self.ir_button.setChecked(True)
        self.red_button.setChecked(False)
        self.green_button.setChecked(False)

        self.ir_button.toggled.connect(lambda:self.btnstate(self.ir_button))
        self.red_button.toggled.connect(lambda:self.btnstate(self.red_button))
        self.green_button.toggled.connect(lambda:self.btnstate(self.green_button))
        
        
        self.ir_button.hide()
        self.red_button.hide()
        self.green_button.hide()

        self.disconnectButton.hide()
        self.pauseButton.hide()
        
        self.currentSlider.hide()
        self.currentLabel.hide()
        self.currentTicks.hide()
        
        self.currentSlider.valueChanged.connect(self.setCurrent)
        
        self.timer = pg.QtCore.QTimer(self)
        self.timer.timeout.connect(self.check_port)
        self.timer.start(1000) # number of seconds (every 1000) for next update
        
    def update_buttons(self):
        global plotUpdateFlag, deviceDisconnected
        if deviceDisconnected:
            self.disconnectButton.hide()
            self.pauseButton.hide()
            self.pauseButton.setStyleSheet('QPushButton {font: 75 18pt "Miriam Mono CLM"; \
                                             background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(83, 116, 249, 255), stop:1 rgba(255, 255, 255, 255));}')
            self.pauseButton.setText("Pause")
            plotUpdateFlag = True
            self.connectButton.setStyleSheet('QPushButton {font: 75 16pt "Miriam Mono CLM"; \
                                             background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(8, 196, 85, 255), stop:1 rgba(255, 255, 255, 255));}')
            self.connectButton.setText("Connect")
            self.connectButton.setDisabled(False)
            
            self.connectButton.setDisabled(False)
            self.connectButton.show()
            
    def setCurrent(self):
        global currentValue, activeChannel
        
        currentValue = self.currentSlider.value()
        serialCommand = [0] * 3
        serialCommand[0] = 0x3D
        
        serialCommand[1] = activeChannel
        serialCommand[2] = currentValue
        self.serialHandler.sendCommand(serialCommand);
        
    def btnstate(self,b):
         global activeChannel
         serialCommand = [0] * 3
         serialCommand[0] = 0x3D
         
         serialCommand[2] = currentValue
         if b.isChecked() == True and deviceDisconnected == False:
            if(b.text().split()[0] == "IR"):
                serialCommand[1] = 0x02
                activeChannel = 0x02
                self.serialHandler.sendCommand(serialCommand);
            elif(b.text().split()[0] == "RED"):
                serialCommand[1] = 0x00
                activeChannel = 0x00
                self.serialHandler.sendCommand(serialCommand);
            elif(b.text().split()[0] == "GREEN"):
                serialCommand[1] = 0x01
                activeChannel = 0x01
                self.serialHandler.sendCommand(serialCommand);

    

            
    def check_port(self):
        ports = serial.tools.list_ports.comports()
        portList = []
        
        for port, desc, hwid in sorted(ports):
            find = False
            for i in range(self.comPort.count()):
                if(self.comPort.itemText(i) == port):
                    find = True
                    break
                
            if (find == False):
                portList.append(port)
                
        self.comPort.addItems(portList) 
        
        for i in range(self.comPort.count()):
            stillConnected = False
            for port, desc, hwid in sorted(ports):
                if(self.comPort.itemText(i) == port):
                    stillConnected = True
                   
            if not stillConnected or len(ports) == 0:
                self.comPort.removeItem(i)
                
        self.update_buttons()             
           
       
        
    def pause_plot_update(self):
        global plotUpdateFlag
        if plotUpdateFlag is True:
            self.serialHandler.flush()
            plotUpdateFlag = False
            self.pauseButton.setStyleSheet('QPushButton {font: 75 18pt "Miriam Mono CLM"; \
                                            background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(235, 182, 68 , 255), stop:1 rgba(255, 255, 255, 255));}')
            self.pauseButton.setText("Resume")
            self.currentSlider.hide()
            self.currentLabel.hide()
            self.currentTicks.hide()
            self.ir_button.hide()
            self.red_button.hide()
            self.green_button.hide()
            self.wavelengthLabel.hide()

        else:
            self.pauseButton.setStyleSheet('QPushButton {font: 75 18pt "Miriam Mono CLM"; \
                                             background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(83, 116, 249, 255), stop:1 rgba(255, 255, 255, 255));}')
            self.pauseButton.setText("Pause")
            plotUpdateFlag = True
            #self.currentSlider.show()
            #self.currentLabel.show()
            #self.currentTicks.show()
            self.ir_button.show()
            self.red_button.show()
            self.green_button.show()
            self.wavelengthLabel.show()

    def serial_connect(self):
        global appSerialHandler
        global deviceDisconnected
        
        self.serialHandler = SerialFrame(self.comPort.currentText(), 115200)
        self.connectButton.setStyleSheet('QPushButton {font: 75 16pt "Miriam Mono CLM"; \
                                         background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(8, 196, 85, 255), stop:1 rgba(255, 255, 255, 255));}')
        self.connectButton.setText("Connected " + u'\N{check mark}')
         
        self.connectButton.setDisabled(True)
        self.ir_button.show()
        self.red_button.show()
        self.green_button.show()
        self.disconnectButton.show()
        #self.pauseButton.show()
        #self.currentSlider.show()
        #self.currentLabel.show()
        #self.currentTicks.show()
        self.connectButton.hide()
        appSerialHandler = self.serialHandler
        deviceDisconnected = False
        
        serialCommand = [0] * 3
        serialCommand[0] = 0x3D
        serialCommand[1] = activeChannel
        serialCommand[2] = currentValue
        self.serialHandler.sendCommand(serialCommand);
            

           
    def serial_disconnect(self):
            global plotUpdateFlag
            global deviceDisconnected
            
            self.serialHandler.close_conection()
            
            self.ir_button.hide()
            self.red_button.hide()
            self.green_button.hide()
            
            self.disconnectButton.hide()
            self.pauseButton.hide()
            
            self.currentSlider.hide()
            self.currentLabel.hide()
            self.currentTicks.hide()
            self.wavelengthLabel.hide()
            self.connectButton.setDisabled(False)
            self.connectButton.show()
            
            self.connectButton.setStyleSheet('QPushButton {font: 75 16pt "Miriam Mono CLM"; \
                                             background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(8, 196, 85, 255), stop:1 rgba(255, 255, 255, 255));}')
            self.connectButton.setText("Connect")
            self.connectButton.setDisabled(False)
            self.pauseButton.setStyleSheet('QPushButton {font: 75 18pt "Miriam Mono CLM"; \
                                            background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(83, 116, 249, 255), stop:1 rgba(255, 255, 255, 255));}')
            self.pauseButton.setText("Pause")
            plotUpdateFlag = True
            deviceDisconnected = True


