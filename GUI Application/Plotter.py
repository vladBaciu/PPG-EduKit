# -*- coding: utf-8 -*-
"""
Created on Fri Nov 12 16:16:26 2021

@author: baciu
"""
import pyqtgraph as pg
import numpy as np 
import application1

from PyQt5 import QtGui
from PyQt5.QtCore import QTime, QTimer

y_range_max = 180
y_range_min = 0
plot_size = 300
    
class  CustomWidget(pg.GraphicsWindow):
    pg.setConfigOption('background', 'w')
    pg.setConfigOption('foreground', 'k')
    

    
    def __init__(self, parent=None, **kargs):
        global y_range_max
        global plot_size
        pg.GraphicsWindow.__init__(self, **kargs)
        self.setParent(parent)
        print(parent.objectName())
        self.setWindowTitle('pyqtgraph example: Scrolling Plots')
        self.p1 = self.addPlot()
        self.p1.setMouseEnabled(False, False)
        self.ax = self.p1.getAxis('bottom')  # This is the trick
        self.ax_y = self.p1.getAxis('left')
        
        labelStyle = {'color': '#2A0202', 'font-size': '14pt'}
        
        self.ax.setLabel('Time (s)', **labelStyle)
        self.ax_y.setLabel('Amplitude', **labelStyle)
        self.p1.setYRange(0, y_range_max)
        self.p1.setXRange(0, plot_size)
        self.p1.enableAutoRange(pg.ViewBox.XAxis)
        self.data1 = np.zeros(plot_size)
        self.time_axis = np.zeros(plot_size)
        self.ptr1 = 0
        self.curve1 = self.p1.plot(self.data1, pen=(3,3))

        self.t = QTime()
        self.t.start()
        
        timer = pg.QtCore.QTimer(self)
        timer.timeout.connect(self.update)
        timer.start(100) # number of seconds (every 1000) for next update

    def update(self):
        global y_range_max
        if application1.deviceDisconnected is True:
            self.data1 = np.zeros(plot_size)
            self.time_axis = np.zeros(plot_size)
            
            reference = range(0, plot_size)
            reference = range(0, plot_size)
            reference = reference[0:-1:50]
            samples_sec = [str(2* (value/125)) for value in reference]
            dx = zip(reference, samples_sec)
            self.ax.setTicks([dx, []])
            self.curve1.setData(y=self.data1)
            self.ptr1 = 0
            
        if application1.plotUpdateFlag is True and application1.deviceDisconnected is False:
            
            application1.deviceDisconnected =  application1.appSerialHandler.deviceDisconnected
            
            maxValue = max(self.data1[0:-1])
            minValue = min(self.data1[0:-1])
            av = sum(self.data1)/len(self.data1)
            self.p1.setYRange(av - (maxValue - minValue), av + (maxValue - minValue))

            if((application1.appSerialHandler != None) and application1.deviceDisconnected is False):
                application1.appSerialHandler.readAndProcessFrame()
                
                if(application1.appSerialHandler.custom_ir_buffer_updated):
                    for i in range(0, len(application1.appSerialHandler.custom_ir_buffer)-1):
                        self.data1[:-1] = self.data1[1:]  # shift data in the array one sample left
                        self.time_axis[:-1] = self.time_axis[1:]  # shift data in the array one sample left
                              
                        self.data1[-1] = application1.appSerialHandler.custom_ir_buffer[i]
                        
                        reference = range(0, plot_size)
                        samples = range(self.ptr1, self.ptr1 + plot_size)
                        reference = reference[0:-1:50]
                        samples = samples[0:-1:50]
                        samples_sec = ["{:.1f}".format(2* (value/125)) for value in samples]
                        dx = zip(samples, samples_sec)
                        self.ax.setTicks([dx, []])
                        self.ptr1 += 1
                        self.curve1.setData(y=self.data1)
                        self.curve1.setPos(self.ptr1, 0)
                        
                        
    
                application1.appSerialHandler.custom_ir_buffer.clear()
                application1.appSerialHandler.custom_red_buffer.clear()
            
                        
            
 

if __name__ == '__main__':
    w = CustomWidget()
    w.show()
    QtGui.QApplication.instance().exec_()