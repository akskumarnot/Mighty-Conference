#ifndef PY_IO_H
#define PY_IO_H
#include"json_utilities.h"
#include<stdio.h>
#include<unistd.h>

extern int sock_fd;
extern pthread_rwlock_t lock;


static PyObject* pyio_write(PyObject *self, PyObject *args)
{
	//parse arguments
	 PyObject* temp_p = PyTuple_GetItem(args,0);
	const char* data = PyString_AsString(temp_p);
	
	int len= strlen(data);
	//will take string and length
	
	//will take lock , will get blocked if writing going on
	pthread_rwlock_wrlock(&lock);

	if( write(sock_fd,data,len) < len)
	{
		perror("Error in writing\n");
	}

	pthread_rwlock_unlock(&lock);
	
	return Py_None;
}

/* Function to get data */

static PyObject* pyio_read(PyObject *self, PyObject *args)
{
	//parse arguments
	int len;
 
	if (!PyArg_ParseTuple(args, "i", &len))
           return NULL;	
	
	//will take the number of characters to be read from the socket
	char buffer[len];
	int char_count = len;
	int chars_read = 0;

	while( ( chars_read = read(sock_fd, buffer + chars_read , char_count ) ) > 0 )
	{
		char_count = char_count - chars_read;
		if( char_count == 0 )
		{
			// All chars are read, break out
			break;
		}
	}

	if( chars_read == -1 )
	{
		perror("Error in reading the line in readLine function : handle_client.h\n");
		return Py_BuildValue("");
	}
	else if( chars_read == 0 )
	{
		printf("Client's connection is terminated\n");
		return Py_BuildValue("");
	}

	return Py_BuildValue("s", buffer);
}

//all functions in the IO module
PyMethodDef pyio_methods[] = {
    {"read", pyio_read, METH_VARARGS,"to send data to the server"},
    {"write", pyio_write, METH_VARARGS,"to send data to the server"},
    {NULL, NULL, 0, NULL}
};

#endif

