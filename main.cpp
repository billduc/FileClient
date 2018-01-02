/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: hydra
 *
 * Created on December 28, 2017, 3:55 PM
 */
#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <netdb.h>
#include <cerrno>

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    int socketfd, port, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    
    char buffer[256];
    
    //check args
    if (argc < 3){
        cerr << "usage: "<< argv[0] << " <hostname> <port>" << endl;
        exit(EXIT_FAILURE);
    }
    
    port = atoi(argv[2]);
    
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (socketfd < 0){
        cerr << "ERROR create socket" << endl;
        exit(EXIT_FAILURE);
    }
    
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port);
    if (connect(socketfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        cerr << "ERROR connecting" << endl;
    
    printf("Server connected \n");
    
    char cmd[] = "upload untitled.c";
    
    send (socketfd, cmd, sizeof(cmd), 0);
    
    recv(socketfd, buffer, sizeof(buffer), 0 );
    cout <<"server answer: " << buffer << endl;
    
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    
    //n = write(socketfd, buffer, strlen(buffer));
    
    string filepath = "/home/hydra/Desktop/untitled.c";
    
    FILE * fp = fopen(filepath.c_str(), "r");
    fseek(fp, 0, SEEK_END);
    
    int fs_block_sz;
    while ((fs_block_sz == fread(buffer , sizeof(char), 256, fp )) > 0 ){
        if (send(socketfd, buffer, fs_block_sz,0) < 0){
            cerr << "ERROR: Failed to send file " << filepath <<". (errno = " << errno << ")\n";
            break;
        }
        bzero(buffer, 256);
    }
    
    printf("Ok File %s from Client was Sent!\n", filepath);
    
    long FileSize = ftell(fp);
    
    cout << FileSize << endl;
    
    
    close(socketfd);
    
    return 0;
}

