# -*- coding: utf-8 -*-
"""
Created on Fri Nov 12 15:47:13 2021

@author: baciu
"""
import image_header_app1
import serial.tools.list_ports
import ctypes
import pyqtgraph as pg

from PyQt5 import QtWidgets, uic
from process_serial_frames import SerialFrame

appSerialHandler = None
plotUpdateFlag = True
deviceDisconnected = True

class App1_Gui(QtWidgets.QMainWindow):

    def __init__(self, *args, **kwargs):
        super(App1_Gui, self).__init__(*args, **kwargs)
        
        #Load the UI Page
        uic.loadUi('app1_gui.ui', self)
        self.setFixedSize(self.size())
        self.setWindowTitle("UART PPG PLOTTING")
        self.connectButton.clicked.connect(self.serial_connect)
        self.disconnectButton.clicked.connect(self.serial_disconnect)
        self.pauseButton.clicked.connect(self.pause_plot_update)
        self.disconnectButton.hide()

        self.pauseButton.hide()
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
            plotUpdateFlag = False
            self.pauseButton.setStyleSheet('QPushButton {font: 75 18pt "Miriam Mono CLM"; \
                                            background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(235, 182, 68 , 255), stop:1 rgba(255, 255, 255, 255));}')
            self.pauseButton.setText("Resume")
        else:
            self.pauseButton.setStyleSheet('QPushButton {font: 75 18pt "Miriam Mono CLM"; \
                                             background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(83, 116, 249, 255), stop:1 rgba(255, 255, 255, 255));}')
            self.pauseButton.setText("Pause")
            plotUpdateFlag = True
        
    def serial_connect(self):
        global appSerialHandler
        global deviceDisconnected
        try:
            self.serialHandler = SerialFrame(self.comPort.currentText(), 115200)
            self.connectButton.setStyleSheet('QPushButton {font: 75 16pt "Miriam Mono CLM"; \
                                             background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(8, 196, 85, 255), stop:1 rgba(255, 255, 255, 255));}')
            self.connectButton.setText("Connected " + u'\N{check mark}')
             
            self.connectButton.setDisabled(True)
            self.disconnectButton.show()
            self.pauseButton.show()
            self.connectButton.hide()
            
            appSerialHandler = self.serialHandler
            deviceDisconnected = False
        except:
            ctypes.windll.user32.MessageBoxW(0, "Could not open {}!\nDevice not connected."
                                             .format(self.comPort.currentText()), 
                                             "Serial error", 16)
            self.connectButton.setStyleSheet('QPushButton {font: 75 10pt "Miriam Mono CLM"; \
                                             background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(216, 75, 33, 255), stop:1 rgba(255, 255, 255, 255));}')
            self.connectButton.setText("Not connected " + u'\N{cross mark}')  
           
    def serial_disconnect(self):
            global plotUpdateFlag
            global deviceDisconnected
            
            self.serialHandler.close_conection()
            self.disconnectButton.hide()
            self.pauseButton.hide()
            
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


