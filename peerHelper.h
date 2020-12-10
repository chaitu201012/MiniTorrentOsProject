#ifndef PEERHELPER
#define PEERHELPER

#include "headerfiles_global_vars.h"




void* copyToOuput(void * pargs){

    // receive fileName and port number of peer server listener and send it to listener 
    //and recceive buffer(data from file and chunk number to write to the file)

    filePort *pf=(filePort*) pargs;
    char Message[1]={0},flag='a';
    char bufChunks[200],fileName[200],hashRecvBuff[200],outfile[200];
    string chunkinfo="";
    int peerport=pf->port;
    int clport=pf->clPort;
    strcpy(fileName,pf->fileName);
    strcpy(outfile,pf->outfile);
    chunkinfo=pf->chunkinfo;
    strcpy(bufChunks,chunkinfo.c_str());

    
    int chunkNumber;
    char receiveBUF[CHUNKSIZE*BUFFERSIZE];
    
    //vecs cunks;
    //cunks=tokeniseCommand2(chunkinfo);
    struct sockaddr_in  peerAddress={0};
	peerAddress.sin_family = AF_INET;
	peerAddress.sin_port = htons( peerport );
	peerAddress.sin_addr.s_addr=inet_addr("127.0.0.1");

    int peerSockDesc = socket( AF_INET, SOCK_STREAM, 0 );
    //cout<<"clientSocketDESC  "<<peerSockDesc<<endl;
    int con=connect ( peerSockDesc, (struct sockaddr *)&peerAddress  , sizeof(struct sockaddr_in));
    if(con<0){
        cout<<"Connection failed\n"<<con;
        exit(EXIT_FAILURE);
    }
    
   // if(logged_ports.find(peerport)!=logged_ports.end()){
        
        send(peerSockDesc,&flag,sizeof(flag),0);
        recv(peerSockDesc,&Message,sizeof(Message),0);

        send(peerSockDesc,fileName,sizeof(fileName),0);
        recv(peerSockDesc,&Message,sizeof(Message),0);

        send(peerSockDesc,bufChunks,sizeof(bufChunks),0);
        recv(peerSockDesc,&Message,sizeof(Message),0);

        //recv(peerSockDesc,bufChunks,sizeof(bufChunks),0);
        
        vecs chunks;
        chunks=tokeniseCommand2(string(bufChunks));

        fileWithTracker kf;
        kf.port=clport;
        strcpy(kf.ip,"127.0.0.1");
        strcpy(kf.fileName,fileName);

        FILE *fp=fopen(outfile,"wb");


       ///#############################  PIECE SELECTION Algorithm #####################
       cout<<"--------------Downloading-----------\n";
       cout<<"lenght of chunks "<<len(chunks)<<endl;
        for(int i=0;i<len(chunks);++i){
            // cout<<chunks[i]<<" "<<chunks[i+1]<<" "<<chunks[i+2]<<" \n";
            chunkNumber=stoi(chunks[i]);
            cout<<"Getting data chunk :"<<chunkNumber<<" from peer "<<peerport<<endl;
            send(peerSockDesc,&chunkNumber,sizeof(chunkNumber),0);
            recv(peerSockDesc,receiveBUF,sizeof receiveBUF,0);
            send(peerSockDesc,&Message,sizeof Message,0);
            fwrite(receiveBUF,sizeof(char),CHUNKSIZE*BUFFERSIZE,fp);
            kf.noOfChunks+=1;
            kf.chunkids+=to_string(chunkNumber)+DELIMITER2;
            memset(receiveBUF,0,CHUNKSIZE*BUFFERSIZE);
            fflush(stdout);   
        }


        //calculate sha before pushingback to vector and check for integrity
        filesAvailable.push_back(kf);
        fclose(fp);
        sleep(0.5);
}


void* client_func(void *pargs){

    //cout<<"saitama client\n";
    clientInfo *one_client=(clientInfo *)pargs;
    int clport=one_client->port;
    char* ipaddress=one_client->ip;
    char* track_info_file=one_client->trackFile;
    int acceptingport;

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp=fopen(track_info_file,"r");

    if(fp==NULL){
        cout<<"FILE DOESNOT EXIST\n";
        return NULL;
    }


    char ip_addr1[32];
    char ip_addr2[32];
    int trackerPort1;
    int trackerPort2;

    read=getline(&line,&len,fp);
    strcpy(ip_addr1,line);
    read=getline(&line,&len,fp);
    trackerPort1=atoi(line);
    

    read=getline(&line,&len,fp);
    strcpy(ip_addr2,line);
    read=getline(&line,&len,fp);
    trackerPort2=atoi(line);



    ip_addr1[strlen(ip_addr1)-1]='\0';
    ip_addr2[strlen(ip_addr2)-1]='\0';

    
    cout<<"client Connected::"<<clport<<endl;

    while(1){
        //*******************************Local Variables used for requests send to server*************************************
        char msg[200];
        char flag;
        int group_id;
        int ack;
        char command[100],buf1[70],buf2[200],buf3[200],buf4[200],hashRecvBuff[200];
        char tokenised_command[4][100];
        char serverMessage[100]={0};
        char Message[1]={0};
        char clientMessage[100]={0};
        long fileSize=0;
        string filePath;
        int noOfChunks=0;
        char peers[200];
        //long fileSize;
        int portPeers[20];


        //***************************************************************************************************************************

        cout<<"Enter the command::\n";
        scanf("%[^\n]%*c",command);
        //fflush(stdin);
        int count=0;
        char* token=strtok(command," ");
        while (token!=NULL) {
            strcpy(tokenised_command[count++],token);
            token=strtok(NULL," ");
        }
        strcpy(buf1,tokenised_command[0]);
        memset(clientMessage, '\0', sizeof clientMessage);//buffer for sending
        memset(Message, '\0', sizeof Message);
        

        //int clientSockDesc = 0, read_size = 0;

        char server_reply[200] = {0};
        

        struct sockaddr_in  clientAddress={0};
		clientAddress.sin_family = AF_INET;
		clientAddress.sin_port = htons( trackerPort1 );
		clientAddress.sin_addr.s_addr=inet_addr(ip_addr1);

        int clientSockDesc = socket( AF_INET, SOCK_STREAM, 0 );
        //cout<<"clientSocketDESC  "<<clientSockDesc<<endl;
        int con=connect ( clientSockDesc, (struct sockaddr *)&clientAddress  , sizeof(struct sockaddr_in));
		if(con<0){
            cout<<"Connection failed\n"<<con;
            exit(EXIT_FAILURE);
        }
        

        if(strcmp(buf1,"create_user")==0){
            strcpy(buf2,tokenised_command[1]);
            strcpy(buf3,tokenised_command[2]);
            //cout<<"Entering create user\n";
            //cout<<"tokens  "<<buf1<<" "<<buf2<<" "<<buf3<<endl;
            if(count<3){
                cout<<"Invalid command\n";
                continue;
            }
            flag='a';
            
            
            if(send(clientSockDesc,&flag,sizeof(flag),0)<0){
                cout<<"Error sending data\n";
                exit(EXIT_FAILURE);
            }
            send(clientSockDesc,&clport,sizeof(clport),0);
            send(clientSockDesc,ipaddress,strlen(ipaddress),0);
            recv(clientSockDesc,&ack,sizeof(ack),0);
            send(clientSockDesc,buf2,strlen(buf2),0);
			recv(clientSockDesc,&ack,sizeof(ack),0);
			send(clientSockDesc,buf3,strlen(buf3),0);
			recv(clientSockDesc,&ack,sizeof(ack),0);
            //cout<<"ack from server "<<*ack_from_server<<endl;
            memset(serverMessage, '\0', sizeof serverMessage);
            recv(clientSockDesc,&serverMessage,sizeof serverMessage,0);
            cout<<"server response "<<serverMessage<<endl;
            memset(serverMessage, '\0', sizeof serverMessage);  
            fflush(stdout);         


        }
        else if(strcmp(buf1,"login")==0){
            strcpy(buf2,tokenised_command[1]);
            strcpy(buf3,tokenised_command[2]);
            if(count<3){
                cout<<"INvalid command\n";
                continue;
            }
            flag ='b';
            //char Message[1]={0};
            //char clientMessage[100]={0};
            memset(clientMessage, '\0', sizeof clientMessage);//buffer for sending
            memset(Message, '\0', sizeof Message);
            strcpy(clientMessage,prepareBuffer(buf2,buf3).c_str());

            if(send(clientSockDesc,&flag,sizeof(flag),0)<0){
                cout<<"Error sending data\n";
                exit(EXIT_FAILURE);
            }
            send(clientSockDesc,&clport,sizeof(clport),0);
            send(clientSockDesc,ipaddress,strlen(ipaddress),0);
            recv(clientSockDesc,&Message,sizeof(Message),0);

            send(clientSockDesc,&clientMessage,100,0);
            recv(clientSockDesc,&Message,sizeof Message,0);

            memset(serverMessage, '\0', sizeof serverMessage);
            recv(clientSockDesc,&serverMessage,sizeof serverMessage,0);
            cout<<"server response "<<serverMessage<<endl;
            memset(serverMessage, '\0', sizeof serverMessage);  
            fflush(stdout);         

        }

        else if(strcmp(buf1,"create_group")==0){
            strcpy(buf2,tokenised_command[1]);
            group_id=atoi(buf2);
            //cout<<"group_id "<<group_id<<endl;
            if(count<2){
                cout<<"Invalid command\n";
                continue;
            }
            flag='c';
            if(send(clientSockDesc,&flag,sizeof(flag),0)<0){
                cout<<"Error sending data\n";
                exit(EXIT_FAILURE);
            }
            send(clientSockDesc,&clport,sizeof(clport),0);
            send(clientSockDesc,ipaddress,strlen(ipaddress),0);
            recv(clientSockDesc,&Message,sizeof(Message),0);
            send(clientSockDesc,&group_id,sizeof group_id,0);

            memset(serverMessage, '\0', sizeof serverMessage);
            recv(clientSockDesc,&serverMessage,sizeof serverMessage,0);
            cout<<"server response "<<serverMessage<<endl;
            memset(serverMessage, '\0', sizeof serverMessage);  
            fflush(stdout); 

        }
        else if(strcmp(buf1,"join_group")==0){
            strcpy(buf2,tokenised_command[1]);
            group_id=atoi(buf2);
            if(count<2){
                cout<<"Invalid command\n";
                continue;
            }
            flag='d';
            //cout<<"group_id "<<group_id<<endl;
            if(send(clientSockDesc,&flag,sizeof(flag),0)<0){
                cout<<"Error sending data\n";
                exit(EXIT_FAILURE);
            }
            send(clientSockDesc,&clport,sizeof(clport),0);
            send(clientSockDesc,ipaddress,strlen(ipaddress),0);
            recv(clientSockDesc,&Message,sizeof(Message),0);
            send(clientSockDesc,&group_id,sizeof group_id,0);

            recv(clientSockDesc,&acceptingport,sizeof acceptingport,0);
            send(clientSockDesc,&Message,sizeof Message,0);

            recv(clientSockDesc,&serverMessage,sizeof serverMessage,0);
            cout<<"server response "<<serverMessage<<endl;


            memset(serverMessage, '\0', sizeof serverMessage);
            
            fflush(stdout); 

        }
        else if(strcmp(buf1,"leave_group")==0){
            strcpy(buf2,tokenised_command[1]);
            group_id=atoi(buf2);
            //cout<<"group_id "<<group_id<<endl;
            if(count<2){
                cout<<"Invalid command\n";
                continue;
            }
            flag='e';
            if(send(clientSockDesc,&flag,sizeof(flag),0)<0){
                cout<<"Error sending data\n";
                exit(EXIT_FAILURE);
            }
            send(clientSockDesc,&clport,sizeof(clport),0);
            send(clientSockDesc,ipaddress,strlen(ipaddress),0);
            recv(clientSockDesc,&Message,sizeof(Message),0);
            send(clientSockDesc,&group_id,sizeof group_id,0);

            memset(serverMessage, '\0', sizeof serverMessage);
            recv(clientSockDesc,&serverMessage,sizeof serverMessage,0);
            cout<<"server response "<<serverMessage<<endl;
            memset(serverMessage, '\0', sizeof serverMessage);  
            fflush(stdout); 

        }
        else if(strcmp(buf1,"upload_file")==0){
            if(count<3){
                cout<<"Invalid command\n";
                continue;
            }
            flag='l';
            strcpy(buf2,tokenised_command[1]);
            //filePath=string(buf2);
            strcpy(buf3,tokenised_command[2]);
            group_id=atoi(buf3);
            memset(clientMessage, '\0', sizeof clientMessage);//buffer for sending
            memset(Message, '\0', sizeof Message);
            strcpy(clientMessage,prepareBuffer(buf2,buf3).c_str());

            if(send(clientSockDesc,&flag,sizeof(flag),0)<0){
                cout<<"Error sending data\n";
                exit(EXIT_FAILURE);
            }
            send(clientSockDesc,&clport,sizeof(clport),0);
            send(clientSockDesc,ipaddress,strlen(ipaddress),0);
            recv(clientSockDesc,&Message,sizeof(Message),0);

            send(clientSockDesc,&group_id,sizeof group_id,0);
            recv(clientSockDesc,&Message,sizeof(Message),0);

            send(clientSockDesc,&clientMessage,100,0);
            recv(clientSockDesc,&Message,sizeof Message,0);

            //*******************calculating file Size****************************

            fileSize=findSize(buf2);
            noOfChunks=ceil(fileSize/(CHUNKSIZE*BUFFERSIZE));
            cout<<"number of chunks "<<noOfChunks<<endl;
            if(noOfChunks==0) noOfChunks=1;


            string hash1;
            cout<<"------------Calculating sha value for chunks of size 20 bytes-----------\n";

            hash1=calculateSHA1(buf2);
            cout<<"hash_value : "<<hash1<<endl;
            sleep(1);
            strcpy(buf4,hash1.c_str());

            send(clientSockDesc,&fileSize,sizeof fileSize,0);
            recv(clientSockDesc,&Message,sizeof(Message),0);

            send(clientSockDesc,&noOfChunks,sizeof noOfChunks,0);
            recv(clientSockDesc,&Message,sizeof(Message),0);

            send(clientSockDesc,buf4,200,0);   //making hash size as 200 fixed for all the files 
            recv(clientSockDesc,&Message,sizeof(Message),0);




            memset(serverMessage, '\0', sizeof serverMessage);
            recv(clientSockDesc,&serverMessage,sizeof serverMessage,0);
            cout<<"server response "<<serverMessage<<endl;
            memset(serverMessage, '\0', sizeof serverMessage);  
            fflush(stdout);



        }
        else if(strcmp(buf1,"download_file")==0){
            if(count<4){
                cout<<"Invalid command\n";
                continue;
            }
            flag='m';       
            strcpy(buf2,tokenised_command[1]);
            //filePath=string(buf2);
            strcpy(buf3,tokenised_command[2]);
            group_id=atoi(buf2);
            strcpy(buf4,tokenised_command[3]);
            //cout<<buf1<<" "<<buf2<<" "<<buf3<<" "<<buf4<<endl;

            if(send(clientSockDesc,&flag,sizeof(flag),0)<0){
                cout<<"Error sending data\n";
                exit(EXIT_FAILURE);
            }
            
            if(send(clientSockDesc,&clport,sizeof(clport),0)<0){
                 cout<<"Port error"; 
                 exit(EXIT_FAILURE);
            }
            if(send(clientSockDesc,ipaddress,strlen(ipaddress),0)<0){
                cout<<"ip error"; 
                 exit(EXIT_FAILURE);
            }
            if(recv(clientSockDesc,&Message,sizeof(Message),0)<0){
                cout<<"Message receive error\n";
                exit(EXIT_FAILURE);
            }

            //cout<<"first  ";

            send(clientSockDesc,&group_id,sizeof group_id,0);
            recv(clientSockDesc,&Message,sizeof(Message),0);

            sleep(0.5);

            //cout<<"second  ";

            send(clientSockDesc,buf3,sizeof buf3,0);
            recv(clientSockDesc,&Message,sizeof(Message),0);

            //cout<<"third  ";

            recv(clientSockDesc,peers,sizeof peers,0);
            send(clientSockDesc,&Message,sizeof(Message),0);

            //cout<<"fourth  ";
            
            vector<string> peerWithFiles;
            if(strcmp(peers,"No peers available for this file")==0 || strlen(peers)==0){
                cout<<"No peers available for this file\n";
                continue;
            }
            else{
                peerWithFiles=tokeniseCommand(peers);
                for(auto i:peerWithFiles){
                    cout<<i<<" ";    
                }
                //vecs ;
                /*
                for(int i=0;i<len(peerWithFiles;i++)){
                    portchunks=tokeniseCommand2(peerWithFiles[i]);
                }
                */
                recv(clientSockDesc,&fileSize,sizeof fileSize,0);
                send(clientSockDesc,&Message,sizeof(Message),0);

                recv(clientSockDesc,hashRecvBuff,sizeof hashRecvBuff,0);
                send(clientSockDesc,&Message,sizeof Message,0);

                string outfile=string(buf4)+"output.txt";
                FILE *fpt=fopen(outfile.c_str(),"wb");
                if(fpt==NULL)
                {
                    cout<<"Error opening file\n";
                }
                long X=fileSize-1;
                //fpt=fopen("4.txt","wb"); 
                fseek(fpt, X , SEEK_SET);
                fputc('\0', fpt);
                fclose(fpt);
                pthread_t thread_id[len(peerWithFiles)];

                cout<<"Downloading started................\n";
                //cout<<"logged_ports "<<endl;
                //for(auto i:logged_ports){
                //    cout<<i<<" ";
                //}
                strcpy(buf4,outfile.c_str());
                filePort pf;
                for(int i=0;i<len(peerWithFiles);i++){
                    portPeers[i]=stoi(peerWithFiles[i].substr(0,4));
                    strcpy(pf.fileName,buf3);
                    pf.port=portPeers[i];
                    pf.clPort=clport;
                    strcpy(pf.outfile,outfile.c_str());
                    pf.chunkinfo=peerWithFiles[i].substr(5);
                    pthread_create(&thread_id[i],NULL,copyToOuput,(void*)&pf);

                }
                for(int i=0;i<len(peerWithFiles);i++)
					pthread_join(thread_id[i],NULL);
                sleep(1);
                cout<<"....";
                sleep(1);
                cout<<"............";
                sleep(1);
                cout<<"................................\n";
                cout<<"-----------Download Complete-----------------------\n";
                string hash1;
                
                hash1=calculateSHA1(buf4);
                strcpy(buf4,hash1.c_str());
                if(strcmp(buf4,hashRecvBuff)==0){
                    cout<<"file integrity is preserved and all chunks are downloaded \n";

                }
                else{
                    cout<<"hash downloaded file "<<hash1<<endl;
                    cout<<"hash uploaded file "<< hashRecvBuff<<endl;
                    cout<<"file integrity is not preserved (or) some chunks may be missing \n";
                }
                
            }

            
        }
        else if(strcmp(buf1,"requests")==0){
            if(count<3){
                cout<<"Invalid command\n";
                continue;
            }
            flag='f';       
            strcpy(buf2,tokenised_command[1]);
            //filePath=string(buf2);
            strcpy(buf3,tokenised_command[2]);
            group_id=atoi(buf3);
            //strcpy(buf4,tokenised_command[3]);
            //cout<<buf1<<" "<<buf2<<" "<<buf3<<endl;

            if(send(clientSockDesc,&flag,sizeof(flag),0)<0){
                cout<<"Error sending data\n";
                exit(EXIT_FAILURE);
            }
            
            if(send(clientSockDesc,&clport,sizeof(clport),0)<0){
                 cout<<"Port error"; 
                 exit(EXIT_FAILURE);
            }
            if(send(clientSockDesc,ipaddress,strlen(ipaddress),0)<0){
                cout<<"ip error"; 
                 exit(EXIT_FAILURE);
            }
            if(recv(clientSockDesc,&Message,sizeof(Message),0)<0){
                cout<<"Message receive error\n";
                exit(EXIT_FAILURE);
            }

            //cout<<"first  ";

            send(clientSockDesc,&group_id,sizeof group_id,0);
            recv(clientSockDesc,&Message,sizeof(Message),0);

            send(clientSockDesc,buf2,sizeof buf2,0);
            recv(clientSockDesc,&Message,sizeof(Message),0);

            sleep(0.5);

            


            cout<<"list requests for joing  for group "<< group_id<<endl;
            while(recv(clientSockDesc,msg,sizeof msg,0)>0){

            //memset(receiveBUF,0,CHUNKSIZE*BUFFERSIZE);
                cout<<msg<<endl;
            
                send(clientSockDesc,&Message,sizeof(Message),0);
                
            }
            recv(clientSockDesc,&serverMessage,sizeof(serverMessage),0);
            cout<<"server response "<<serverMessage<<endl;

        }
        else if(strcmp(buf1,"list_groups")==0){
            if(count<1){
                cout<<"Invalid command\n";
                continue;
            }
            flag='h';
            if(send(clientSockDesc,&flag,sizeof(flag),0)<0){
                cout<<"Error sending data\n";
                exit(EXIT_FAILURE);
            }
            send(clientSockDesc,&clport,sizeof(clport),0);
            send(clientSockDesc,ipaddress,strlen(ipaddress),0);
            recv(clientSockDesc,&Message,sizeof(Message),0);

            
             while(recv(clientSockDesc,msg,sizeof msg,0)>0){

            //memset(receiveBUF,0,CHUNKSIZE*BUFFERSIZE);
                cout<<msg<<endl;
            
                send(clientSockDesc,&Message,sizeof(Message),0);
                
            }

            recv(clientSockDesc,&serverMessage,sizeof(serverMessage),0);
            cout<<"server response "<<serverMessage<<endl;
        }
        else if(strcmp(buf1,"list_files")==0){
            if(count<2){
                cout<<"Invalid command\n";
                continue;
            }

            flag='n';

            strcpy(buf2,tokenised_command[1]);
            group_id=atoi(buf2);

            if(send(clientSockDesc,&flag,sizeof(flag),0)<0){
                cout<<"Error sending data\n";
                exit(EXIT_FAILURE);
            }
            send(clientSockDesc,&clport,sizeof(clport),0);
            send(clientSockDesc,ipaddress,strlen(ipaddress),0);
            recv(clientSockDesc,&Message,sizeof(Message),0);

            send(clientSockDesc,&group_id,sizeof(group_id),0);
            recv(clientSockDesc,&Message,sizeof(Message),0);

            
             while(recv(clientSockDesc,msg,sizeof msg,0)>0){

            //memset(receiveBUF,0,CHUNKSIZE*BUFFERSIZE);
                cout<<msg<<endl;
            
                send(clientSockDesc,&Message,sizeof(Message),0);
                
            }

            recv(clientSockDesc,&serverMessage,sizeof(serverMessage),0);
            cout<<"server response "<<serverMessage<<endl;
        }
        else if(strcmp(buf1,"accept_request")==0){
            strcpy(buf2,tokenised_command[1]);
            group_id=atoi(buf2);
            if(count<3){
                cout<<"Invalid command\n";
                continue;
            }
            strcpy(buf3,tokenised_command[2]);
            flag='g';
           // cout<<"group_id "<<group_id<<endl;
            if(send(clientSockDesc,&flag,sizeof(flag),0)<0){
                cout<<"Error sending data\n";
                exit(EXIT_FAILURE);
            }
            send(clientSockDesc,&clport,sizeof(clport),0);
            send(clientSockDesc,ipaddress,strlen(ipaddress),0);
            recv(clientSockDesc,&Message,sizeof(Message),0);
            send(clientSockDesc,&group_id,sizeof group_id,0);

            send(clientSockDesc,buf3,sizeof buf3,0);
            recv(clientSockDesc,&Message,sizeof(Message),0);

            memset(serverMessage, '\0', sizeof serverMessage);
            recv(clientSockDesc,&serverMessage,sizeof serverMessage,0);
            cout<<"server response "<<serverMessage<<endl;
            memset(serverMessage, '\0', sizeof serverMessage);  
            fflush(stdout); 

        }
        else if (strcmp(buf1,"logout")==0){
            if(count<1){
                cout<<"Invalid command\n";
                continue;

            }
            flag='o';
            if(send(clientSockDesc,&flag,sizeof(flag),0)<0){
                cout<<"Error sending data\n";
                exit(EXIT_FAILURE);
            }
            send(clientSockDesc,&clport,sizeof(clport),0);
            send(clientSockDesc,ipaddress,strlen(ipaddress),0);
            recv(clientSockDesc,&Message,sizeof(Message),0);recv(clientSockDesc,&serverMessage,sizeof serverMessage,0);
            cout<<"server response "<<serverMessage<<endl;
            memset(serverMessage, '\0', sizeof serverMessage);  
            fflush(stdout);
        }

        memset(buf1,0,sizeof(buf1));
        memset(buf2,0,sizeof(buf2));
        memset(buf3,0,sizeof(buf3));
        memset(buf4,0,sizeof(buf4));
        close(clientSockDesc);
        cout<<"request completed\n";
    
    }
}



#endif //PEERHELPER