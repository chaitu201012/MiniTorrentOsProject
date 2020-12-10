#ifndef  HELPER2
#define  HELPER2
//******************************

#include "headerfiles_global_vars.h"
#include "helper1_tracker.h"


using namespace std;

void* quitting(void* pargs)
{
	string s1;
	std::cout<<"\n";
	while(1)
	{
		cin>>s1;
		if(s1=="quit")
		{
			std::cout<<"Qutting the tracker\n";
			exit(0);
		}
	}
}

void connectionEstablish(int serverSockDesc,struct sockaddr_in servAddress){
    int sockAddrLen=sizeof(sockaddr);
    while(1){
        int sock=accept(serverSockDesc,(struct sockaddr *)&servAddress,(socklen_t *)&sockAddrLen);
        cout<<":: Connection Established ::"<<sock<<endl;
        pthread_t thread;
        if(pthread_create(&thread,NULL,handleServices,(void *)&sock)){
            cout<<"Error creating thread for handle services in tracker\n";
            continue;
        }
    }


}

void listenerServerCreate(char ip[],int portNoTracker){

    int serverSockDesc=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in servAddress;
    servAddress.sin_family=AF_INET;
    servAddress.sin_port = htons(portNoTracker);
	servAddress.sin_addr.s_addr=inet_addr(ip);
    if(bind (serverSockDesc  , (struct sockaddr *)&servAddress , sizeof (servAddress) )<0){
        cout<<"bind failedd";
        exit(1);
    }
    
    cout<<"::LISTENING::\n";
    listen(serverSockDesc,50);
    
    connectionEstablish(serverSockDesc,servAddress);

}

void* listener(void* pargs){
    tracker_attrs* track= (tracker_attrs*)pargs;

    char ip[32];
    strcpy(ip,track->ipAddress);
    int portNo =track->port;

    std::cout<<ip<<"\n";
    listenerServerCreate(ip,portNo);
}

#endif//tracker2