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
#include "ui_urtupdater.h"

UrTUpdater::UrTUpdater(QWidget *parent) : QMainWindow(parent), ui(new Ui::UrTUpdater)
{
    ui->setupUi(this);

    dManager = new DownloadM(this);

    linkAPI             = "";
    prevKey             = "";
    password            = "";
    updaterVersion      = "3.1";
    dManager->assetsDir	= "q3ut4";
    dManager->nbTries = 0;

    sepa1               = "";
    sepa2               = "";
    sepa3               = "";

    everythingFinished	= false;
    downloadInProgress	= false;

    QStringList arguments = QCoreApplication::arguments();
    if(arguments.count() >= 1) {
        for(int i = 0; i < arguments.count(); i++) {
            if(arguments.at(i) == "--version" && (i + 1 < arguments.count()) ) {
                i++;
                prevKey = arguments.at( i );
            }
            if(arguments.at(i) == "--password" && (i + 1 < arguments.count()) ) {
                i++;
                password = arguments.at( i );
            }
        }
    }

    //to REMOVE the MINIMIZE and MAXIMIZE buttons
    //setWindowFlags( windowFlags() & ~Qt::WindowCloseButtonHint );
    //setWindowFlags( windowFlags() & ~Qt::WindowMinimizeButtonHint);

    // no action on botton ... so disable them
    ui->BT_1->hide();
    ui->BT_2->hide();
    ui->BT_3->hide();

    // switch layout
    ui->layouts->setCurrentWidget( this->ui->l_waitAPI);

    dlRateText = new QLabel(this);
    dlRateText->move(160, 210);
    dlRateText->setFixedWidth(200);
    dlRateText->setStyleSheet("color:white;");
    dlRateText->hide();

    // Start the real Jobs
    checkPlatform();
    checkGameFolder();

    sendRequestAPI();

    connect(dManager,SIGNAL(downloadComplete(QString, QString)),this,SLOT(downloadFinished(QString, QString)));
    connect(dManager,SIGNAL(progress(qint64, qint64, QString)),this,SLOT(downloadProgress(qint64, qint64, QString)));

}

//==================================================================//
//                       CHECK REQUIREMENTS                         //
//==================================================================//

// check the platform, it will be
// used by the API
void UrTUpdater::checkPlatform()
{
    #ifdef Q_WS_X11
    Platform = "Linux";
    #endif

    #ifdef Q_WS_MAC
    Platform = "Mac";
    #endif

    #ifdef Q_WS_QWS
    Platform = "Linux";
    #endif

    #ifdef Q_WS_WIN
    Platform = "Windows";
    #endif

    if(Platform.isEmpty())
        Platform = "Linux";

    dManager->Platform = Platform;
}

// check if the game folder (q3ut4) exists,
// if not it means it's the first time we
// launch the updater
void UrTUpdater::checkGameFolder()
{
    QStringList filesFrom41;
    QStringList filters;
    QDir assets(dManager->assetsDir);
    QDir rootDir("./");

    if(!QDir(dManager->assetsDir).exists())
    {
        if(!QDir().mkdir(dManager->assetsDir))
        {
            QMessageBox::critical(this, "Assets folder", "Could not create the assets folder ("+dManager->assetsDir+"). Please verify the user rights of this folder. <br />"
                                                         "You can ask for help on <a href='www.urbanterror.info'>the official website</a>");
            appQuit();
        }
    }

    // Remove .part and .pk3 files from the root directory
    // If you find one it's probably coming from a previous
    // download that has been aborted.
    filters << "*.part*" << "*.pk3";
    rootDir.setNameFilters(filters);
    dManager->filesPart = rootDir.entryList(QDir::Files);

    if(!dManager->filesPart.isEmpty())
        QFile::remove(dManager->filesPart.last());

    //@Barbatos - check for 4.1 zpacks and don't let them install 4.2 over 4.1
    filters.clear();
    filters << "zpak00*";
    assets.setNameFilters(filters);
    filesFrom41 = assets.entryList(QDir::Files);

    // We found 4.1 zpacks, warn this n00b and quit!
    if(!filesFrom41.isEmpty())
    {
        QMessageBox::critical(this, "Installing 4.2 over 4.1", "You are trying to install Urban Terror 4.2 in the same folder as Urban Terror 4.1! Please create a new folder for 4.2");
        appQuit();
    }

    // Check if the authkey file exists
    QFile authKey(dManager->assetsDir+"/authkey");

    if(!authKey.open(QIODevice::ReadOnly, QIODevice::Text)) {
        authKeyExists = 0;
    }
    else {
        authKeyExists = 1;
    }

    authKey.close();
}


//==================================================================//
//                              API                                 //
//==================================================================//

void UrTUpdater::sendRequestAPI( ){
    UrTUpdater::sendRequestAPI( "" , QStringList() );
}

void UrTUpdater::sendRequestAPI( QStringList md5sums ){
    UrTUpdater::sendRequestAPI( "" , md5sums );
}

void UrTUpdater::sendRequestAPI( QString btnKey ){
    UrTUpdater::sendRequestAPI( btnKey , QStringList());
}

void UrTUpdater::sendRequestAPI( QString btnKey , QStringList md5sums ){
    QCryptographicHash hash(QCryptographicHash::Sha1);
    QUrlQuery url;
    QStringList tmpStrList;

    // hash the updater version
    hash.addData(updaterVersion.toUtf8());
    //hash.addData(QByteArray(updaterVersion));

    QUrl apiFile(linkAPI);

    QNetworkRequest apiRequest(apiFile);
    apiRequest.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded;charset=utf-8");

    url.addQueryItem("p0", hash.result().toHex() );
    url.addQueryItem("p1", Platform.toUtf8());

    if ( btnKey == "" )
        url.addQueryItem("p2", prevKey);
    else
        url.addQueryItem("p2", btnKey );

    if ( md5sums.size() != 0 ) {
        foreach ( QString md5 , md5sums ) {
            tmpStrList = md5.split("=");
            if ( tmpStrList.size() != 2 )
                apiBadRequest();
            url.addQueryItem("p3["+ tmpStrList.at(0).toUtf8()+"]", tmpStrList.at(1) );
        }
    }

    if (!password.isEmpty())
        url.addQueryItem("p4", password );

    //@Barbatos - the auth key doesn't exist
    if(!authKeyExists)
        url.addQueryItem("p5", "1");

    QNetworkAccessManager *apiManager = new QNetworkAccessManager(this);

    // get the answer from urt.info
    apiAnswer = apiManager->post(apiRequest, url.query(QUrl::FullyEncoded).toUtf8());

    ui->layouts->setCurrentWidget( ui->l_waitAPI );

    connect(apiAnswer, SIGNAL(finished()), this, SLOT(receiveAnswerAPI()));
    connect(apiAnswer, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(apiDlError(QNetworkReply::NetworkError)));
}

void UrTUpdater::receiveAnswerAPI() {
    QByteArray  apiByteAnswer;
    QStringList tmpStrList;
    apiByteAnswer = apiAnswer->readAll();

    tmpStrList = QString(apiByteAnswer).split( sepa1 );
    if ( tmpStrList.size() < 2 )
        apiBadAnswer();

    prevKey         = tmpStrList.takeFirst();

    windowsList     = tmpStrList;

    loadLayout();
}


//==================================================================//
//                          API ERRORS                              //
//==================================================================//

void UrTUpdater::apiDlError(QNetworkReply::NetworkError) {
    QMessageBox::critical(this, "API error", "Could not get the information from the API, please report it on www.urbanterror.info or try again later.");
}

void UrTUpdater::apiBadAnswer() {
    QMessageBox::critical(this, "API error", "Info from the API are not valid. Please contact a Frozen Sand admin on www.urbanterror.info or try again later.");
    exit( EXIT_FAILURE );
}

void UrTUpdater::apiBadRequest() {
    QMessageBox::critical(this, "API error", "Error when trying to build a request.");
    exit( EXIT_FAILURE );
}


//==================================================================//
//                       LAYOUT MANAGMENT                           //
//==================================================================//

void UrTUpdater::loadLayout() {
    QStringList tmpStrList;
    QString name;

    if (windowsList.size() < 1)
        apiBadAnswer();

    tmpStrList = windowsList.at(0).split( sepa2 );
    if ( tmpStrList.size() < 4 )
        apiBadAnswer();

    name        = tmpStrList.takeFirst();

    updateBT( ui->BT_1 , tmpStrList.takeFirst().split( sepa3 ) );
    updateBT( ui->BT_2 , tmpStrList.takeFirst().split( sepa3 ) );
    updateBT( ui->BT_3 , tmpStrList.takeFirst().split( sepa3 ) );

    if( name == "download" ){
        ui->dl_ProgressBar->setValue(0);
        ui->dl_Text->setText("Downloading ...");
        ui->layouts->setCurrentWidget( ui->l_download );

        filesList = tmpStrList;
        nbFilesToDl = filesList.count();
        nbFilesDownloaded = 0;

        if (nbFilesToDl != 0){
            downloadFiles();
        }
        else {
            windowsList.removeFirst();
            loadLayout();
        }

    }
    else if( name == "move" ){
        ui->mv_ProgressBar->setValue(0);
        ui->mv_Text->setText("Moving ...");
        ui->layouts->setCurrentWidget( ui->l_move );

        int i = 1;
        QString move;
        foreach ( move , tmpStrList){
            QStringList tmp = move.split( sepa3 );
            if (tmp.size() != 2 )
                apiBadAnswer();
            ui->mv_Text->setText("Moving : " + tmp.at(0).split("/").last() + " ( " + QString::number( i ) + " / " + QString::number( tmpStrList.size() ) + " )" );
            QFile file( "./" + tmp.at(0));
            file.rename( tmp.at(1) );
            ui->mv_ProgressBar->setValue( (i * 100 )/ tmpStrList.size() );
            i++;
        }

        windowsList.removeFirst();
        loadLayout();
    }
    else if( name == "remove" ){
        ui->rm_ProgressBar->setValue(0);
        ui->rm_Text->setText("Removing ...");
        ui->layouts->setCurrentWidget( ui->l_remove );

        QString remove;
        int i = 1;
        foreach ( remove , tmpStrList){
            ui->rm_Text->setText("Removing : " + remove.split("/").last() + " ( " + QString::number(i) + " / " + QString::number( tmpStrList.size() ) + " )" );
            QFile file( "./" + remove );
            file.remove();
            ui->rm_ProgressBar->setValue( (i * 100 )/ tmpStrList.size() );
            i++;
        }

        windowsList.removeFirst();
        loadLayout();
    }
    else if( name == "info" ){
        ui->layouts->setCurrentWidget( ui->l_information );
        ui->info_textZone->setHtml( tmpStrList.first() );
    }
    else if( name == "audit" ){
        ui->audit_ProgressBar->setValue(0);
        ui->audit_Text->setText("Collecting information ...");
        ui->layouts->setCurrentWidget( ui->l_audit );

        QStringList md5sum;
        QString audit;
        int i = 1;
        foreach ( audit , tmpStrList){
            ui->audit_Text->setText("Checking "+audit.split("/").last()+" ( " + QString::number(i) + " / " + QString::number(tmpStrList.size()) + " )" );
            md5sum.append( audit + "=" + getMd5( audit ) );
            ui->audit_ProgressBar->setValue( (i * 100 )/ tmpStrList.size() );
            i++;
        }
        sendRequestAPI(md5sum);
    }
    else {
        apiBadAnswer();
    }
}


//==================================================================//
//                       BUTTONS MANAGEMENT                         //
//==================================================================//

void UrTUpdater::updateBT( QPushButton *BT , QStringList args ) {
    if ( args.size() < 1 )
        apiBadAnswer();

    if ( args.size() == 1 ){
        if ( args.at(0) == "hide" )
            BT->hide();
        else
            apiBadAnswer();
    }
    else if ( args.size() == 2 || args.size() == 3 ){
        BT->setText( args.at(0) );
        BT->setEnabled( true );
        BT->show();
    }
    else
        apiBadAnswer();
}

void UrTUpdater::BT_clicked( QPushButton *BT , int n ){
    QStringList tmpStrList;
    QString     action;
    QString     arg;

    ui->BT_1->setEnabled( false );
    ui->BT_2->setEnabled( false );
    ui->BT_3->setEnabled( false );

    if (windowsList.size() < 1)
        apiBadAnswer();

    tmpStrList = windowsList.at(0).split( sepa2 );
    if ( tmpStrList.size() < 4 )
        apiBadAnswer();

    tmpStrList = tmpStrList.at( n ).split( sepa3 );
    action = tmpStrList.at( 1 );

    if ( tmpStrList.size() == 3 )
        arg = tmpStrList.at( 2 );
    else
        arg = "";

    if ( action == "quit" ){
        qApp->quit();
    }
    else if ( action == "sendApi" ){
        sendRequestAPI( arg );
    }
    else if ( action == "launch" ){
        if (arg == "")
            apiBadAnswer();

        QProcess process;
        process.startDetached( arg );
        process.waitForStarted();
        qApp->quit();
    }
    else if ( action == "goto" ){
        if (arg == "") {
            windowsList.removeFirst();
            loadLayout();
        }
        else {
            int n = 1;
            if ( arg.toInt() > 0 )
                n = arg.toInt();
            for (int i = 0; i < n; i ++)
                windowsList.removeFirst();
            loadLayout();
        }
    }
    else {
        apiBadAnswer();
    }
}

void UrTUpdater::on_BT_1_clicked() {
    this->BT_clicked( ui->BT_1 , 1 );
}

void UrTUpdater::on_BT_2_clicked() {
    this->BT_clicked( ui->BT_2 , 2 );
}

void UrTUpdater::on_BT_3_clicked() {
    this->BT_clicked( ui->BT_3 , 3 );
}

//==================================================================//
//                          Layout DL                               //
//==================================================================//

void UrTUpdater::downloadProgress(qint64 percentage, qint64 speed, QString unit)
{
    dlRateText->setText("Speed:  "+QString::number(speed, 'f', 2)+" "+QString(unit));
    ui->dl_ProgressBar->setValue( percentage);
}

void UrTUpdater::downloadFinished(QString md5FromWeb, QString md5File)
{
    downloadInProgress = false;
    dlRateText->hide();

    filesList.first();

    //@Barbatos
    // check if the md5 are equal, go to the next file
    if(md5FromWeb == md5File) {
        filesList.removeFirst();
        dManager->nbTries = 0;
    }

    // if we tried more than two times to download the file and it still fails, abort all downloads
    else if((dManager->nbTries >= 2) && (md5FromWeb != md5File)) {
        return errorMd5();
    }

    // if the download failed but we haven't tried more than two times, let's try again
    else {
        dManager->nbTries++;
        nbFilesDownloaded--;
    }

    // if the list isn't empty, start the download of a file
    if(!filesList.empty() && !filesList.at(0).isEmpty() && filesList.at(0) != ""){
        downloadFiles();
    }

    // else we've finished our downloads
    else {
        windowsList.removeFirst();
        loadLayout();
    }
}

void UrTUpdater::downloadFiles()
{
    QStringList tmpStrList;
    QString fileClean;

    if( filesList.size() > 0 )
    {
        if(downloadInProgress != true)
        {
            downloadInProgress = true;
            this->ui->layouts->setCurrentWidget( this->ui->l_download );
            nbFilesDownloaded++;

            tmpStrList = filesList.first().split( sepa3 );

            fileClean = tmpStrList.first().split("/").last();

            ui->dl_Text->setText("Downloading: "+ fileClean +" ( "+QString::number(nbFilesDownloaded) +" / "+QString::number(nbFilesToDl)+" )");
            dlRateText->show();

            dManager->download(QUrl( tmpStrList.first() ) , tmpStrList.last() );
        }
    }
}

//==================================================================//
//                         TOOLS & EXTRA                            //
//==================================================================//


QByteArray UrTUpdater::getMd5(QString filename)
{
    QCryptographicHash hash(QCryptographicHash::Md5);
    QFile in( "./" + filename );
    QFileInfo fileInfo( "./" + filename );

    const qint64 bufferSize = 10000;
    if (! in.open(QIODevice::ReadOnly)) {
        return "";
    }

    qint64 imageSize = in.size();

    char buf[bufferSize];
    int bytesRead;

    hash.reset();
    int readSize = qMin(imageSize, bufferSize);
    while (readSize > 0 && (bytesRead = in.read(buf, readSize)) > 0) {
        imageSize -= bytesRead;
        hash.addData(buf, bytesRead);
        readSize = qMin(imageSize, bufferSize);
    }

    in.close();

    return hash.result().toHex();
}

void UrTUpdater::openAboutPage() {
    QMessageBox::information(this, "About UrT Updater", "<strong>Urban Terror Updater</strong><br />Version <strong>"+updaterVersion+"</strong><br /><br />By Charles 'Barbatos' Duprey and Jean-Philippe 'HoLbLiN' Zolesio<br />barbatos@urbanterror.info<br /><a href=\"http://www.urbanterror.info\">www.urbanterror.info</a>");
}

void UrTUpdater::openHelpPage() {
    QMessageBox::information(this, "Get help", "If you are having issues using this updater, please open a thread on our forums (www.urbanterror.info) or contact me: barbatos@urbanterror.info");
}

void UrTUpdater::errorMd5()
{
    QMessageBox::critical(0, "Download checksum error", "The downloaded file is corrupted. Please check your internet connection and launch again the Updater. <br />"
                          "You can ask for help on the official website: www.urbanterror.info");
    appQuit();
}

void UrTUpdater::appQuit() {
    qApp->quit();
}

UrTUpdater::~UrTUpdater() {
    delete ui;
}

