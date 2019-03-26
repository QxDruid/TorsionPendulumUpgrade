from desktop import Ui_MainWindow
from PyQt5 import QtWidgets, QtGui, QtCore
from PyQt5.QtWidgets import QAction
from PyQt5.QtCore import Qt, pyqtSignal
from PyQt5.QtSerialPort import QSerialPort, QSerialPortInfo
import sys
import numpy as np
import pyqtgraph as pg



class MainWindow(QtWidgets.QMainWindow):

    serial = QSerialPort()
    
    # new signals
    data_isReady_signal = pyqtSignal()


    # data variables
    x = np.arange(0, 3648, 1)
    y = []

    # flags
    data_flag = False
    begin = 0
    
    def __init__(self):
        super(MainWindow, self).__init__()
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.serial = QSerialPort(self)

        self.serial.setBaudRate(QSerialPort.Baud9600) # По умолчанию для отладки
        self.serial.setPortName("COM6")
        self.graphInit()
        self.getBaudRate()
        self.getAvaliablePorts()


        self.ui.pushButton_Get.clicked.connect(self.getOnce_slot)
        self.ui.pushButton_Start.clicked.connect(self.start_slot)
        self.ui.pushButton_Stop.clicked.connect(self.stop_slot)

        self.serial.readyRead.connect(self.serialRead_slot)
        self.data_isReady_signal.connect(self.plotUpdate)

    def plotUpdate(self):
        #print(self.y)
        self.ui.graphicsView.plot(self.y, pen='k', symbol='o', symbolPen='k', symbolSize=0, clear=True)

    def graphInit(self):
        self.ui.graphicsView.setBackground('w') # backgroung color

        # X axis
        self.ui.graphicsView.setLabel('bottom', 'Position', color='k', **{'font-size':'12pt'}) # label
        self.ui.graphicsView.getAxis('bottom').setPen(pg.mkPen(color='k', width = 1)) # axis style
        self.ui.graphicsView.setXRange(0, 520) # axis size

        # Y axis
        self.ui.graphicsView.setLabel('left', 'Intensity', color='k', **{'font-size':'12pt'})
        self.ui.graphicsView.getAxis('left').setPen(pg.mkPen(color='k', width = 1))
        self.ui.graphicsView.setYRange( 0, 260)

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

    def getBaudRate(self):
        for baudRate in ["2400", "4800", "9600", "19200", "57600", "115200"]:
            entry = QAction(baudRate, self)
            self.ui.menuBaudRate.addAction(entry)
            entry.triggered.connect(lambda bVal, menuItem=baudRate: self.setBaudRate(menuItem))
            entry.setText(baudRate)

    def setPort(self, portItem):
        self.serial.setPortName(portItem)

    def setBaudRate(self, menuItem):
        if menuItem == "2400":
            self.serial.setBaudRate(QSerialPort.Baud2400)
        elif menuItem == "4800":
            self.serial.setBaudRate(QSerialPort.Baud4800)
        elif menuItem == "9600":
            self.serial.setBaudRate(QSerialPort.Baud9600)
        elif menuItem == "19200":
            self.serial.setBaudRate(QSerialPort.Baud19200)
        elif menuItem == "57600":
            self.serial.setBaudRate(QSerialPort.Baud57600)
        elif menuItem == "115200":
            self.serial.setBaudRate(QSerialPort.Baud115200) 

        self.ui.statusbar.showMessage("BaudRate is: {}".format(menuItem))

    def slotConnect(self):
        self.serial.close()
        self.serial.setDataBits(QSerialPort.Data8)
        self.serial.setParity(QSerialPort.NoParity)
        self.serial.setStopBits(QSerialPort.OneStop)
        self.serial.setFlowControl(QSerialPort.NoFlowControl)
        if self.serial.open(QSerialPort.ReadWrite):
            self.ui.statusbar.showMessage("{} connected".format(self.serial.portName()))
        else:
             self.ui.statusbar.showMessage("Cannot connect to device on port {}".format(self.serial.portName()))

    def serialRead_slot(self):
        while self.serial.canReadLine():
            tmpString = self.serial.readLine()
            if tmpString == b'end\n':
                self.data_flag = 0
                self.data_isReady_signal.emit()
            if self.data_flag == 1:
                self.y.append(int(str(tmpString.data(), encoding='utf-8').replace("\n","")))
            if tmpString == b'begin\n':
                self.data_flag = 1
                self.y = []


                
         
        
    def getOnce_slot(self):
        self.serial.write('g'.encode())

    def start_slot(self):
        self.serial.write('s'.encode())

    def stop_slot(self):
        self.serial.write('t'.encode())

app = QtWidgets.QApplication([])
application = MainWindow()
application.show()

sys.exit(app.exec())