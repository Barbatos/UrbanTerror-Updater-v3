#
# Urban Terror Updater
#
# This software is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this software; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#
# @version    3.1
# @authors    Charles 'Barbatos' Duprey & Jean-Philippe 'HoLbLiN' Zolesio
# @email      barbatos@urbanterror.info
# @copyright  2012/2013 Frozen Sand / 0870760 B.C. Ltd

LFLAGS = -static-libgcc

# TTimo - http://qt-project.org/forums/viewthread/16957/
CONFIG   += x86

QT       += core gui network widgets
#QT       += webkit

TARGET = UrTUpdater
TEMPLATE = app

INCLUDEPATH += . \
                quazip

SOURCES += main.cpp\
           urtupdater.cpp\
           download.cpp

HEADERS  += urtupdater.h\
            download.h \
            ui_urtupdater.h

FORMS    += urtupdater.ui

RC_FILE = urtupdater.rc

RESOURCES += \
	images.qrc
