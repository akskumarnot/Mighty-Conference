#ifndef PY_JSON_H
#define PY_JSON_H
#include"json_utilities.h"
#include<stdio.h>

static PyObject* Py_JSON_make_length_str(PyObject *self, PyObject *args)
{
	char * str = NULL;

	if (!PyArg_ParseTuple(args, "s", &str))
           return Py_BuildValue("");
	
	int len = strlen(str);
	char len_str[LEN_STR_LENGTH];
	snprintf(len_str,6,"%5d",len);
	
	json_t* len_str_object = json_object();
	int err = json_object_set_new(len_str_object, "LENGTH", json_string(len_str));
	
	if( err == -1 )
	{
		printf("Error in json_object_get");
		return Py_BuildValue("");
	}
	
	char * str1 = json_dumps(len_str_object , JSON_DECODE_ANY);
	PyObject * tmp = PyString_FromString(str1);
	json_decref(len_str_object);
	return tmp;
}

static PyObject* Py_JSON_make_str(PyObject *self, PyObject *args)
{
	char * key ,*value;
	if (!PyArg_ParseTuple(args, "ss", &key,&value))
           return Py_BuildValue("");	
	json_t* root = json_object();
	json_object_set_new(root,key,json_string(value));
	char * str = json_dumps(root , JSON_DECODE_ANY);
	return Py_BuildValue("s",str);
}

//all functions in the JSON module
PyMethodDef pyjson_methods[] = {
    {"makeLenStr", Py_JSON_make_length_str, METH_VARARGS,"function to make length string"},
    {"makeJSONStr", Py_JSON_make_str, METH_VARARGS,"function to make json string"},
    {NULL, NULL, 0, NULL}
};

#endif
