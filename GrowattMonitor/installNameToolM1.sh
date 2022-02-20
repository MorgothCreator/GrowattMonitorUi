#!/bin/bash


install_name_tool -change @rpath/QtCharts.framework/Versions/A/QtCharts @executable_path/../Frameworks/QtCharts.framework/Versions/A/QtCharts ~/Documents/GitHub_Lyra/LyraWallet/LyraWallet-MacOsX/lyra-pc-wallet.app/Contents/MacOS/lyra-pc-wallet
install_name_tool -change @rpath/QtOpenGLWidgets.framework/Versions/A/QtOpenGLWidgets @executable_path/../Frameworks/QtOpenGLWidgets.framework/Versions/A/QtOpenGLWidgets ~/Documents/GitHub_Lyra/LyraWallet/LyraWallet-MacOsX/lyra-pc-wallet.app/Contents/MacOS/lyra-pc-wallet
install_name_tool -change @rpath/QtWidgets.framework/Versions/A/QtWidgets @executable_path/../Frameworks/QtWidgets.framework/Versions/A/QtWidgets ~/Documents/GitHub_Lyra/LyraWallet/LyraWallet-MacOsX/lyra-pc-wallet.app/Contents/MacOS/lyra-pc-wallet
install_name_tool -change @rpath/QtOpenGL.framework/Versions/A/QtOpenGL @executable_path/../Frameworks/QtOpenGL.framework/Versions/A/QtOpenGL ~/Documents/GitHub_Lyra/LyraWallet/LyraWallet-MacOsX/lyra-pc-wallet.app/Contents/MacOS/lyra-pc-wallet
install_name_tool -change @rpath/QtGui.framework/Versions/A/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/A/QtGui ~/Documents/GitHub_Lyra/LyraWallet/LyraWallet-MacOsX/lyra-pc-wallet.app/Contents/MacOS/lyra-pc-wallet
install_name_tool -change @rpath/QtWebSockets.framework/Versions/A/QtWebSockets @executable_path/../Frameworks/QtWebSockets.framework/Versions/A/QtWebSockets ~/Documents/GitHub_Lyra/LyraWallet/LyraWallet-MacOsX/lyra-pc-wallet.app/Contents/MacOS/lyra-pc-wallet
install_name_tool -change @rpath/QtNetwork.framework/Versions/A/QtNetwork @executable_path/../Frameworks/QtNetwork.framework/Versions/A/QtNetwork ~/Documents/GitHub_Lyra/LyraWallet/LyraWallet-MacOsX/lyra-pc-wallet.app/Contents/MacOS/lyra-pc-wallet
install_name_tool -change @rpath/QtCore5Compat.framework/Versions/A/QtCore5Compat @executable_path/../Frameworks/QtCore5Compat.framework/Versions/A/QtCore5Compat ~/Documents/GitHub_Lyra/LyraWallet/LyraWallet-MacOsX/lyra-pc-wallet.app/Contents/MacOS/lyra-pc-wallet
install_name_tool -change @rpath/QtCore.framework/Versions/A/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/A/QtCore ~/Documents/GitHub_Lyra/LyraWallet/LyraWallet-MacOsX/lyra-pc-wallet.app/Contents/MacOS/lyra-pc-wallet
otool -l ~/Documents/GitHub_Lyra/LyraWallet/LyraWallet-MacOsX/lyra-pc-wallet.app/Contents/MacOS/lyra-pc-wallet
