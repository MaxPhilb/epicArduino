# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'mainWindow.ui'
##
## Created by: Qt User Interface Compiler version 6.0.0
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide6.QtCore import *
from PySide6.QtGui import *
from PySide6.QtWidgets import *


class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        if not MainWindow.objectName():
            MainWindow.setObjectName(u"MainWindow")
        MainWindow.resize(677, 467)
        self.actionQuitter = QAction(MainWindow)
        self.actionQuitter.setObjectName(u"actionQuitter")
        self.centralwidget = QWidget(MainWindow)
        self.centralwidget.setObjectName(u"centralwidget")
        self.centralwidget.setEnabled(True)
        self.centralwidget.setMaximumSize(QSize(16777213, 16777213))
        self.verticalLayout = QVBoxLayout(self.centralwidget)
        self.verticalLayout.setSpacing(2)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.verticalLayout_3 = QVBoxLayout()
        self.verticalLayout_3.setObjectName(u"verticalLayout_3")
        self.horizontalLayout = QHBoxLayout()
        self.horizontalLayout.setSpacing(2)
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.verticalLayout_5 = QVBoxLayout()
        self.verticalLayout_5.setObjectName(u"verticalLayout_5")
        self.label = QLabel(self.centralwidget)
        self.label.setObjectName(u"label")

        self.verticalLayout_5.addWidget(self.label)

        self.comboSerial = QComboBox(self.centralwidget)
        self.comboSerial.setObjectName(u"comboSerial")

        self.verticalLayout_5.addWidget(self.comboSerial)

        self.btnSerial = QPushButton(self.centralwidget)
        self.btnSerial.setObjectName(u"btnSerial")

        self.verticalLayout_5.addWidget(self.btnSerial)

        self.verticalSpacer_2 = QSpacerItem(20, 40, QSizePolicy.Minimum, QSizePolicy.Expanding)

        self.verticalLayout_5.addItem(self.verticalSpacer_2)


        self.horizontalLayout.addLayout(self.verticalLayout_5)

        self.verticalLayout_4 = QVBoxLayout()
        self.verticalLayout_4.setObjectName(u"verticalLayout_4")
        self.label_2 = QLabel(self.centralwidget)
        self.label_2.setObjectName(u"label_2")

        self.verticalLayout_4.addWidget(self.label_2)

        self.comboJoystick = QComboBox(self.centralwidget)
        self.comboJoystick.setObjectName(u"comboJoystick")

        self.verticalLayout_4.addWidget(self.comboJoystick)

        self.btnJoystick = QPushButton(self.centralwidget)
        self.btnJoystick.setObjectName(u"btnJoystick")

        self.verticalLayout_4.addWidget(self.btnJoystick)

        self.verticalSpacer_4 = QSpacerItem(20, 40, QSizePolicy.Minimum, QSizePolicy.Expanding)

        self.verticalLayout_4.addItem(self.verticalSpacer_4)


        self.horizontalLayout.addLayout(self.verticalLayout_4)


        self.verticalLayout_3.addLayout(self.horizontalLayout)

        self.horizontalLayout_2 = QHBoxLayout()
        self.horizontalLayout_2.setObjectName(u"horizontalLayout_2")
        self.verticalLayout_7 = QVBoxLayout()
        self.verticalLayout_7.setObjectName(u"verticalLayout_7")
        self.horizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)

        self.verticalLayout_7.addItem(self.horizontalSpacer)

        self.label_3 = QLabel(self.centralwidget)
        self.label_3.setObjectName(u"label_3")

        self.verticalLayout_7.addWidget(self.label_3)

        self.pushButton = QPushButton(self.centralwidget)
        self.pushButton.setObjectName(u"pushButton")

        self.verticalLayout_7.addWidget(self.pushButton)

        self.label_5 = QLabel(self.centralwidget)
        self.label_5.setObjectName(u"label_5")

        self.verticalLayout_7.addWidget(self.label_5)

        self.resultat = QLabel(self.centralwidget)
        self.resultat.setObjectName(u"resultat")

        self.verticalLayout_7.addWidget(self.resultat)


        self.horizontalLayout_2.addLayout(self.verticalLayout_7)

        self.verticalLayout_6 = QVBoxLayout()
        self.verticalLayout_6.setObjectName(u"verticalLayout_6")
        self.contenuGrid = QVBoxLayout()
        self.contenuGrid.setObjectName(u"contenuGrid")
        self.label_4 = QLabel(self.centralwidget)
        self.label_4.setObjectName(u"label_4")

        self.contenuGrid.addWidget(self.label_4)


        self.verticalLayout_6.addLayout(self.contenuGrid)

        self.horizontalSpacer_2 = QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)

        self.verticalLayout_6.addItem(self.horizontalSpacer_2)


        self.horizontalLayout_2.addLayout(self.verticalLayout_6)


        self.verticalLayout_3.addLayout(self.horizontalLayout_2)


        self.verticalLayout.addLayout(self.verticalLayout_3)

        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QMenuBar(MainWindow)
        self.menubar.setObjectName(u"menubar")
        self.menubar.setGeometry(QRect(0, 0, 677, 24))
        self.menuFichier = QMenu(self.menubar)
        self.menuFichier.setObjectName(u"menuFichier")
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QStatusBar(MainWindow)
        self.statusbar.setObjectName(u"statusbar")
        MainWindow.setStatusBar(self.statusbar)

        self.menubar.addAction(self.menuFichier.menuAction())
        self.menuFichier.addAction(self.actionQuitter)

        self.retranslateUi(MainWindow)

        QMetaObject.connectSlotsByName(MainWindow)
    # setupUi

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QCoreApplication.translate("MainWindow", u"MainWindow", None))
        self.actionQuitter.setText(QCoreApplication.translate("MainWindow", u"Quitter", None))
        self.label.setText(QCoreApplication.translate("MainWindow", u"Choisir un port serie", None))
        self.btnSerial.setText(QCoreApplication.translate("MainWindow", u"Connecter", None))
        self.label_2.setText(QCoreApplication.translate("MainWindow", u"Choisir  un joystick", None))
        self.btnJoystick.setText(QCoreApplication.translate("MainWindow", u"Ouvrir", None))
        self.label_3.setText(QCoreApplication.translate("MainWindow", u"Lancer le test", None))
        self.pushButton.setText(QCoreApplication.translate("MainWindow", u"D\u00e9marrer", None))
        self.label_5.setText(QCoreApplication.translate("MainWindow", u"R\u00e9sultat:", None))
        self.resultat.setText(QCoreApplication.translate("MainWindow", u"ms", None))
        self.label_4.setText(QCoreApplication.translate("MainWindow", u"Bool In:", None))
        self.menuFichier.setTitle(QCoreApplication.translate("MainWindow", u"Fichier", None))
    # retranslateUi

