#ifndef WEBSERVER_H
#define WEBSERVER_H
#define WS_RUNNING 1
#define WS_STOP 0
#include <Python.h>
#include"py_json.h"
#include"py_io.h"

int ws_state;
PyObject * pModule; //object to hold module


//need to create a new thread for the server
void* ws_start(void * args){
	//args is null 
      PyObject 
   *pName, //object to hold script name string
   *pFunc; //object to hold function


    Py_Initialize(); //init the interpreter
   //extend the embedded interpreter for sending data to t
 
   //extension module functions added to the embedded intepreter	
   Py_InitModule("pyio",  pyio_methods);
   Py_InitModule("pyjson",  pyjson_methods);

   //functions added to interpreter

    PySys_SetPath("./../Webserver:/usr/lib/python2.7"); //all the webserver files will be in this
    pName = PyString_FromString("ws");
    //created a python string	
    /* Error checking of pName left out */

    pModule = PyImport_Import(pName);
	//pModule now importing the webserver module
    Py_DECREF(pName);    //script name object not needed so reference being decremented
	
    if (pModule != NULL) {
        pFunc = PyObject_GetAttrString(pModule, "main");
        /* pFunc is a new reference */

        if (pFunc && PyCallable_Check(pFunc)) {
		//the main function in module ws is present and is callable	
            PyObject_CallObject(pFunc, NULL);
	    ws_state=WS_RUNNING;
        }
        else {
            if (PyErr_Occurred())
                PyErr_Print();
            fprintf(stderr, "Cannot find function \"%s\"\n", "main");
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    }
    else {
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n", "webserver.py");
        return ;
    }
	//need  to keep the thread alive
	//while(ws_state==WS_RUNNING){}
}

//function to embed the py interpreter and start the webserver
int ws_init(){
    pthread_t tid;
    if( pthread_create(&tid, NULL, ws_start, NULL) != 0)
	{
		printf("Failed to spawn a thread for the python webserver");
		return -1;
	}
}

void ws_finalize(){
	//to terminate the thread
	ws_state= WS_STOP;
	Py_Finalize();
}
#endif
