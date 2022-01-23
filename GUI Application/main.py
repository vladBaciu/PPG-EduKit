# -*- coding: utf-8 -*-
"""
Created on Thu Nov 11 17:47:46 2021

@author: vlad-eusebiu.baciu@vub.be
"""

import pyqtgraph as pg
import sys
import logo_qrc
import ctypes

from PyQt5 import QtWidgets, uic
from pyqtgraph import PlotWidget
from application1 import App1_Gui
from application2 import App2_Gui


class MainWindow(QtWidgets.QMainWindow):

    def __init__(self, *args, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)
        
        #Load the UI Page
        uic.loadUi('app_entry.ui', self)
        self.setWindowTitle("PPG EduKit Demos")
        self.app1_button.clicked.connect(self.open_app1)
        self.app2_button.clicked.connect(self.open_app2)
        self.show_pdf_list.triggered.connect(self.open_app_pdf)
        self.show_info_box.triggered.connect(self.show_info)
        
    def open_app_pdf(self):
        print("open_app_pdf")
        
    def show_info(self):
        print("show_info")
        ctypes.windll.user32.MessageBoxW(1, "Bla bla bla text text text", "About PPG EduKit platform", 64)

    def open_app1(self):
        self.main = App1_Gui()
        self.main.show()
        self.close()
    
    def open_app2(self):
        self.main = App2_Gui()
        self.main.show()
        self.close()
        
def main():
    app = QtWidgets.QApplication(sys.argv)
    main = MainWindow()
    main.show()
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()