
from qiskit import QuantumCircuit
from qiskit.transpiler import Target, CouplingMap, PassManager
from spectator_measures import AddSpectatorMeasures

N = 10

target = Target.from_configuration(
    basis_gates=["x", "sx", "rz", "cx"],
    num_qubits=N,
    coupling_map=CouplingMap.from_line(N),
)

def run_pass(qc, **kwargs):
    pm = PassManager([AddSpectatorMeasures(target, **kwargs)])
    return pm.run(qc)

def measured_qubits(qc):
    return {
        qc.find_bit(inst.qubits[0]).index
        for inst in qc.data
        if inst.operation.name == "measure"
    }

# TEST 1: Include unmeasured active qubits
qc1 = QuantumCircuit(N)
qc1.x(0)
qc1.x(5)

result1 = run_pass(qc1, include_unmeasured=True)
assert measured_qubits(result1) == {0, 1, 4, 5, 6}
print("TEST 1 PASSED: include_unmeasured")

# TEST 2: Preserve existing measurements
qc2 = QuantumCircuit(N, 1)
qc2.x(0)
qc2.measure(0, 0)
qc2.x(5)

result2 = run_pass(qc2)
assert measured_qubits(result2) == {0, 1, 4, 6}
assert result2.num_clbits == 4
print("TEST 2 PASSED: existing measurements")

# TEST 3: Disable barrier
qc3 = QuantumCircuit(N)
qc3.x(0)
qc3.x(5)

result3 = run_pass(qc3, add_barrier=False)
assert measured_qubits(result3) == {1, 4, 6}
assert not any(
    inst.operation.name == "barrier" for inst in result3.data
)
print("TEST 3 PASSED: add_barrier=False")

# TEST 4: Empty circuit
qc4 = QuantumCircuit(N)

result4 = run_pass(qc4)
assert measured_qubits(result4) == set()
assert result4.num_clbits == 0
print("TEST 4 PASSED: empty circuit")

print("\nALL ADVANCED TESTS PASSED!")

