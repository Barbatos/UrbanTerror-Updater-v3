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
  
#ifndef URTUPDATER_H
#define URTUPDATER_H

#include "download.h"

namespace Ui {
class UrTUpdater;
}

class UrTUpdater : public QMainWindow
{
    Q_OBJECT

public:
    explicit UrTUpdater(QWidget *parent = 0);
    ~UrTUpdater();
    QString Platform;

public slots:

    void checkGameFolder();
    void checkPlatform();
    void appQuit();


    //==================================================================//
    //                              API                                 //
    //==================================================================//

    void sendRequestAPI( );
    void sendRequestAPI( QString );
    void sendRequestAPI( QStringList );
    void sendRequestAPI( QString , QStringList );
    void receiveAnswerAPI();


    //==================================================================//
    //                          API ERRORS                              //
    //==================================================================//

    void apiBadRequest();
    void apiBadAnswer();
    void apiDlError(QNetworkReply::NetworkError);


    //==================================================================//
    //                       LAYOUT MANAGEMENT                           //
    //==================================================================//

    void loadLayout();

    void updateBT( QPushButton* , QStringList );
    void BT_clicked( QPushButton* , int );

    //==================================================================//
    //                              XXX                                 //
    //==================================================================//


    static QByteArray getMd5(QString filename);


    void downloadFiles();
    void downloadProgress(qint64, qint64, QString);
    void downloadFinished(QString, QString);

    void openAboutPage();
    void openHelpPage();

    void errorMd5();

private slots:

    void on_BT_1_clicked();

    void on_BT_2_clicked();

    void on_BT_3_clicked();

private:
    Ui::UrTUpdater *ui;

    QString sepa1;
    QString sepa2;
    QString sepa3;

    QByteArray UpdaterVersion;
    QString assetsDir;
    QString linkAPI;
    QString updaterVersion;

    QStringList filesInFolders;
    QNetworkReply *apiAnswer;

    QTextEdit *textZone;
    QPushButton *quitButton;
    QPushButton *okButton;
    QLabel *iconUrT;
    QLabel *copyright;

    QStringList windowsList;

    bool everythingFinished;
    bool downloadInProgress;
    QStringList filesList;
    QLabel *dlText;
    int nbFilesToDl;
    int nbFilesDownloaded;

    QString prevKey;
    QString password;

    DownloadM* dManager;
    QProgressBar *dProgressBar;
    QLabel *dlRateText;
    bool authKeyExists;
};

#endif // URTUPDATER_H

