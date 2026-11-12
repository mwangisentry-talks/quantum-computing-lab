#define QISKIT_PYTHON_EXTENSION
#include <Python.h>
#include <qiskit.h>

#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/**
 * The default name to use for `creg_name` if none is supplied.
 */
static char DEFAULT_CREG_NAME[] = "spec";

/**
 * Is there a 2q link from the given qubit to any active qubit?
 */
 
 /**
static bool adjacent_to_active(QkNeighbors *adj, uint32_t qubit,
                               bool *active) {
    for (uint32_t offset = adj->partition[qubit];
         offset < adj->partition[qubit + 1]; offset++) {
        if (active[adj->neighbors[offset]]) {
            return true;
        }
    }
    return false;
}
/**/


static bool adjacent_to_active(QkNeighbors *adj,
                               uint32_t qubit,
                               bool *active) {

    // Handle all-to-all connectivity.
    if (qk_neighbors_is_all_to_all(adj)) {
        for (uint32_t i = 0; i < adj->num_qubits; i++) {
            if (i != qubit && active[i]) {
                return true;
            }
        }
        return false;
    }

    // Handle limited two-qubit connectivity.
    for (size_t offset = adj->partition[qubit];
         offset < adj->partition[qubit + 1]; offset++) {

        if (active[adj->neighbors[offset]]) {
            return true;
        }
    }

    return false;
}


/**
 * A transpiler pass that adds terminal measurements to all "spectator"
 * qubits.
 */
static uint32_t add_spectator_measures(QkDag *dag,
                                       const QkTarget *target,
                                       bool include_unmeasured,
                                       const char *creg_name,
                                       bool add_barrier) {
    uint32_t num_spectators = 0;
    uint32_t num_qubits = qk_dag_num_qubits(dag);
    uint32_t num_instructions = qk_dag_num_op_nodes(dag);
    bool *active = calloc(num_qubits, sizeof(*active));
    bool *is_additional_spectator =
        calloc(num_qubits, sizeof(*is_additional_spectator));
    uint32_t *spectators = malloc(num_qubits * sizeof(*spectators));
    uint32_t *topological =
        malloc(num_instructions * sizeof(*topological));
    QkNeighbors neighbors;
    QkCircuitInstruction instruction;

    qk_neighbors_from_target(target, &neighbors);
    qk_dag_topological_op_nodes(dag, topological);

    for (uint32_t i = 0; i < num_instructions; i++) {
        qk_dag_get_instruction(dag, topological[i], &instruction);
        if (!strcmp(instruction.name, "barrier")) {
            // Barriers don't count for the purposes of determining
            // final measurements, either.
            qk_circuit_instruction_clear(&instruction);
            continue;
        }
        // If we're not adding measurements to "unmeasured" active
        // qubits, then nothing counts as an additional "maybe
        // spectator".  If we are, then it's a maybe spectator if its
        // last visited instruction was not a measure.
        bool additional_spectator =
            include_unmeasured && strcmp(instruction.name, "measure");
        for (uint32_t *qarg = instruction.qubits;
             qarg != instruction.qubits + instruction.num_qubits;
             qarg++) {
            active[*qarg] = true;
            is_additional_spectator[*qarg] = additional_spectator;
        }
        qk_circuit_instruction_clear(&instruction);
    }

    for (uint32_t qubit = 0; qubit < num_qubits; qubit++) {
        bool is_spectator =
            !active[qubit] &&
            adjacent_to_active(&neighbors, qubit, active);
        is_spectator = is_spectator || is_additional_spectator[qubit];
        if (is_spectator) {
            spectators[num_spectators] = qubit;
            num_spectators += 1;
        }
    }

    if (num_spectators) {
        uint32_t clbit = qk_dag_num_clbits(dag);
        creg_name = creg_name ? creg_name : DEFAULT_CREG_NAME;
        QkClassicalRegister *creg =
            qk_classical_register_new(num_spectators, creg_name);
        qk_dag_add_classical_register(dag, creg);
        qk_classical_register_free(creg);
        if (add_barrier) {
            qk_dag_apply_barrier(dag, NULL, num_qubits, false);
        }
        for (uint32_t i = 0; i < num_spectators; i++) {
            qk_dag_apply_measure(dag, spectators[i], clbit + i, false);
        }
    }

    qk_neighbors_clear(&neighbors);
    free(topological);
    free(spectators);
    free(is_additional_spectator);
    free(active);
    return num_spectators;
}

static PyObject *py_add_spectator_measures(PyObject *self,
                                           PyObject *args,
                                           PyObject *kwargs) {
    // Define space to hold the C-native handles we will parse out of the
    // Python-space inputs.
    QkDag *dag;
    QkTarget *target;
    const char *creg_name;
    int include_unmeasured, add_barrier;

    // This `kwlist` and `PyArg_Parse*` setup is standard Python C API
    // programming for extension modules.  We will examine the use of
    // Qiskit C API functions within it afterwards.
    static char *const kwlist[] = {
        "dag",       "target",      "include_unmeasured",
        "creg_name", "add_barrier", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&O&|pzp", kwlist,
                                     qk_dag_convert_from_python, &dag,
                                     qk_target_convert_from_python,
                                     &target, &include_unmeasured,
                                     &creg_name, &add_barrier)) {
        // An error has occurred. The Python exception state will already
        // be set, so we need to return the error indicator.
        return NULL;
    }

    // Now we have C-native types, we can delegate to our C logic.
    add_spectator_measures(dag, target, include_unmeasured, creg_name,
                           add_barrier);
    Py_RETURN_NONE;
}
static PyMethodDef core_methods[] = {
    // This entry is our function, cast to the correct type.
    {"add_spectator_measures",
     (PyCFunction)(void (*)(void))py_add_spectator_measures,
     METH_VARARGS | METH_KEYWORDS, ""},
    // A sentinel marking the end of the list.
    {NULL, NULL, 0, NULL},
};
static struct PyModuleDef core_module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "_core",
    .m_methods = core_methods,
};
PyMODINIT_FUNC PyInit__core(void) {
    // This line is critical to use the Qiskit C API. Your code will
    // likely be immediately terminated by the operating system if you
    // forget to do this.
    if (qk_import() < 0) {
        return NULL;
    };
    // The standard Python call to initialize a module.
    return PyModuleDef_Init(&core_module);
}

