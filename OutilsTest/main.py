import sys
from PySide6.QtUiTools import QUiLoader
from PySide6.QtWidgets import QApplication
from PySide6.QtCore import QFile, QIODevice


def connectSerial():
    print("serial")


def connectJoystick():
    print("joysticl")


if __name__ == "__main__":
    app = QApplication(sys.argv)

    ui_file_name = "mainWindow.ui"
    ui_file = QFile(ui_file_name)
    if not ui_file.open(QIODevice.ReadOnly):
        print(f"Cannot open {ui_file_name}: {ui_file.errorString()}")
        sys.exit(-1)
    loader = QUiLoader()
    window = loader.load(ui_file)

    window.btnSerial.clicked.connect(connectSerial)
    window.btnJoystick.clicked.connect(connectJoystick)

    ui_file.close()
    if not window:
        print(loader.errorString())
        sys.exit(-1)
    window.show()

    sys.exit(app.exec_())
