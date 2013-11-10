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

#include "urtupdater.h"

DownloadM::DownloadM(QObject *parent) :
    QObject(parent),dCurrentReply(0),dFile(0),dDlSizeAtPause(0)
{
    dManager = new QNetworkAccessManager( this );
}

void DownloadM::download( QUrl url , QString matchMd5 )
{
    url0 = url;
    QFileInfo fileInfo(url.path());
    QString filename;
    filename=fileInfo.fileName();

    curentMd5 = matchMd5;

    dDlSizeAtPause = 0;
    dCurrentRequest = QNetworkRequest(url);

    // remove the older file that has the same name
    if(filename.contains(".pk3", Qt::CaseInsensitive) || filename.contains(".cfg", Qt::CaseInsensitive)
            || filename.contains(".txt", Qt::CaseInsensitive))

        QFile::remove(assetsDir+"/"+filename);

    else
        QFile::remove(filename);

    dFile = new QFile(filename);
    dFile->open(QIODevice::ReadWrite);

    downloadTime.start();

    download(dCurrentRequest);
}

void DownloadM::pause()
{
    if( dCurrentReply == 0 )
        return;

    disconnect(dCurrentReply,SIGNAL(finished()),this,SLOT(finished()));
    disconnect(dCurrentReply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(downloadProgress(qint64,qint64)));
    disconnect(dCurrentReply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(error(QNetworkReply::NetworkError)));

    dCurrentReply->abort();
    dFile->write( dCurrentReply->readAll());
    dCurrentReply = 0;
}

void DownloadM::resume()
{
    dDlSizeAtPause = dFile->size();
    QByteArray rangeHeaderValue = "bytes=" + QByteArray::number(dDlSizeAtPause) + "-";
    dCurrentRequest.setRawHeader("Range",rangeHeaderValue);

    download(dCurrentRequest);
}

void DownloadM::download( QNetworkRequest& request )
{
    dCurrentReply = dManager->get(request);

    connect(dCurrentReply,SIGNAL(finished()),this,SLOT(finished()));
    connect(dCurrentReply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(downloadProgress(qint64,qint64)));
    connect(dCurrentReply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(error(QNetworkReply::NetworkError)));
}

void Append_file_to_file(char *inputPath, char *outputPath)
{
        qint64 BUFFER_SIZE = 10*1024*1024;
        qint64 buffer_len = 0;

        QFile inputfile(inputPath);
        if (!inputfile.open(QIODevice::ReadOnly))
                 return;

        QFile outputfile(outputPath);
        if (!outputfile.open(QIODevice::WriteOnly | QIODevice::Append))
                 return;

        QDataStream inputstream(&inputfile);
        QDataStream outputstream(&outputfile);

        char* buffer = new char[BUFFER_SIZE];

        while(!inputstream.atEnd())
        {
                buffer_len = inputstream.readRawData( buffer, BUFFER_SIZE );
                outputstream.writeRawData( buffer, buffer_len );
        }

        inputfile.close();
        outputfile.close();

        delete[] buffer;

        return;
}

void DownloadM::finished()
{
    dFile->close();

    QFileInfo fileInfo(url0.path());
    QString filename;

    filename = fileInfo.fileName();

    QString tmp_md5 = UrTUpdater::getMd5( filename );

    //if ( curentMd5 != tmp_md5 ) {
    //    errorMd5();
    //}

    // move the .pk3, .cfg in the assets subdir
    if( (filename.contains(".pk3", Qt::CaseInsensitive) && !filename.contains(".part", Qt::CaseInsensitive)) || filename.contains(".cfg", Qt::CaseInsensitive)
            || (filename.contains(".txt", Qt::CaseInsensitive)) || (filename.contains(".doc", Qt::CaseInsensitive)))
    {
        dFile->rename(filename, assetsDir+"/"+filename);
    }


    if(filename.contains(".part", Qt::CaseInsensitive))
    {
            QString finalFileName;
            finalFileName = filename.split(".part").at(0)+".tmp";

            Append_file_to_file(filename.toUtf8().data(), finalFileName.toUtf8().data());
            QFile::remove(filename);
    }

    //@Barbatos - apply chmod +x for executable files on linux
    if((filename.contains(".i386", Qt::CaseInsensitive) || (filename.contains(".x86_64", Qt::CaseInsensitive))) && (Platform == "Linux"))
    {
        QString cmd("chmod +x "+filename);
        QProcess* process = new QProcess(this);
        process->start(QFile::encodeName(cmd).data());
        process->waitForFinished(3000);
    }

    if((Platform == "Linux") || (Platform == "Mac"))
    {
        if(filename.contains(".zip", Qt::CaseInsensitive))
        {
            QString cmd("unzip -q -o "+filename);
            QProcess* process = new QProcess(this);
            process->start(QFile::encodeName(cmd).data());
            process->waitForFinished(30000);
            QFile::remove(filename);
        }
    }

    dFile = 0;
    dCurrentReply = 0;

    emit downloadComplete(curentMd5, tmp_md5);
}

void DownloadM::downloadProgress ( qint64 bytesReceived, qint64 bytesTotal )
{
    QString unit;
    qint64 speed;
    qint64 percentage;

    dFile->write( dCurrentReply->readAll() );

    if (dDlSizeAtPause+bytesTotal == 0){
        emit progress(0, 0, "b/s");
        return;
    }

    percentage = ((bytesReceived * 100) / bytesTotal);

    //@Barbatos - calculate the download speed
    speed = bytesReceived * 1000.0 / downloadTime.elapsed();

    if (speed < 1024) {
        unit = "b/s";
    } else if (speed < 1024*1024) {
        speed /= 1024;
        unit = "kb/s";
    } else {
        speed /= 1024*1024;
        unit = "Mb/s";
    }

    emit progress(percentage, speed, unit);
}

void DownloadM::error(QNetworkReply::NetworkError code)
{
    QMessageBox::critical(0, "Download error", "Could not download the files. Please check your internet connection. <br />"
                          "You can ask for help on the official website: www.urbanterror.info");
    qApp->quit();
}

