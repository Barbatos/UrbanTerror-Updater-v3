/**
  * Urban Terror Updater
  *
  * This software is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 2.1 of the License, or (at your option) any later version.
  *
  * This software is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with this software; if not, write to the Free Software
  * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
  *
  * @version    3.1
  * @authors    Charles 'Barbatos' Duprey & Jean-Philippe 'HoLbLiN' Zolesio
  * @email      barbatos@urbanterror.info
  * @copyright  2012/2013 Frozen Sand / 0870760 B.C. Ltd
  */

#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <iostream>
#include <QApplication>
#include <QObject>
#include <QFileInfo>
#include <QMainWindow>
#include <QtGui>
#include <QWidget>
#include <QUrl>
#include <QPushButton>
#include <QMessageBox>
#include <QProgressDialog>
#include <QFile>
#include <QLabel>
#include <QTime>
#include <QTextStream>
#include <QProcess>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QStatusBar>
#include <QTextEdit>
#include <QUrlQuery>
#include <QByteArray>

class DownloadM : public QObject
{
	Q_OBJECT
public:
	explicit DownloadM(QObject *parent = 0);
	QFileInfo	fileInfo;
	QUrl		url0;
	QString		curentMd5;
	QString		assetsDir;
	QStringList	filesPart;
	QString		Platform;
    QTime       downloadTime;
    int         nbTries;

signals:
    void downloadComplete(QString, QString);
    void progress( qint64 percentage, qint64 speed, QString unit);

public slots:
	void download(QUrl url, QString matchMd5);
	void pause();
    void resume();

private slots:
	void download( QNetworkRequest& request );
	void finished( );
	void downloadProgress ( qint64 bytesReceived, qint64 bytesTotal );
	void error ( QNetworkReply::NetworkError code );

private:

	QNetworkAccessManager*	dManager;
	QNetworkRequest			dCurrentRequest;
	QNetworkReply*			dCurrentReply;
	QFile*					dFile;

	int dDlSizeAtPause;
};

#endif // DOWNLOAD_H
