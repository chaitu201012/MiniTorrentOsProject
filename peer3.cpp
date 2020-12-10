#include "headerfiles_global_vars.h"
#include "peerHelper.h"
#include "peerHelperServer.h"

using namespace std;


//******************************************

//main  program
int main(int argc, char *argv[]){

    if (argc<3){
        cout<<"Please enter the Port number as a command line argument\n";
        return 0;
    }
    char* token = strtok(argv[1], ":");
	int port = atoi(strtok(NULL, ":"));

    clientInfo cl;


    strcpy(cl.trackFile,argv[2]);
    strcpy(cl.ip,token);
    cl.port=port;
    pthread_t thread_id1,thread_id2;

//*****************Thread creations for peer to acts as client and request services*******************


    if(pthread_create(&thread_id2, NULL, client_func,(void*)&cl)){
        cout<<"client thread creation failed with error "<<endl;
        exit(EXIT_FAILURE);
    }



//*****************Thread creations for peer to acts as server Listener******************************


    if(pthread_create(&thread_id1, NULL, serverListener,(void*)&port)){
        cout<<"Listenr thread creation failed with error "<<endl;
        exit(EXIT_FAILURE);
    }


    pthread_join(thread_id1,NULL);
    pthread_join(thread_id2,NULL);
    cout<<"Leaving !!!!\n";
    return 0;
}
