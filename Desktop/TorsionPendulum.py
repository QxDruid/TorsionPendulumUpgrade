#! /usr/bin/env python3
# -*- coding: utf-8 -*-

from desktop import Ui_MainWindow
from PyQt5 import QtWidgets, QtGui, QtCore
from PyQt5.QtWidgets import QAction
from PyQt5.QtCore import Qt, pyqtSignal
from PyQt5.QtSerialPort import QSerialPort, QSerialPortInfo
import sys
import numpy as np
import scipy as scipy
#from scipy.signal import savgol_filter
import pyqtgraph as pg



class MainWindow(QtWidgets.QMainWindow):

    serial = QSerialPort()
    # timer = QtCore.QElapsedTimer() // FPS TEST

    # new signals
    data_isReady_signal = pyqtSignal()
    get_data_signal = pyqtSignal()

    # data variables
    disp = 0.02741
    x = np.arange(0, 3648*disp, disp)
    y = [] 
    data_y = np.zeros_like(x)
    average_count = 0

    # flags
    data_flag = False
    start_flag = False
    begin = 0

    def __init__(self):
        super(MainWindow, self).__init__()
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.serial = QSerialPort(self)
        self.timer = QtCore.QElapsedTimer()
          
        self.graphInit()
        self.getAvaliablePorts()

        self.ui.pushButton_Get.clicked.connect(self.getOnce_slot)
        self.ui.pushButton_Start.clicked.connect(self.start_slot)
        self.ui.pushButton_Stop.clicked.connect(self.stop_slot)


        self.serial.readyRead.connect(self.serialRead_slot)
        self.data_isReady_signal.connect(self.plotUpdate)
        self.get_data_signal.connect(self.getOnce_slot)

        average = []
        for i in range(1, 10):
            average.append(str(i))
        for average_ in average:
            entry = QAction(average_, self)
            self.ui.menuAverage.addAction(entry)
            entry.triggered.connect(lambda Val, average_to_set=average_: self.average_set(average_to_set))

    def position(self):
        pos = self.y.index(max(self.y))
        self.ui.lcd_pos.display(round(self.x[pos],2))    

    def plotUpdate(self):
        # Филтрация Savitzky-Golay (не уверен что в этом есть смысл)
        #tmp = savgol_filter(self.y, 5, 3)
        #self.ui.graphicsView.plot(self.x, tmp, pen='k', symbol=None, symbolPen='k', clear=True)

        if ui.radioButton_interpolate.isChecked():
            f =  scipy.interp1d(x, y)
            self.y = f(self.x)
        # если включена интерполяция то делаем интерполяцию данных
        if aver > 1:
            data_y = data_y + y
            aver = aver - 1
        else:
            data_y = data_y + y


            try:
                self.ui.graphicsView.plot(self.x, self.y, pen='k', symbol=None, symbolPen='k', clear=True)
                self.position()
                aver = average_count
                data_y = np.zeros_like(self.x)
            except:
                pass
            #plot = self.ui.graphicsView.plot(self.x, tmp, pen='g', symbol=None, symbolPen='k', clear=True)
        
            #print(round(1/(self.timer.elapsed()/1000),1))  // FPS TEST
            if self.start_flag == True:
                self.get_data_signal.emit()
                #self.timer.start() // FPS TEST 

    def graphInit(self):
        self.ui.graphicsView.setBackground('w') # backgroung color
        self.ui.graphicsView.visibleRange()
        # X axis
        self.ui.graphicsView.setLabel('bottom', 'Position', color='k', **{'font-size':'12pt'}) # label
        self.ui.graphicsView.getAxis('bottom').setPen(pg.mkPen(color='k', width = 1)) # axis style
        self.ui.graphicsView.setXRange(self.disp, 3648*self.disp) # axis size

        # Y axis
        self.ui.graphicsView.setLabel('left', 'Intensity(a.e.)', color='k', **{'font-size':'12pt'})
        self.ui.graphicsView.getAxis('left').setPen(pg.mkPen(color='k', width = 1))
        self.ui.graphicsView.setYRange(100, 2400)

    def getAvaliablePorts(self):
        ports = QSerialPortInfo.availablePorts()
        portName = []
        for i in ports:
            portName.append(i.portName())

        for port in portName:
            entry = QAction(port, self)
            self.ui.menuPort_3.addAction(entry)
            entry.triggered.connect(lambda Val, portItem=port: self.setPort(portItem))
            entry.triggered.connect(self.slotConnect)
            entry.setText(port)


    def setPort(self, portItem):
        self.serial.setPortName(portItem)

    def slotConnect(self):
        self.serial.setBaudRate(QSerialPort.Baud115200) 
        self.serial.close()
        self.serial.setDataBits(QSerialPort.Data8)
        self.serial.setParity(QSerialPort.NoParity)
        self.serial.setStopBits(QSerialPort.OneStop)
        self.serial.setFlowControl(QSerialPort.NoFlowControl)
        if self.serial.open(QSerialPort.ReadWrite):
            self.ui.statusbar.showMessage("{} connected".format(self.serial.portName()))
        else:
             self.ui.statusbar.showMessage("Cannot connect to device on port {}".format(self.serial.portName()))

    def average_set(self, average_):
        self.average_count = int(average_)
        self.ui.statusbar.showMessage("Average count: {}".format(self.average_count))

    def serialRead_slot(self):
        while self.serial.canReadLine():
            tmpString = self.serial.readLine()
            if tmpString == b'end\n':
                self.data_flag = 0
                self.data_isReady_signal.emit()
            if self.data_flag == 1:
                try:
                    self.y.append(4096 - int(str(tmpString.data(), encoding='utf-8').replace("\n","")))
                except:
                    self.y.append(0)
                
            if tmpString == b'begin\n':
                self.data_flag = 1
                self.y = []
                self.serial.write('n'.encode())
        
    def getOnce_slot(self):
        self.serial.write('g'.encode())

    def start_slot(self):
        self.start_flag = True
        self.serial.write('g'.encode())

    def stop_slot(self):
        self.start_flag = False

app = QtWidgets.QApplication([])
application = MainWindow()
application.show()

sys.exit(app.exec_())