# -*- coding: utf-8 -*-
"""
Created on Fri Nov 12 16:16:26 2021

@author: baciu
"""
import pyqtgraph as pg
import numpy as np 
import application2

from PyQt5 import QtGui
from PyQt5.QtGui import QColor
from PyQt5.QtCore import QTime, QTimer

y_range_max1 = 120
y_range_min1 = 0

y_range_max2 = 58000
y_range_min2 = 55000


plot_size = 300
    
class  PlotWidget(pg.GraphicsWindow):
    pg.setConfigOption('background', 'w')
    pg.setConfigOption('foreground', 'k')
    

    
    def __init__(self, parent=None, **kargs):
        global y_range_max
        global plot_size
        pg.GraphicsWindow.__init__(self, **kargs)
        self.setParent(parent)
        print(parent.objectName())
        self.setWindowTitle('pyqtgraph example: Scrolling Plots')
        self.p1 = self.addPlot(0,0 )
    

        self.p1.setMouseEnabled(False, False)
        self.ax1 = self.p1.getAxis('bottom')  # This is the trick
        self.ax1_y = self.p1.getAxis('left')
        
        labelStyle = {'color': '#2A0202', 'font-size': '14pt'}
        
        self.ax1.setLabel('Samples', **labelStyle)
        self.ax1_y.setLabel('Amplitude', **labelStyle)
        self.p1.setYRange(0, y_range_max1)
        self.p1.setXRange(0, plot_size)
        self.p1.enableAutoRange(pg.ViewBox.XAxis)
        self.data1 = np.zeros(plot_size)
        self.time_axis = np.zeros(plot_size)
        self.ptr1 = 0
        self.curve1 = self.p1.plot(self.data1, pen=(3,3))
        
        
        self.p2 = self.addPlot(1,0 )
         
        
        self.p2.setMouseEnabled(False, False)
        self.ax2 = self.p2.getAxis('bottom')  # This is the trick
        self.ax2_y = self.p2.getAxis('left')
                
        self.ax2.setLabel('Samples', **labelStyle)
        self.ax2_y.setLabel('Amplitude', **labelStyle)
        self.p2.setYRange(0, y_range_max2)
        self.p2.setXRange(0, plot_size)
        self.p2.enableAutoRange(pg.ViewBox.XAxis)
        self.data2 = np.zeros(plot_size)
        self.time_axis = np.zeros(plot_size)
        self.ptr2 = 0
        self.curve2 = self.p2.plot(self.data2, pen=(3,3))
        
        
        
        
            
        self.t = QTime()
        self.t.start()
        
        timer = pg.QtCore.QTimer(self)
        timer.timeout.connect(self.update)
        timer.start(100) # number of seconds (every 1000) for next update

    def update(self):
        global y_range_max
        if application2.deviceDisconnected is True:
            
            self.data1 = np.zeros(plot_size)
            self.data2 = np.zeros(plot_size)
            self.ptr1 = 0
            self.ptr2 = 0
    
        if application2.plotUpdateFlag is True and application2.deviceDisconnected is False:
            maxValue1 = max(self.data1[plot_size-20:-1])
            
            if(maxValue1 <= y_range_max1):
                self.p1.setYRange(y_range_min1,y_range_max1)
            else:
                self.p1.setYRange(maxValue1 - y_range_max1 ,maxValue1 + y_range_max1)
            
            if(maxValue1 <= y_range_min1):
                self.p1.setYRange(0,y_range_max1)
                
    
            maxValue2 = max(self.data2[plot_size-20:-1])
            
            
            if(maxValue2 <= y_range_max2):
                self.p2.setYRange(y_range_min2,y_range_max2)
            else:
                self.p2.setYRange(maxValue2 - 400 ,maxValue2 + 400)
            
            if(maxValue2 <= y_range_min2):
                self.p2.setYRange(maxValue2 - 400 ,maxValue2 + 400)

            if((application2.appSerialHandler != None) and application2.appSerialHandler.is_open()):
                application2.appSerialHandler.readAndProcessFrame()
                application2.deviceDisconnected =  application2.appSerialHandler.deviceDisconnected
                

                 # IR CHANEL 
                if (application2.activeChannel == 0x02):
                    
                    if(application2.appSerialHandler.custom_ir_buffer_updated):
                        for i in range(0, len(application2.appSerialHandler.custom_ir_buffer)-1):
                            self.data1[:-1] = self.data1[1:]  # shift data in the array one sample left
                                  
                            self.data1[-1] = application2.appSerialHandler.custom_ir_buffer[i]
                         
                            self.ptr1 += 1
                            self.curve1.setData(y=self.data1)
                            self.curve1.setPos(self.ptr1, 0)
                            self.curve1.setPen(QColor(28, 6, 1))

                    elif(application2.appSerialHandler.max_ir_buffer_updated):
                            for i in range(0, len(application2.appSerialHandler.max_ir_buffer)-1):
                                self.data2[:-1] = self.data2[1:]  # shift data in the array one sample left
                                      
                                self.data2[-1] = application2.appSerialHandler.max_ir_buffer[i]
                                
                      
                                self.ptr2 += 1
                                self.curve2.setData(y=self.data2)
                                self.curve2.setPos(self.ptr2, 0)
                                self.curve2.setPen(QColor(28, 6, 1))
                # RED CHANEL                
                elif(application2.activeChannel == 0x00):
                    
                    if(application2.appSerialHandler.custom_red_buffer_updated):
                        for i in range(0, len(application2.appSerialHandler.custom_red_buffer)-1):
                            self.data1[:-1] = self.data1[1:]  # shift data in the array one sample left
                                  
                            self.data1[-1] = application2.appSerialHandler.custom_red_buffer[i]
                         
                            self.ptr1 += 1
                            self.curve1.setData(y=self.data1)
                            self.curve1.setPos(self.ptr1, 0)
                            self.curve1.setPen(QColor(238, 54, 15))
                            
                    elif(application2.appSerialHandler.max_red_buffer_updated):
                            for i in range(0, len(application2.appSerialHandler.max_red_buffer)-1):
                                self.data2[:-1] = self.data2[1:]  # shift data in the array one sample left
                                      
                                self.data2[-1] = application2.appSerialHandler.max_red_buffer[i]
                                
                      
                                self.ptr2 += 1
                                self.curve2.setData(y=self.data2)
                                self.curve2.setPos(self.ptr2, 0)
                                self.curve2.setPen(QColor(238, 54, 15))
                # GREEN CHANEL                
                elif(application2.activeChannel == 0x01):
                    
                    if(application2.appSerialHandler.custom_green_buffer_updated):
                        for i in range(0, len(application2.appSerialHandler.custom_green_buffer)-1):
                            self.data1[:-1] = self.data1[1:]  # shift data in the array one sample left
                                  
                            self.data1[-1] = application2.appSerialHandler.custom_green_buffer[i]
                         
                            self.ptr1 += 1
                            self.curve1.setData(y=self.data1)
                            self.curve1.setPos(self.ptr1, 0)
                            self.curve1.setPen(QColor(83, 143, 7))
                            
                    elif(application2.appSerialHandler.max_green_buffer_updated):
                            for i in range(0, len(application2.appSerialHandler.max_green_buffer)-1):
                                self.data2[:-1] = self.data2[1:]  # shift data in the array one sample left
                                      
                                self.data2[-1] = application2.appSerialHandler.max_green_buffer[i]
                                
                      
                                self.ptr2 += 1
                                self.curve2.setData(y=self.data2)
                                self.curve2.setPos(self.ptr2, 0)
                                self.curve2.setPen(QColor(83, 143, 7))
                    
                
            
                        
            
 

if __name__ == '__main__':
    w = PlotWidget()
    w.show()
    QtGui.QApplication.instance().exec_()