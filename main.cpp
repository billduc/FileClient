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

#include "header/md5.h"

using namespace std;

#define MAX_SIZE 409600
/*
 * 
 */

string getFileName(string filepath){
    int post = -1;
    //linux operator
    for(int i = sizeof(filepath) - 1; i >=0; --i){
        if (filepath[i] == '/')
        {
            post = i;
            break;
        }
    }
    if (post == -1)
        return string(filepath);
    else{
        //cout << post  <<" " << filepath << " " << sizeof(filepath)<< " " << filepath.length() - post << endl;
        string filename = filepath.substr(post+1, filepath.length() - post);
        return filename;
    }
}

int main(int argc, char** argv) {
    
    cout << md5("trungduc") << endl;
    int socketfd, port, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    
   
    char buffer[MAX_SIZE];
    
    //check args
    if (argc < 3){
        cerr << "usage: "<< argv[0] << " <hostname> <port> <file path>" << endl;
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

    //auth user agent
    string md5code = md5("test");
    send(socketfd, md5code.c_str(), md5code.length(), 0);

    //confirm form server
    bool okConn = false;
    recv(socketfd, buffer, sizeof(buffer), 0);
    if (strcmp(buffer, "200 ok") == 0){
        cout << "auth ok" << endl;
        okConn = true;
    }
    else{
        cout << "fail connection" << endl; 
    }

    if (okConn){
    
    string filepath = argv[3];
    //cout << filepath << endl;
    cout << "file name: " <<  getFileName(filepath) << endl;
    
    string cmd = "upload " + getFileName(filepath);
    
    send (socketfd, cmd.c_str(), cmd.length(), 0);
    
    recv(socketfd, buffer, sizeof(buffer), 0 );
    cout <<"server answer: " << buffer << endl;
    
    bzero(buffer,MAX_SIZE);
    
    //n = write(socketfd, buffer, strlen(buffer));
    
    //string filepath = "/home/hydra/Desktop/untitled.c";
    
    //string filepath = argv[3];
    
    FILE * fp = fopen(filepath.c_str(), "r");
    
    if(fp == NULL)
    {
        printf("ERROR: File %s not found.\n", filepath);
        exit(1);
    }
    
    //fseek(fp, 0, SEEK_END);
    bzero(buffer, sizeof buffer); 
    
    int fs_block_sz;
    while ( (fs_block_sz = fread(buffer , sizeof(char), MAX_SIZE, fp ) ) > 0 ){
        cout << "send file " << fs_block_sz << endl;
        if (send(socketfd, buffer, fs_block_sz,0) < 0){
            cerr << "ERROR: Failed to send file " << filepath <<". (errno = " << errno << ")\n";
            break;
        }
        bzero(buffer, MAX_SIZE);
    }
    
    printf("Ok File %s from Client was Sent!\n", filepath);
    
    long FileSize = ftell(fp);
    
    cout << FileSize << endl;
    
    }

    
    close(socketfd);
    
    return 0;
}

