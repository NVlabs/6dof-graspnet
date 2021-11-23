#if PY_VERSION_HEX < 0x03000000
// ByteArray buffer protocol functions
// See: http://www.python.org/dev/peps/pep-3118/
extern "C" {
static Py_ssize_t SbkByteArray_segcountproc(PyObject* self, Py_ssize_t* lenp)
{
    if (lenp)
        *lenp = self->ob_type->tp_as_sequence->sq_length(self);
    return 1;
}
static Py_ssize_t SbkByteArray_readbufferproc(PyObject* self, Py_ssize_t segment, void** ptrptr)
{
    if (segment || !Shiboken::Object::isValid(self))
        return -1;

    ByteArray* cppSelf = %CONVERTTOCPP[ByteArray*](self);
    *ptrptr = reinterpret_cast<void*>(const_cast<char*>(cppSelf->data()));
    return cppSelf->size();
}
PyBufferProcs SbkByteArrayBufferProc = {
    /*bf_getreadbuffer*/    &SbkByteArray_readbufferproc,
    /*bf_getwritebuffer*/   (writebufferproc)&SbkByteArray_readbufferproc,
    /*bf_getsegcount*/      &SbkByteArray_segcountproc,
    /*bf_getcharbuffer*/    (charbufferproc)&SbkByteArray_readbufferproc
};
}
#endif
