if (!%CPPSELF.%FUNCTION_NAME()) {
    PyObject* error_msg = PyBytes_FromFormat(
            "Could not open file: \"%s\"", %CPPSELF->filename());
    PyErr_SetObject(PyExc_IOError, error_msg);
    return 0;
}
