#define PORT 8000
#ifndef CLIENT_H
#define CLIENT_H
#include<string.h>
#include"webserver.h"
#include"json_utilities.h"


extern pthread_rwlock_t lock;

int sock_fd;
struct sockaddr_in server_sock;
extern PyObject * pModule; //ref to the python module object

void client_init(){
	sock_fd = socket(AF_INET,SOCK_STREAM,0);
	
	// Initialize the socket structure to zero
	memset(&server_sock,sizeof(struct sockaddr_in),'\0');

	server_sock.sin_port = htons(PORT);
	server_sock.sin_family = AF_INET;
	inet_pton(AF_INET,"127.0.0.1",&server_sock.sin_addr);

	//initiate connection 
	connect(sock_fd,(struct sockaddr*)&server_sock,sizeof(struct sockaddr_in));
	
	//connection made
}


static void* heart_beat_message(void* arg)
{
	int * tmp = (int *)arg;
	int sock_fd = *tmp;
	
	json_t * json_obj=NULL;
	json_t * json_obj2=NULL;	
	while(1)
	{
		printf("Sent BEEP\n");
		
		json_obj = JSON_make_str("HEARTBEAT","BEEP");
		char* str = json_dumps(json_obj, JSON_DECODE_ANY);
		
		//length string json object
		json_obj2 = JSON_make_length_str(str); 
		char* len_str =  json_dumps(json_obj2, JSON_DECODE_ANY);
		
		//lock
		pthread_rwlock_wrlock(&lock);

		Write(sock_fd,len_str,JSON_LEN_SIZE);
		Write(sock_fd,str,strlen(str));

		//unlock
		pthread_rwlock_unlock(&lock);

		sleep(4);
	}

	json_decref(json_obj);
	json_decref(json_obj2);
}

void heartbeat_init(){
	pthread_t tid;
	if( pthread_create(&tid, NULL, heart_beat_message, (void*)&sock_fd) != 0)
	{
		printf("Failed to spawn a thread for the list monitor");
	}
}

void client_parse(char * message_json){
	const char * value;
	/*checking for authentication*/	
	json_t* json_obj = JSON_get_value_from_pair(message_json,"AUTH");
	value = json_string_value(json_obj);
	if(value!=NULL){
	PyObject * pFunc = PyObject_GetAttrString(pModule, "auth_state");
	//now need to call the function
	if (pFunc && PyCallable_Check(pFunc)) {
		//the auth_state function in module ws is present and is callable	
            	PyObject_CallObject(pFunc, Py_BuildValue("s", value));
		printf("\ncalled obj\n");
       		}
	json_decref(json_obj);
	return;	//auth related
	}
    json_decref(json_obj);
}
#endif
