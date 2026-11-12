# spectator\_measures: Qiskit C Extension Documentation

Version 0.0.1 · Qiskit \~=2.6.0 · Python 3.12 · tested against Qiskit 2.6.2

## 1. Overview

`spectator_measures` is a Python package with a native C extension (`spectator_measures._core`) built on the Qiskit C API. It provides one transpiler pass, `AddSpectatorMeasures`, which adds terminal measurements to **spectator qubits**: qubits that carry no operations but are directly coupled to a qubit that does. Measuring them records crosstalk and leakage on idle neighbours of the active region.

The pass edits the circuit's `DAGCircuit` in place through the Qiskit C API, so the analysis and the edit both run natively.

## 2. Package layout

| File | Role |
| --- | --- |
| `pyproject.toml` | Build metadata; requires `qiskit~=2.6.0` at build and run time |
| `setup.py` | Declares the `spectator_measures._core` extension and adds `qiskit.capi.get_include()` to the include path |
| `src/spectator_measures/_coremodule.c` | The C implementation (module `_core`) |
| `src/spectator_measures/__init__.py` | Python wrapper: the `AddSpectatorMeasures` pass |
| `tests/` | `test_spectators.py`, `test_all_to_all.py`, `test_spectators_advanced.py` |

## 3. Building and installing

```bash
pip install qiskit~=2.6.0
pip install -e . --no-build-isolation
```

The build needs a C compiler and the Qiskit C headers shipped with the `qiskit` wheel. `setup.py` locates them with `qiskit.capi.get_include()`.

## 4. Python API

```python
AddSpectatorMeasures(
    target: Target,
    *,
    include_unmeasured: bool = False,
    creg_name: str | None = None,
    add_barrier: bool = True,
)
```

| Parameter | Meaning |
| --- | --- |
| `target` | The `Target` whose two-qubit connectivity defines adjacency. Its qubit count must equal the circuit's. |
| `include_unmeasured` | If `True`, also measure active qubits whose last non-barrier operation is not a `measure`. |
| `creg_name` | Name of the new classical register. Default `"spec"`. |
| `add_barrier` | If `True`, insert a barrier across all qubits before the new measurements. |

### Example

```python
from qiskit import QuantumCircuit
from qiskit.transpiler import Target, CouplingMap, PassManager
from spectator_measures import AddSpectatorMeasures

target = Target.from_configuration(
    basis_gates=["x", "sx", "rz", "cx"],
    num_qubits=10,
    coupling_map=CouplingMap.from_line(10),
)
qc = QuantumCircuit(10)
qc.x(0)
qc.x(5)

result = PassManager([AddSpectatorMeasures(target)]).run(qc)
# qubits 1, 4 and 6 are measured into a 3-bit register named "spec"
```

### Behaviour of the wrapper

- `run(dag)` mutates the DAG in place and returns it.
- If `creg_name` is not given, the wrapper picks `spec`, then `spec_1`, `spec_2`, and so on, so that the pass can be applied repeatedly to one DAG.
- If an explicit `creg_name` already exists on the DAG, it raises `TranspilerError`. This check lives in Python because the Qiskit C API aborts the process on a duplicate register name (section 8).
- A non-string `creg_name` raises `TypeError`.

## 5. Algorithm

`add_spectator_measures` in `_coremodule.c` works in four steps.

1. **Find active qubits.** Walk the DAG's operation nodes in topological order. Barriers are skipped. Every qubit touched by any other instruction is marked active. When `include_unmeasured` is set, a qubit is also flagged as an *additional spectator* if its most recently visited instruction was not a `measure`.
2. **Read connectivity.** Build a `QkNeighbors` object from the target with `qk_neighbors_from_target`.
3. **Select spectators.** A qubit is a spectator if it is inactive and has a two-qubit link to an active qubit, or if it is flagged as an additional spectator.
   - *Limited connectivity:* scan the qubit's slice of the adjacency list, `neighbors[partition[q] .. partition[q+1])`.
   - *All-to-all:* every inactive qubit is adjacent to every active one, so an inactive qubit is a spectator exactly when at least one qubit is active.
4. **Edit the DAG.** If there are any spectators: add a classical register of that size, optionally apply a barrier across all qubits, then apply one `measure` per spectator into consecutive new clbits.

The DAG is only modified in step 4, after all analysis and validation have succeeded.

## 6. Qiskit C API semantics relied upon

These were checked against the current IBM Qiskit C API documentation.

| Function | Behaviour used |
| --- | --- |
| `qk_neighbors_from_target` | Returns `true` for all-to-all connectivity. In that case `neighbors` and `partition` are null and only `num_qubits` is meaningful. |
| `QkNeighbors` (limited case) | `partition` has `num_qubits + 1` entries (type `size_t`), starting at 0 and non-decreasing. Each qubit's neighbours are sorted and unique. |
| `qk_neighbors_clear` | Safe to call on an all-to-all object (a no-op). Must only be called on objects initialised by `qk_neighbors_from_target`. |
| `qk_dag_num_op_nodes` | Returns `size_t`. |
| `qk_dag_topological_op_nodes` | Writes exactly `qk_dag_num_op_nodes(dag)` `uint32_t` values into the caller's buffer. |
| `qk_dag_get_instruction` | Output must be released with `qk_circuit_instruction_clear` before reuse. |
| `qk_dag_apply_barrier` | A null `qubits` pointer means all qubits in the DAG; `num_qubits` is then not read. |
| `qk_dag_apply_measure` | Behaviour is undefined if the qubit or clbit index is out of range. |
| `qk_classical_register_new` | `name` must be a valid, nul-terminated UTF-8 string. |
| `qk_dag_add_classical_register` | Returns `void`. Observed behaviour: it panics, aborting the process, if a register with the same name already exists. This is not stated in the documentation. |
| `qk_dag_convert_from_python`, `qk_target_convert_from_python` | `PyArg_Parse*` converters that borrow the Python object and set a Python exception on failure. |

## 7. Safety and robustness design

### Argument parsing

The format string is `"O&O&|pzp"` with keywords `dag, target, include_unmeasured, creg_name, add_barrier`. Omitted optionals are never written by `PyArg_ParseTupleAndKeywords`, so the defaults are assigned before parsing:

```c
int include_unmeasured = 0;
int add_barrier = 1;
const char *creg_name = NULL;
```

`z` accepts `str` or `None` and rejects embedded NUL characters. The string is borrowed from the Python object and stays valid for the duration of the call.

### Memory

- All four working arrays are allocated through `alloc_zeroed`, which requests at least one element. `NULL` therefore always means allocation failure, even for an empty circuit with zero instructions.
- All exits go through a single `cleanup:` label, which frees the arrays, the `QkNeighbors` (only if it was initialised) and the temporary `QkClassicalRegister`.
- Each instruction is cleared inside the loop immediately after use.

### Bounds

- The target's qubit count must equal the DAG's. Otherwise both `active[]` and the adjacency list would be indexed out of range.
- Qubit indices read from instructions come from the DAG itself and are in range.

### Integer types

- The operation count is kept as `size_t` and rejected if it exceeds `UINT32_MAX`, because node indices are `uint32_t`.
- `clbit + num_spectators` is checked for wraparound before use.

### Error propagation to Python

`add_spectator_measures` returns a status code, which the Python-facing function turns into an exception:

| Status | Python exception |
| --- | --- |
| `SPEC_NOMEM` | `MemoryError` |
| `SPEC_TARGET_MISMATCH` | `ValueError` |
| `SPEC_TOO_LARGE` | `OverflowError` |
| `SPEC_INTERNAL` (register creation returned `NULL`) | `RuntimeError` |

Argument parsing errors, such as passing something other than a `DAGCircuit`, are raised by the converters as `TypeError`.

## 8. Known limitations

- **Duplicate register names abort the process.** Because this panic cannot be caught from C, the uniqueness check is done in `AddSpectatorMeasures.run`. Calling `_core.add_spectator_measures` directly bypasses that protection.
- **Qubit counts must match.** A target with a different number of qubits than the circuit is rejected with `ValueError`; the pass does not map or truncate.
- **Unverified inputs.** Behaviour with control-flow operations or custom operations in the DAG has not been tested, and neither has a zero-qubit target.
- **GIL.** The extension holds the GIL for the whole call.

## 9. Testing

All three test scripts pass on the patched code with Qiskit 2.6.2.

| Test | Checks |
| --- | --- |
| `test_spectators.py` | Line-coupled 10-qubit circuit with X on qubits 0 and 5: three spectators are measured into a register `spec`. |
| `test_all_to_all.py` | Target with no coupling map: every qubit except 0 and 5 is measured. |
| `test_spectators_advanced.py` | `include_unmeasured=True` gives `{0, 1, 4, 5, 6}`; an existing measurement is preserved and `num_clbits == 4`; `add_barrier=False` emits no barrier; an empty circuit adds nothing. |

Edge cases checked manually against the patched build: omitted optional arguments (barrier now defaults to on), running the pass twice on one DAG (`spec`, `spec_1`), an explicit duplicate name (`TranspilerError`), and a target smaller or larger than the circuit (`ValueError`).

The audit was done by compiling and running the code. An AddressSanitizer or valgrind run has not been done.

## 10. Changes made during the safety audit

1. Optional arguments are initialised before parsing.
2. Allocation failures are checked and raise `MemoryError`; zero-length allocations are handled by `alloc_zeroed`.
3. The target and DAG qubit counts are compared, with `ValueError` on mismatch (previously a segfault or out-of-bounds read).
4. `size_t` and `uint32_t` conversions are checked.
5. A single cleanup path frees all resources on every exit.
6. The all-to-all branch was changed from O(n²) to O(n) with identical results.
7. Duplicate classical register names are resolved or rejected in Python (previously a process abort).
8. The dead commented-out function was removed, `DEFAULT_CREG_NAME` was made `const`, and `kwlist` was made non-`const` to remove a compiler warning on Python 3.12.

## 11. References

- IBM Quantum Platform, *Qiskit C API: QkNeighbors*, https://quantum.cloud.ibm.com/docs/en/api/qiskit-c/qk-neighbors
- IBM Quantum Platform, *Qiskit C API: QkDag*, https://quantum.cloud.ibm.com/docs/en/api/qiskit-c/qk-dag
- IBM Quantum Platform, *Qiskit C API: QkClassicalRegister*, https://quantum.cloud.ibm.com/docs/en/api/qiskit-c/qk-classical-register
- Python documentation, *Parsing arguments and building values* (`PyArg_ParseTupleAndKeywords`)
