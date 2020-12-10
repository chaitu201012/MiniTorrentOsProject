#ifndef PEERHELPERSERVER
#define PEERHELPERSERVER

#include "headerfiles_global_vars.h"



void* handleDownloads(void * pargs){

    //int sock=*((int*)pargs);
    //cout<<"first checkpoint \n";
    peerSock  *ps=(peerSock*) pargs;

    char receiveBUF[CHUNKSIZE*BUFFERSIZE];
    
    int sock;
    char user_name[50];
    char pwd[50];
    char ip[32];
    int port,read;
    int ack=0;
    vecs cmd;
    int group_id;
    int chunkNumber;
    vector<int> chunks;
    char serverMessage[100]={0};

    char bufFile[200],bufDestPath[200],hashRecvBuff[200],bufChunks[200];

    string hashValue;
    int fileSize;
    int noOfChunks;
    char fileName[200]; 
    char Message[1]={0};

    sock=ps->sock;
    port=ps->port;

    char flag;
    recv(sock,&flag,sizeof(flag),0);
    send(sock,&Message,sizeof(Message),0);
    char clientMessage[100]={0};
    if(flag=='a'){
        memset(clientMessage, '\0', sizeof clientMessage);//buffer for sending
        memset(Message, '\0', sizeof Message);
        memset(serverMessage, '\0', sizeof serverMessage);
        

        //cout<<sock<<" "<<port<<endl;

        recv(sock,fileName,sizeof fileName,0);
        send(sock,&Message,sizeof(Message),0);

        recv(sock,bufChunks,sizeof(bufChunks),0);
        send(sock,&Message,sizeof(Message),0);

        vecs cunks;
        cunks=tokeniseCommand2(string(bufChunks));
        for(int i=0;i<len(cunks);i++){
            //cout<<cunks[i]<<" ";
            chunks.push_back(stoi(cunks[i]));
        }

        //cout<<fileName<<" second checkpoint"<<endl;
        string stchunk="";
        /*
        for(auto it=ports_with_chunks.begin();it!=ports_with_chunks.end();it++){
            if((it->first.first)==port){
            for(auto s:it->second){
                stchunk+=to_string(s)+DELIMITER;
                chunks.push_back(s);
            }

            }

        }
        */
        FILE *fp=fopen(fileName,"rb");
        memset(receiveBUF,0,CHUNKSIZE*BUFFERSIZE);
        while(recv(sock,&chunkNumber,sizeof chunkNumber,0)>0){

            //memset(receiveBUF,0,CHUNKSIZE*BUFFERSIZE);
                fseek(fp,(chunkNumber-1)*CHUNKSIZE*BUFFERSIZE,SEEK_SET);
                read=fread(receiveBUF,sizeof(char),CHUNKSIZE*BUFFERSIZE,fp);
                send(sock,receiveBUF,sizeof receiveBUF,0);
                recv(sock,&Message,sizeof Message,0);
                read=0;
                memset(receiveBUF,0,CHUNKSIZE*BUFFERSIZE);
        }
        fclose(fp);
        //cout<<stchunk<<endl;
        
        //strcpy(bufChunks,stchunk.c_str());
        //send(sock,bufChunks,sizeof(bufChunks),0);    

        /*
        for(int i=0;i<len(chunks);i++){

            chunkNumber=chunks[i];
            //memset(receiveBUF,0,CHUNKSIZE*BUFFERSIZE);
            fseek(fp,(chunks[0]-1)*CHUNKSIZE*BUFFERSIZE,SEEK_SET);
            read=fread(receiveBUF,sizeof(char),CHUNKSIZE*BUFFERSIZE,fp);
            send(sock,&chunkNumber,sizeof chunkNumber,0);
            recv(sock,&Message,sizeof Message,0);
            send(sock,receiveBUF,sizeof receiveBUF,0);
            recv(sock,&Message,sizeof Message,0);
            read=0;
            memset(receiveBUF,0,CHUNKSIZE*BUFFERSIZE);

        }
        */
    }
    else if(flag=='b'){//for  accepting joining request
        char msg[200],yes;  
            

        int clport,group_id;

        recv(sock,&clport,sizeof(clport),0);
        send(sock,&Message,sizeof(Message),0);

        recv(sock,&group_id,sizeof(group_id),0);
        send(sock,&Message,sizeof(Message),0);

        recv(sock,msg,sizeof(msg),0);
        send(sock,&Message,sizeof(Message),0);
        cout<<msg<<endl;

    }

    //cout<<chunkNumber<<" third checkpoint"<<endl;

    
    
    

}

void connectionEstablish(int serverSockDesc,struct sockaddr_in servAddress,int port){
    int sockAddrLen=sizeof(sockaddr);
    while(1){
        int sock=accept(serverSockDesc,(struct sockaddr *)&servAddress,(socklen_t *)&sockAddrLen);
        cout<<":: Connection Established ::"<<sock<<endl;
        pthread_t thread;
        peerSock ps;
        ps.sock=sock;
        ps.port=port;
         if(pthread_create(&thread,NULL,handleDownloads,(void *)&ps)){
            cout<<"Error creating thread for handle download services in tracker\n";
            continue;
        }
        
        pthread_join(thread,NULL);
        if(logged_ports.find(port)==logged_ports.end()){
        break;
     }
    }


}

void* serverListener(void *prags){

    int port = *((int*)prags);

    cout<<"This is for server listening\n";
    int serverSockDesc = socket (AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in  servAddress;
	servAddress.sin_family = AF_INET;
	servAddress.sin_port = htons( port );
	servAddress.sin_addr.s_addr=INADDR_ANY;

	if(bind (serverSockDesc  ,(struct sockaddr *)&servAddress , sizeof (servAddress) )<0){
        cout<<"bind failedd";
        exit(1);
    }
    
    cout<<"::LISTENING::\n";
    listen(serverSockDesc,50);

     connectionEstablish(serverSockDesc,servAddress,port);
     
	
}


#endif//PEERHELPERSERVER