//
// Created by vlad on 14.01.19.
//
#include "bktree.h"
#include "levenshtein_wstring.h"

#include <stdio.h>
#include <iostream>
#include <string>
#include "Python.h"
#include "structmember.h"

#if PY_MAJOR_VERSION >= 3
#    define IS_PY3K
#endif

#ifndef Py_TYPE
#    define Py_TYPE(ob) (((PyObject *)(ob))->ob_type)
#endif

#ifdef IS_PY3K
#    define PyInt_FromLong PyLong_FromLong
#endif

// bktree python object

using result_pair = std::pair<std::wstring, int>;

struct pair_hash {
    size_t operator()(const result_pair &el) const {
        return std::hash<std::wstring>()(el.first);
    }
};

typedef struct {
    PyObject_HEAD MB::bktree<std::wstring, int, MB::levenshtein_distance> *ptr;
} py_bktree;

static PyMemberDef py_bk_members[] = {
    {NULL} /* Sentinel */
};

static PyObject *py_bk_new(PyTypeObject *type, PyObject *args,
                           PyObject *kwargs) {
    py_bktree *self = (py_bktree *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    self->ptr = new MB::bktree<std::wstring, int, MB::levenshtein_distance>;
    return (PyObject *)self;
}

static void py_bk_dealloc(py_bktree *self) {
    delete self->ptr;
    Py_TYPE(self)->tp_free((PyObject *)self);
}

PyObject *vector_to_pylist(const std::vector<uint32_t> &w) {
    PyObject *res_list = PyList_New(w.size());
    if (w.empty()) return res_list;
    for (size_t i = 0; i < w.size(); ++i) {
        PyList_SetItem(res_list, i, PyLong_FromUnsignedLong(w[i]));
    }

    return res_list;
}

PyObject *get_neib_to_python(const std::vector<result_pair> &result,
                             int include_distances) {
    PyObject *l = PyList_New(result.size());
    if (!include_distances) {
        for (size_t i = 0; i < result.size(); i++)
            PyList_SetItem(l, i,
                           PyUnicode_FromWideChar(result[i].first.c_str(),
                                                  result[i].first.size()));
        return l;
    } else {
        for (size_t i = 0; i < result.size(); ++i) {
            PyObject *t = PyTuple_New(2);
            PyTuple_SetItem(t, 0,
                            PyUnicode_FromWideChar(result[i].first.c_str(),
                                                   result[i].first.size()));
            PyTuple_SetItem(t, 1, PyLong_FromUnsignedLong(result[i].second));
            PyList_SetItem(l, i, t);
        }
    }
    return l;
}

static PyObject *py_bk_add_item(py_bktree *self, PyObject *args,
                                PyObject *kwargs) {
    PyObject *v;
    if (!self->ptr) return NULL;
    static char const *kwlist[] = {"string", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", (char **)kwlist, &v))
        return NULL;

    //    if (!check_constraints(self, item, true)) {
    //        return NULL;
    //    } TODO: check el in tree

    Py_ssize_t len = PyObject_Size(v);
    std::wstring w;
    w.resize(len);
    int size = PyUnicode_AsWideChar(v, const_cast<wchar_t *>(w.data()), len);

    self->ptr->insert(w);  // TODO: add insert p

    Py_RETURN_NONE;
}

static PyObject *py_bk_find_neib(py_bktree *self, PyObject *args,
                                 PyObject *kwargs) {
    PyObject *v;
    PyObject *l_result;
    int dist;
    int32_t include_distances = 0;

    if (!self->ptr) return NULL;
    static char const *kwlist[] = {"vector", "dist", "include_distances", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "Oi|i", (char **)kwlist, &v,
                                     &dist, &include_distances))
        return NULL;

    size_t len = PyObject_Size(v);
    std::wstring w;
    w.resize(len);
    if (len != PyUnicode_AsWideChar(v, const_cast<wchar_t *>(w.data()), len))
        return NULL;
    std::vector<result_pair> results;

    self->ptr->find(w, dist, std::back_inserter(results));
    l_result = get_neib_to_python(results, include_distances);

    return l_result;
}

static PyObject *py_bk_find_list_neib(py_bktree *self, PyObject *args,
                                 PyObject *kwargs) {
    PyObject *v;
    PyObject *l_result;
    int dist;
    int32_t include_distances = 0;

    if (!self->ptr) return NULL;
    static char const *kwlist[] = {"vector", "dist", "include_distances", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "Oi|i", (char **)kwlist, &v,
                                     &dist, &include_distances))
        return NULL;

    size_t len = PyObject_Size(v);
    std::vector<std::wstring> w;
    w.resize(len);
    if (len != PyUnicode_AsWideChar(v, const_cast<wchar_t *>(w.data()), len))
        return NULL;
    std::vector<result_pair> results;

    self->ptr->find(w, dist, std::back_inserter(results));
    l_result = get_neib_to_python(results, include_distances);

    return l_result;

}

static PyMethodDef AnnoyMethods[] = {
    {"add_item", (PyCFunction)py_bk_add_item, METH_VARARGS | METH_KEYWORDS,
     "add item in tree."},
    {"get_neib", (PyCFunction)py_bk_find_neib, METH_VARARGS | METH_KEYWORDS,
     "get neighbourhood."},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

static PyTypeObject PyBKTreeType = {
    PyVarObject_HEAD_INIT(NULL, 0) "bk_tree.BkTree", /*tp_name*/
    sizeof(py_bktree),                               /*tp_basicsize*/
    0,                                               /*tp_itemsize*/
    (destructor)py_bk_dealloc,                       /*tp_dealloc*/
    0,                                               /*tp_print*/
    0,                                               /*tp_getattr*/
    0,                                               /*tp_setattr*/
    0,                                               /*tp_compare*/
    0,                                               /*tp_repr*/
    0,                                               /*tp_as_number*/
    0,                                               /*tp_as_sequence*/
    0,                                               /*tp_as_mapping*/
    0,                                               /*tp_hash */
    0,                                               /*tp_call*/
    0,                                               /*tp_str*/
    0,                                               /*tp_getattro*/
    0,                                               /*tp_setattro*/
    0,                                               /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /*tp_flags*/
    "annoy objects",                                 /* tp_doc */
    0,                                               /* tp_traverse */
    0,                                               /* tp_clear */
    0,                                               /* tp_richcompare */
    0,                                               /* tp_weaklistoffset */
    0,                                               /* tp_iter */
    0,                                               /* tp_iternext */
    AnnoyMethods,                                    /* tp_methods */
    py_bk_members,                                   /* tp_members */
    0,                                               /* tp_getset */
    0,                                               /* tp_base */
    0,                                               /* tp_dict */
    0,                                               /* tp_descr_get */
    0,                                               /* tp_descr_set */
    0,                                               /* tp_dictoffset */
    0,                                               /* tp_init */
    0,                                               /* tp_alloc */
    py_bk_new,                                       /* tp_new */
};

static PyMethodDef module_methods[] = {
    {NULL} /* Sentinel */
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "bktreelib",    /* m_name */
    "",             /* m_doc */
    -1,             /* m_size */
    module_methods, /* m_methods */
    NULL,           /* m_reload */
    NULL,           /* m_traverse */
    NULL,           /* m_clear */
    NULL,           /* m_free */
};
#endif

PyObject *create_module(void) {
    PyObject *m;

    if (PyType_Ready(&PyBKTreeType) < 0) return NULL;

#if PY_MAJOR_VERSION >= 3
    m = PyModule_Create(&moduledef);
#else
    m = Py_InitModule("bktreelib", module_methods);
#endif

    if (m == NULL) return NULL;

    Py_INCREF(&PyBKTreeType);
    PyModule_AddObject(m, "BkTree", (PyObject *)&PyBKTreeType);
    return m;
}

#if PY_MAJOR_VERSION >= 3
PyMODINIT_FUNC PyInit_bktreelib(void) {
    return create_module();  // it should return moudule object in py3
}
#else
PyMODINIT_FUNC initbktreelib(void) { create_module(); }
#endif
