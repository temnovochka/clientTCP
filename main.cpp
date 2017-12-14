#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

//#include <arpa/inet.h>
//#include <stdlib.h>
//#include <sstream>
//#include <thread>
//#include <c++/5/istream>
//#include <c++/5/iostream>

#define DEFAULT_PORT 27001
#define DEFAULT_ADDR "192.168.0.100"
#define DEFAULT_BUFLEN 11

#include <stdio.h>
#include <iostream>
using namespace std;

//----------------------------------------------------------------------------------------------------------------------------------------
// A function that guarantees reading specified number of bytes
int readn( long socket, char *recvbuf, int recvbuf_len ){
    int iResult = 0;
    for( int i = 0; i < recvbuf_len; i++ )
        recvbuf[i] = 0;
    //memset( recvbuf, 0, recvbuf_len );                              // set recvbuf[i] = 0
    int numOfRecivedBytes = 0;                                      // variable to count received bytes

    while( numOfRecivedBytes < recvbuf_len ) {                      // while number of read bytes is less than specified number
                                                                    // receive data from a connected socket
        iResult = recv( socket, recvbuf + numOfRecivedBytes, recvbuf_len - numOfRecivedBytes, 0 );
        if( iResult <= 0 )                                          // if error of receiving - return -1
            return -1;
        numOfRecivedBytes += iResult;                               // add the number of reading bytes
    }
    return numOfRecivedBytes;                                       // success - return number of receiving bytes
}

//----------------------------------------------------------------------------------------------------------------------------------------
int main() {
    //----------------------
    // Create a SOCKET for connecting to server
    int ConnectSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if( ConnectSocket <= 0 ) {
        cout << "Socket function failed with error" << endl;
        return 1;
    }

    //----------------------
    // The sockaddr_in structure specifies the address family,
    // IP address, and port of the server to be connected to.
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr( DEFAULT_ADDR );
    clientService.sin_port = htons( DEFAULT_PORT );

    //----------------------
    // Connect to server.
    int iResult = connect( ConnectSocket, (const struct sockaddr *) &clientService, sizeof (clientService) );
    if( iResult <= 0 ) {
        cout << "Connect function failed with error" << endl;
        iResult = close( ConnectSocket );
        if( iResult <= 0 )
            cout << "Close socket function failed with error" << endl;
        return 1;
    }

    cout << "Connected with " << DEFAULT_ADDR << endl;

    //----------------------
    // Declare different variables
    char choice;                        // choice of the user
    char tmp[] = "exit";                // message that sends to the server when we close client
    char message[DEFAULT_BUFLEN];       // variable for messages from client to server

    // Print list of commands
    cout << "You can: " << endl <<
                "0: Commands" << endl <<
                "1: send message to server" << endl <<
                "2: exit" << endl;

    //------------------------------------------------------------------------------------------------------------------------------------
    bool working = true;
    while( working ) {
        cin >> choice;
        switch( choice ) {
        //--------------------------------------------------------------------------------------------------------------------------------
        case '0':                                                   // case 0: print commands
            cout << "You can: " << endl <<
                    "0: Commands" << endl <<
                    "1: send message to server" << endl <<
                    "2: exit" << endl;
            break;
        //--------------------------------------------------------------------------------------------------------------------------------
        case '1':                                                   // case 1: sending message to server
            char answer[DEFAULT_BUFLEN];
            //memset( message, 0, DEFAULT_BUFLEN );
            for( int i = 0; i < DEFAULT_BUFLEN; i++ )
                message[i] = 0;
            cout << "Enter your message(max size " << DEFAULT_BUFLEN-1 << "):";// << endl;
            cin >> message;
            iResult = send( ConnectSocket, message, DEFAULT_BUFLEN, 0 );
            if( iResult <= 0) {
                cout << "Send failed with error" << endl;
                working = false;
                break;
            }
            iResult = readn( (int &)ConnectSocket, answer, DEFAULT_BUFLEN );
            if( iResult <= 0 ){
                cout << "Receiving answer failed with error" << endl;
                working = false;
                break;
            }
            cout << "Server's answer:" << answer << endl;
            break;
        //--------------------------------------------------------------------------------------------------------------------------------
        case '2':                                                   // case 2: turn off client
            iResult = send( ConnectSocket, tmp, DEFAULT_BUFLEN, 0 );      // Sending message about disconnection
            if( iResult <= 0 ){
                cout << "Send failed with error" << endl;
            }
            working = false;
            break;
        //--------------------------------------------------------------------------------------------------------------------------------
        default:
            cout << "Can't understand your choice. Try again." << endl;
        }
    }
    //------------------------------------------------------------------------------------------------------------------------------------
    cout << "Closing client..." << endl;
    sleep( 1000 );
    shutdown( ConnectSocket, SHUT_RDWR );
    close( ConnectSocket );
    return 0;
}
