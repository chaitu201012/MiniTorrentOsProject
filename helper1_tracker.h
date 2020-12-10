#ifndef  HELPER1
#define  HELPER1

#include "headerfiles_global_vars.h"

void* acceptJoinRequest(void * pargs){
    //flag='b'
    threadport *tp=(threadport*) pargs;
    int peerport=tp->accport;
    int clport=tp->clport;
    int group_id=tp->group_id;
    char flag='b',sendmessage[200],msg[200];
    char Message[1]={0};
    if(logged_ports.find(peerport)!=logged_ports.end()){
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
        char flag1='b',yes;
        send(peerSockDesc,&flag1,sizeof(flag1),0);
        recv(peerSockDesc,&Message,sizeof(Message),0);

        send(peerSockDesc,&clport,sizeof(clport),0);
        recv(peerSockDesc,&Message,sizeof(Message),0);

        send(peerSockDesc,&group_id,sizeof(group_id),0);
        recv(peerSockDesc,&Message,sizeof(Message),0);
        string temp="You have a new  joining request" + to_string(clport)+" for group_id "+ to_string(group_id);
        strcpy(msg,temp.c_str());
        send(peerSockDesc,msg,sizeof(msg),0);
        recv(peerSockDesc,&Message,sizeof(Message),0);

    }
}

void* handleServices(void * pars){
    char user_name[50];
    char pwd[50];
    char ip[32];
    int port;
    int ack=0;
    vecs cmd;
    int group_id;
    char serverMessage[100]={0};

    char bufFile[200],bufDestPath[200],hashRecvBuff[200];

    string hashValue;
    long fileSize;
    int noOfChunks;
    string fileName;

    cout<<"Handling  your requests\n";

    int sock=*((int*)pars);
    char flag;
    char clientMessage[100]={0};
    char Message[1]={0};
    memset(clientMessage, '\0', sizeof clientMessage);//buffer for sending
    memset(Message, '\0', sizeof Message);
    memset(serverMessage, '\0', sizeof serverMessage);
    recv(sock,&flag,sizeof(flag),0);
    //cout<<"flag "<<flag<<endl;

    switch(flag){

        case 'a':

            cout<<"creating_user\n";
            recv(sock,&port,sizeof(port),0);
            recv(sock,ip,sizeof(ip),0);
            send(sock,&ack,sizeof(ack),0);
            recv(sock,user_name,sizeof(user_name),0);
            send(sock,&ack,sizeof(ack),0);
            recv(sock,pwd,sizeof(pwd),0);
            send(sock,&ack,sizeof(ack),0);
            //cout<<ip<<" "<<port<<" "<<user_name<<" "<<pwd<<endl;
            userInfo ui;
            strcpy(ui.user_name,user_name);
            strcpy(ui.pwd,pwd);
            strcpy(ui.ip,ip);
            ui.port=port;
            user_number++;

            //map_user_info.find(string(user_name));

            if(map_user_info.find(string(user_name))!=map_user_info.end()){
                cout<<"user::"<<user_name<<" already created\n";
                strcpy(serverMessage,"User already created::");
            }
            else{
                map_user_info.insert({string(ui.user_name),ui});
                strcpy(serverMessage,"User created successfully::");
            }
            //map_user_info.insert({string(ui.user_name),ui});
            send(sock,&serverMessage,sizeof(serverMessage),0);

            break;

        case 'b':

            recv(sock,&port,sizeof port,0);
            recv(sock,ip,sizeof(ip),0);
            send(sock,&Message,sizeof(Message),0);
            recv(sock,clientMessage,100,0);
            send(sock,&Message,sizeof(Message),0);

            
            cmd=tokeniseCommand(clientMessage);

            strcpy(user_name,cmd[0].c_str());
            strcpy(pwd,cmd[1].c_str());
            cout<<user_name<<" "<<pwd<<endl;

            //auto it1=login.find(string(user_name));

            if(logged_ports.find(port)!=logged_ports.end()){
                cout<<"User::"<<user_name<<" already logged in\n";
                strcpy(serverMessage,"::User already logged in::");
            }
            else{
                login.insert(string(user_name));
                logged_ports.insert(port);
                cout<<"port logged in "<<port<<endl;
                //cout<<"logged_ports "<<endl;
                //for(auto i:logged_ports){
                //    cout<<i<<" ";
                //}
                strcpy(serverMessage,"::User  logged in::");
            }
            send(sock,&serverMessage,sizeof(serverMessage),0);

            memset(clientMessage,0,sizeof(clientMessage));
			fflush(stdout);

            break;

        case 'c':
            //int group_id;
            cout<<"create_group\n";
            recv(sock,&port,sizeof port,0);
            recv(sock,ip,sizeof(ip),0);
            send(sock,&Message,sizeof(Message),0);
            recv(sock,&group_id,sizeof group_id,0);
            //cout<<"group_id "<<group_id<<endl;
            if(groupids.find(group_id)!=groupids.end()){
                cout<<"group id already exist\n";
                strcpy(serverMessage,"::Group already created::");
            }
            else{
                if(logged_ports.find(port)!=logged_ports.end()){
                    groupids.insert(group_id);
                    groups.insert({port,group_id});
                    owners.insert({group_id,port});
                    cout<<"Group created\n";
                    strcpy(serverMessage,"::Group created :: and you are joined in it");
                }
                else{
                    cout<<"You need to login first to create the group\n";
                    strcpy(serverMessage,"::Login first to  create groups ::");
                }
            }
            send(sock,&serverMessage,sizeof(serverMessage),0);

            memset(clientMessage,0,sizeof(clientMessage));
			fflush(stdout);

            break;

        case 'd': {
            char msg[200];
            cout<<"Join group\n";
            int acceptingport;
             //int group_id;
            recv(sock,&port,sizeof port,0);
            recv(sock,ip,sizeof(ip),0);
            send(sock,&Message,sizeof(Message),0);
            recv(sock,&group_id,sizeof group_id,0);
            if(logged_ports.find(port)==logged_ports.end()){
                strcpy(serverMessage,"you need to login first to join group");  
            }
            else{
                join_requests.insert({port,group_id});
                //cout<<"group_id "<<group_id<<endl;
                if(groupids.find(group_id)!=groupids.end()){
                    if(groups.find(port)!=groups.end()){
                        if(groups.at(port)==group_id){
                            cout<<"Already joined the group\n";
                            strcpy(serverMessage,"::Already joined the group:");
                        }
                    }
                    else{
                        
                        if(owners.find(group_id)!=owners.end()){
                            acceptingport=owners.at(group_id);
                            send(sock,&acceptingport,sizeof acceptingport,0);
                            recv(sock,&Message,sizeof Message,0);
                            //join_requests.insert({port,group_id});
                            strcpy(serverMessage,"sent details to respecting client to respective group owner for joining");  
                            threadport tp;
                            tp.accport=acceptingport;
                            tp.clport=port;
                            tp.group_id=group_id;

                            pthread_t thread_idx;

                            pthread_create(&thread_idx,NULL,acceptJoinRequest,(void*)&tp);

                            pthread_join(thread_idx,NULL);
                        }
                    }
                }
            }
            send(sock,&serverMessage,sizeof(serverMessage),0);

            memset(clientMessage,0,sizeof(clientMessage));
			fflush(stdout);
            break;
        }

        case 'e':
            cout<<"Leave group\n";
            //cout<<"Join group\n";
           // int group_id;
            recv(sock,&port,sizeof port,0);
            recv(sock,ip,sizeof(ip),0);
            send(sock,&Message,sizeof(Message),0);
            recv(sock,&group_id,sizeof group_id,0);
            if(groupids.find(group_id)!=groupids.end()){
                if(groups.find(port)!=groups.end()){
                    groups.erase(port);
                    cout<<"Leaving the group\n";
                    strcpy(serverMessage,"::Leaving the group:");
                }
                else{
                    cout<<"Not Present in the group\n";
                    strcpy(serverMessage,"::Not Present in the group:");
                }
            }
            else{
                 cout<<"Group not yet created. Leaving is not supported \n";
                strcpy(serverMessage,"::Group not yet created. Leaving is not supported::");

            }
            send(sock,&serverMessage,sizeof(serverMessage),0);

            memset(clientMessage,0,sizeof(clientMessage));
			fflush(stdout);

            break;

        case 'f':{
            cout<<"request pending join list\n";
            char msg[200],flag1='c',buf2[200];

            recv(sock,&port,sizeof port,0);
            recv(sock,ip,sizeof(ip),0);
            send(sock,&Message,sizeof(Message),0);
            recv(sock,&group_id,sizeof group_id,0);
            send(sock,&Message,sizeof(Message),0);
            recv(sock,buf2,sizeof buf2,0);
            send(sock,&Message,sizeof(Message),0);

            //cout<<buf2<<"  "<<group_id<<endl;
            for(auto i:join_requests){
                string temp1=to_string(i.first)+" asks join request to "+to_string(i.second);
                strcpy(msg,temp1.c_str());
                send(sock,msg,sizeof(msg),0);
                recv(sock,&Message,sizeof(Message),0);

            }
            strcpy(serverMessage,"sent all the pending requests");
            send(sock,&serverMessage,sizeof(serverMessage),0);

            memset(clientMessage,0,sizeof(clientMessage));
			fflush(stdout);


            break;
        }
        case 'n':{
            cout<<"list all shareable files\n";
            char msg[200];

            recv(sock,&port,sizeof port,0);
            recv(sock,ip,sizeof(ip),0);
            send(sock,&Message,sizeof(Message),0);
            recv(sock,&group_id,sizeof group_id,0);
            send(sock,&Message,sizeof(Message),0);

            for(auto it=filesAvailable.begin();it!=filesAvailable.end();it++){
                if(it->group_id==group_id){
                    string temp1="File Name: "+ string(it->fileName) + " group_id " +to_string(it->group_id) + " port :" +to_string(it->port)+
                                " ip address : "+string(it->ip) + " filesize "+ it->sha1+
                                " no of chunks "+ to_string(it->noOfChunks)+ " chunk ids: "+ it->chunkids;
                    strcpy(msg,temp1.c_str());
                    send(sock,msg,sizeof(msg),0);
                    recv(sock,&Message,sizeof(Message),0);
                }
                
            }


            strcpy(serverMessage,"sent all the pending requests");
            send(sock,&serverMessage,sizeof(serverMessage),0);

            memset(clientMessage,0,sizeof(clientMessage));
			fflush(stdout);



            break;

        }
        case 'o':{

            recv(sock,&port,sizeof port,0);
            recv(sock,ip,sizeof(ip),0);
            send(sock,&Message,sizeof(Message),0);
            if(logged_ports.find(port)!=logged_ports.end()){
                logged_ports.erase(port);
                strcpy(serverMessage,":Logged out successfully:");

            }
            else{
                strcpy(serverMessage,":Already Logged out:");

            }

            send(sock,&serverMessage,sizeof(serverMessage),0);

            memset(clientMessage,0,sizeof(clientMessage));
			fflush(stdout);
            break;

        }
        case 'g':{
            cout<<"Accepts groupid join request by user\n";//need to done few more modifications
            char msg[200],buf3[200];
            //int group_id;
            recv(sock,&port,sizeof port,0);
            recv(sock,ip,sizeof(ip),0);
            send(sock,&Message,sizeof(Message),0);
            recv(sock,&group_id,sizeof group_id,0);

            send(sock,buf3,sizeof buf3,0);
            recv(sock,&Message,sizeof(Message),0);

            for(auto it=map_user_info.begin();it!=map_user_info.end();it++){
                if(string(it->first)==string(buf3)){
                    groups.insert({it->second.port,group_id});
                    join_requests.erase(port);

                }
            }
            groups.insert({port,group_id});
            join_requests.erase(port);

            string temp=" user "+string(buf3)+" accepted to join the group and added successfully ";
        
            strcpy(serverMessage,temp.c_str());
            send(sock,&serverMessage,sizeof(serverMessage),0);

            memset(clientMessage,0,sizeof(clientMessage));
			fflush(stdout);
            break;
        }
        case 'h':{
            cout<<"List groups\n";
            char msg[200];

            recv(sock,&port,sizeof port,0);
            recv(sock,ip,sizeof(ip),0);
            send(sock,&Message,sizeof(Message),0);
            for(auto i:groupids){
                string temp1="group-id: "+to_string(i);
                strcpy(msg,temp1.c_str());
                send(sock,msg,sizeof(msg),0);
                recv(sock,&Message,sizeof(Message),0);

            }
            strcpy(serverMessage,"sent all the groups");
            send(sock,&serverMessage,sizeof(serverMessage),0);

            memset(clientMessage,0,sizeof(clientMessage));
			fflush(stdout);
            

            break;
        }
        case 'l':{
            cout<<"Upload files::\n";
            recv(sock,&port,sizeof port,0);
            recv(sock,ip,sizeof(ip),0);
            send(sock,&Message,sizeof(Message),0);

            recv(sock,&group_id,sizeof group_id,0);
            send(sock,&Message,sizeof(Message),0);

            recv(sock,clientMessage,100,0);
            send(sock,&Message,sizeof(Message),0);

            
            cmd=tokeniseCommand(clientMessage);
            strcpy(bufFile,cmd[0].c_str());
            //group_id=atoi(cmd[1].c_str());

            //cout<<"group id of the file  "<<group_id<<endl;

            recv(sock,&fileSize,sizeof fileSize,0);
            send(sock,&Message,sizeof(Message),0);

            recv(sock,&noOfChunks,sizeof fileSize,0);
            send(sock,&Message,sizeof(Message),0);

            recv(sock,hashRecvBuff,sizeof hashRecvBuff,0);
            send(sock,&Message,sizeof(Message),0);

            fileWithTracker fileInfo;
            fileInfo.port=port;
            strcpy(fileInfo.ip,ip);
            strcpy(fileInfo.fileName,bufFile);
            fileInfo.fileSize=fileSize;
            fileInfo.sha1=string(hashRecvBuff);
            fileInfo.noOfChunks=noOfChunks;
            fileInfo.chunkids=DELIMITER2;
            fileInfo.group_id=group_id;
            for(int i=1;i<=noOfChunks;i++)
                fileInfo.chunkids+=to_string(i)+DELIMITER2;
            filesAvailable.push_back(fileInfo);  
            auto it=files_group.find(group_id);
            //cout<<ip<<"  "<<fileSize<<"  "<<hashRecvBuff<<"  "<<noOfChunks<<"  "<<group_id<<"  "<<bufFile<<endl;
            
            if(it!=files_group.end()){
                it->second.insert(string(bufFile));// if there is already group with some files then we will insert into the set
            }
            else {
                files.clear();
                files.insert(string(bufFile));
                files_group.insert({group_id,files}); ///which group has which set of files 
                files.clear();
            }
            set<int> temp;
            for(int i=1;i<=noOfChunks;i++){
                temp.insert(i);
            }
            ports_with_chunks.insert({{port,string(bufFile)},temp});
            


            strcpy(serverMessage,"File Uploaded successfully in the tracker");
            send(sock,&serverMessage,sizeof(serverMessage),0);

            memset(clientMessage,0,sizeof(clientMessage));
			fflush(stdout);

            break;
        }

        case 'm':{
            
            cout<<"Download files\n";
            recv(sock,&port,sizeof port,0);
            recv(sock,ip,sizeof(ip),0);
            send(sock,&Message,sizeof(Message),0);

            recv(sock,&group_id,sizeof group_id,0);
            send(sock,&Message,sizeof(Message),0);

            //cout<<"group_id "<<group_id<<endl;
 
            recv(sock,bufFile,sizeof bufFile,0);
            send(sock,&Message,sizeof(Message),0);
            
            fileName=string(bufFile);

            string peersWithFile="";
            long maxfilesize=0;
            for(auto it=filesAvailable.begin();it!=filesAvailable.end();it++){
                if(string(it->fileName)==fileName && group_id==it->group_id){
                    fileSize=it->fileSize;
                    strcpy(hashRecvBuff,it->sha1.c_str());
                    maxfilesize=max(fileSize,maxfilesize);
                    //cout<<it->fileName<<" "<<it->port<<endl;
                    
                    if(logged_ports.find(it->port)!=logged_ports.end() || login.find(string(user_name))!=login.end()){
                        string peerPort=to_string(it->port);
                        peersWithFile+=peerPort+it->chunkids+DELIMITER;
                    }
                }
            }
            //cout<<bufFile<<endl;
            //cout<<peersWithFile<<endl;
            //vecs peerport;
            //peerport=tokeniseCommand(peersWithFile);
            //for(auto it)

            char peers[len(peersWithFile)+1];
            strcpy(peers,peersWithFile.c_str());

            if(len(peersWithFile)>3){
                send(sock,peers,sizeof peers,0);
                recv(sock,&Message,sizeof Message,0);
                send(sock,&maxfilesize,sizeof maxfilesize,0);
                recv(sock,&Message,sizeof Message,0);
                send(sock,hashRecvBuff,sizeof hashRecvBuff,0);
                recv(sock,&Message,sizeof Message,0);
            }
            else{
                strcpy(peers,"No peers available for this file");
                send(sock,peers,sizeof peers,0);
                recv(sock,&Message,sizeof Message,0);
            }

            break;
        }
        
        default:{
            cout<<"Invalid request/invalid command\n";
            strcpy(serverMessage,"Please enter valid command");
            send(sock,&serverMessage,sizeof(serverMessage),0);

            memset(clientMessage,0,sizeof(clientMessage));
			fflush(stdout);
        
            break;
        }

        //default:
        //    cout<<"Invalid flag details\n";
    }
    memset(clientMessage,0,sizeof(clientMessage));
    memset(bufFile,0,sizeof(bufFile));
    memset(hashRecvBuff,0,sizeof(hashRecvBuff));
    memset(bufDestPath,0,sizeof(bufDestPath));
	fflush(stdout);
    close(sock);
    //close(peerSockDesc);
    cmd.clear();
    cout<<"Request completed\n";



}


#endif //HELPER1