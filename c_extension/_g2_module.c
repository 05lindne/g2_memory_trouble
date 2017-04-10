#include <Python.h>
#include <numpy/arrayobject.h>
#include "g2_module.h"

static char module_docstring[] = "This module provides an interface for calculating G2 functions using C.";
static char test_docstring[] = "Calculate the G2 function of some data.";

static PyObject *g2_module_test(PyObject *self, PyObject *args);

static PyMethodDef module_methods[] = {
    {"test", g2_module_test, METH_VARARGS, test_docstring},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC init_g2_module(void)
{
    PyObject *m = Py_InitModule3("_g2_module", module_methods, module_docstring);
    if (m == NULL)
        return;

    /* Load `numpy` functionality. */
    import_array();
}

// int test(int* g2, int* cl1, const float scale);
static PyObject *g2_module_test(PyObject *self, PyObject *args)
{
    const float scale;
    PyObject *g2_obj, *cl1_obj;


    /* Parse the input tuple */
    if (!PyArg_ParseTuple(args, "OOd", &g2_obj, &cl1_obj, &scale))
        return NULL;

    /* Interpret the input objects as numpy arrays. */
    PyObject *g2_array = PyArray_FROM_OTF(g2_obj, NPY_INT, NPY_IN_ARRAY);
    PyObject *cl1_array = PyArray_FROM_OTF(cl1_obj, NPY_INT, NPY_IN_ARRAY);
    
    /* If that didn't work, throw an exception. */
    if (g2_array == NULL || cl1_array == NULL) {
        Py_XDECREF(g2_array);
        Py_XDECREF(cl1_array);
        return NULL;
    }

    /* How many data points are there? */
    int len_g2 = (int)PyArray_DIM(g2_array, 0);
    int len_n1 = (int)PyArray_DIM(cl1_array, 0);

    /* Get pointers to the data as C-types. */
    int *g2    = (int*)PyArray_DATA(g2_array);
    int *cl1    = (int*)PyArray_DATA(cl1_array);
    
    /* Call the external C function to compute the chi-squared. */
    double value = test(g2, cl1, len_n1);

    /* Clean up. */
    Py_DECREF(g2_array);
    Py_DECREF(cl1_array);
   
    if (value < 0.0) {
        PyErr_SetString(PyExc_RuntimeError,
                    "Chi-squared returned an impossible value.");
        return NULL;
    }

    /* Build the output tuple */
    PyObject *ret = Py_BuildValue("d", value);
    return ret;
}

