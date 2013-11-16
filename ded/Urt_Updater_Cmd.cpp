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

#ifdef _WIN32
	#include "stdafx.h"
#endif

#include <iostream>
#include <list>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/iter_find.hpp>

using namespace std;

__asm__(".symver realpath,realpath@GLIBC_2.2.5");

string	UpdaterVersion			= "";
string	UpdaterVersionSHA1		= "";
string	assetsDir				= "q3ut4";

string	LinkAPI					= "";

string	prevKey					= "";

string	password				= "";

string	sepa1					= "";
string	sepa2					= "";
string	sepa3					= "";

string	Platform				= "LinuxCMD";

// string	limitRate			= "--limit-rate=2000k";
string	limitRate				= "";

string	BT1, BT2, BT3;

list<string> windowsList;

bool quiet = false;

string split_first( string &input, string delim );
list<string> split( string input, string delim );
FILE* sys_popen( const char* command , const char* mode);
int sys_pclose( FILE* pipe );
int exec( string cmd );
int exec( string cmd, string &result );
void checkGameFolder();
void apiBadAnswer();
void sendRequestAPI( string btnKey , list<string> md5sums );
void sendRequestAPI( list<string> md5sums );
void sendRequestAPI( string btnKey );
void sendRequestAPI( );
void BTaction( list<string> BT );
void BTchoice( string BT1, string BT2, string BT3 );
void loadLayout();
int main( int argc, char* argv[]);


string split_first( string &input, string delim ){
	if ( input.size() == 0 )
		return "";
	int l = input.find( delim );
	string result = input.substr( 0 , l );
	input = input.substr( l + delim.size() );
	return result;
}

list<string> split( string input, string delim ) {
	list<string> result;

	int l;
	do {
		if ( input.size() < 1 ) {
			result.push_back( "" );
			break;
		}
		else {
			l = input.find( delim );
			result.push_back( input.substr( 0 , l ) );
			input = input.substr( l + delim.size() );
		}
	}while ( -1 != l ) ;

	return result;
}

/*
void replaceAll(std::string& str, const std::string& from, const std::string& to) {
	if(from.empty())
		return;
	size_t start_pos = 0;
	while((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}
*/



FILE* sys_popen( string command , const char* mode){
	#ifdef _WIN32
		return _popen( (command + " 2>&1" ).c_str() , mode);
	#else
		return popen(  (command + " 2>&1" ).c_str() , mode);
	#endif
}

int sys_pclose( FILE* pipe ){
	#ifdef _WIN32
		return _pclose( pipe );
	#else
		return pclose( pipe );
	#endif
}


int exec( string cmd ) {
	#ifdef _WIN32
	return system( cmd.c_str() );
	#else
		return system( ( cmd + " >/dev/null 2>/dev/null" ).c_str() );
	#endif
}

int exec( string cmd, string &result ) {
	FILE* pipe = sys_popen( cmd , "r" );
	if (!pipe)
		return 1;

	char buffer[128];
	result = "";

	while(!feof(pipe)) {
		if(fgets(buffer, 128, pipe) != NULL)
			result += buffer;
	}
	return sys_pclose(pipe);
}

void checkGameFolder() {

	cout << "Checking game folder...";
	
	exec( "mkdir -p " + assetsDir );
	
	// NOTE: that's in the top folder, not in assetsDir .. this is where the downloads go initially
	exec( "rm ./*.part" );
	exec( "rm ./*.pk3" );
	
	cout << " [Done]" << endl;
}

void apiBadAnswer() {
	cerr << "[API error]" << " Infos from www.urbanterror.info are not valid. Try again later." << endl;
	exit( EXIT_FAILURE );
}

void sendRequestAPI( string btnKey , list<string> md5sums ){
	string	args = "";
	string  file, output;
	bool tmp = false;

	cout << "Waiting data from urbanterror.info..." << endl;
	fflush(stdout);
	
	args += "p0=" + UpdaterVersionSHA1;
	args += "&p1=" + Platform;

	if ( btnKey == "" )
		args += "&p2=" + prevKey;
	else
		args += "&p2=" + btnKey;

	if ( md5sums.size() != 0){
		cout << "    Getting data for request (" << md5sums.size() << ")"<< endl;
		cout << "    ";
		fflush(stdout);
		tmp = true;
	}
	
	while( md5sums.size() > 0 ){
		file = md5sums.front();

		if ( 0 != exec( "md5sum "+ file , output ) ) {
			args += "&p3[" + file + "]=";
		}
		else {
			args += "&p3[" + file + "]=" + split_first( output , " " );
		}

		cout << ".";
		fflush(stdout);
	
		md5sums.pop_front();
	}

	if ( password != "" ){
		args += "&p4=" + password;
	}

	if ( tmp ){
		cout << " [Done]" << endl;
	}

	if( 0 != exec( "wget "+LinkAPI+" -q -O - --post-data=\"" + args + "\"" , output )){
		cerr << "Could not open the downloaded file from the API, aborted." << endl;
		exit( EXIT_FAILURE );
	}

	prevKey = split_first( output , sepa1 );

	windowsList.clear();
	windowsList = split( output , sepa1 );

	cout << "Instructions from urbanterror.info received." << endl;
	
	loadLayout();
}

void sendRequestAPI( list<string> md5sums ){
	sendRequestAPI( "" , md5sums );
}

void sendRequestAPI( string btnKey ){
	list<string> s;
	sendRequestAPI( btnKey ,  s);
}

void sendRequestAPI( ){
	list<string> s;
	sendRequestAPI( "" , s );
}

void BTaction( list<string> BT ){

	BT.pop_front();
	string action	= BT.front();
	string arg		= "";

	if ( BT.size() == 2 )
		arg = BT.back();

	if ( action == "quit" ){
		exit( EXIT_SUCCESS );
	}

	else if ( action == "sendApi" ){
		sendRequestAPI( arg );
	}

	else if ( action == "launch" ){
		if (arg == "")
			apiBadAnswer();
		system( arg.c_str() );
		exit( EXIT_SUCCESS );
	}

	else if ( action == "goto" ){
		if (arg == "") {
			windowsList.pop_front();
			loadLayout();
		}
		else {
			int n = 1;

			if ( atoi( arg.c_str() ) > 0 )
				n = atoi( arg.c_str() );

			for (int i = 0; i < n; i ++)
				windowsList.pop_front();

			loadLayout();
		}
	}

	else {
		apiBadAnswer();
	}

}

void BTchoice( string BT1, string BT2, string BT3 ){
	list<string> bt1 = split( BT1 , sepa3 );
	list<string> bt2 = split( BT2 , sepa3 );
	list<string> bt3 = split( BT3 , sepa3 );
	
	int i;

	if(quiet == true){
		BTaction(bt3);
		return;
	}
		
	do {
		string name = bt3.front();
		if ( name != "" && name != "hide" ) {
			cout << " 1) " << name;
		}
	
		name = bt2.front();
		if ( name != "" && name != "hide" ) {
			cout << " 2) " << name;
		}
	
		name = bt1.front();
		if ( name != "" && name != "hide" ) {
			cout << " 3) " << name;
		}

		string rep;
		cout << endl << "=> ";
		cin >> rep;
		i = atoi( rep.c_str() );

	} while( i < 1 || i > 3 );

	if ( ( bt1.front() == "hide" || bt1.front() == "" ) && i == 3 ){
		i = 2;
	}

	if ( ( bt2.front() == "hide" || bt2.front() == "" ) && i == 2 ){
		i = 1;
	}

	switch( i ){
		case 3:
			BTaction(bt1);
			break;

		case 2:
			BTaction(bt2);
			break;

		case 1:
		default:
			BTaction(bt3);
			break;
	}

}

void loadLayout() {
	list<string>	tmpStrList;
	string			tmpStr;
	string			name;
	
	if (windowsList.size() < 1)
		apiBadAnswer();

	tmpStrList = split( windowsList.front() , sepa2 );
	if ( tmpStrList.size() < 4 )
		apiBadAnswer();
	
	name	= tmpStrList.front();	tmpStrList.pop_front();

	BT1		= tmpStrList.front();	tmpStrList.pop_front();
	BT2		= tmpStrList.front();	tmpStrList.pop_front();
	BT3		= tmpStrList.front();	tmpStrList.pop_front();


	if( name == "download" ){
		
		cout << "Downloading files (" << tmpStrList.size() << ")"<< endl;
		
		while ( tmpStrList.size() > 0 )
		{
			tmpStr		= tmpStrList.front();
			string url	= split_first( tmpStr , sepa3 );
			string md5	= split_first( tmpStr , sepa3 );
			vector<string> fileDir;
			string fileName;

			boost::iter_split(fileDir, url, boost::first_finder("/"));
			fileName = *(fileDir.end() - 1);
		
			cout << "Downloading: " << fileName;

			exec( "wget " + url + " -nc -q --wait=5 --no-directories " + limitRate );
			
			list<string> lstDir = split( url , "/" );

			// check the md5sum
			string output;
			if ( 0 != exec( "md5sum ./"+ lstDir.back() , output ) ) {
				cerr << "download error" << endl;
				exit( EXIT_FAILURE );
			}
			else {
				if ( split_first( output , " " ) != md5 ){
					cerr << "download corrupted" << endl;
					exec( "rm ./"+ lstDir.back() );
					exit( EXIT_FAILURE );
				}
			}
			
			cout << " [DONE!]" << endl;

			if( (strstr( lstDir.back().c_str() , ".pk3"))
					|| (strstr( lstDir.back().c_str(), ".cfg")) 
					|| (strstr( lstDir.back().c_str(), ".txt")))
			{
				exec( "mv ./"+ lstDir.back() + " ./"+ assetsDir + "/" + lstDir.back() );
			}

			// Barbatos - check if it's an executable file and chmod +x it
			if(strstr(lstDir.back().c_str(), ".i386") || (strstr(lstDir.back().c_str(), ".x86_64")))
			{
				cout << "Applying chmod +x for the executable file " << lstDir.back() << endl;
				if(exec( "chmod +x ./"+ lstDir.back() ))
					cerr << "chmod failed." << endl;
			}

			fflush(stdout);
		
			tmpStrList.pop_front();
		}
		
		windowsList.pop_front();
		loadLayout();
	}

	else if( name == "move" ){
				
		cout << "Moving files (" << tmpStrList.size() << ")"<< endl;
		
		while ( tmpStrList.size() > 0 ){
			tmpStr = tmpStrList.front();
			exec( "mv ./" + split_first( tmpStr , sepa3 ) + " ./" + split_first( tmpStr , sepa3 ) );
			
			cout << ".";
			fflush(stdout);
	
			tmpStrList.pop_front();
		}

		cout << " [Done]" << endl;
		
		windowsList.pop_front();
		loadLayout();
	}

	else if( name == "remove" ){
	
		cout << "Removing files (" << tmpStrList.size() << ")"<< endl;

		while ( tmpStrList.size() > 0 ){
			tmpStr = tmpStrList.front();
			exec( "rm ./" + tmpStr );
			
			cout << ".";
			fflush(stdout);
			
			tmpStrList.pop_front();
		}

		cout << " [Done]" << endl;
		
		windowsList.pop_front();
		loadLayout();
	}

	else if( name == "info" ){
		cout << "Informations :" << endl << endl;

		boost::replace_all(tmpStrList.front(), "<br /><br />", "<br />");
		boost::replace_all(tmpStrList.front(), "<br />", "\n");

		cout << tmpStrList.front() << endl;
		
		BTchoice( BT1, BT2, BT3 );

	}

	else if( name == "audit" ){
		sendRequestAPI( tmpStrList );
	}

	else {
		apiBadAnswer();
	}
}


int main( int argc, char* argv[]){

	for ( int i = 1 ; i < argc ; ++i ){
		if( !strcmp( argv[i] , "--version" ) ) {
			prevKey = argv[i];
		}

		if( !strcmp( argv[i] , "--password" ) ) {
			password = argv[i];
		}

		if( !strcmp( argv[i] , "--quiet" ) || !strcmp(argv[ i ], "-q") ) {
			quiet = true;
		}
	}

	cout << "==========================" << endl;
	cout << "Urban Terror Updater" << endl;
	cout << "Version: " << UpdaterVersion << endl;
	cout << "Copyright 2013 FrozenSand/0870760 B.C. Ltd" << endl;
	cout << "Get help on www.urbanterror.info" << endl;
	cout << "==========================" << endl;

	checkGameFolder();

	sendRequestAPI();

}
