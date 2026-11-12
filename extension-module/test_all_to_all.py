
from qiskit import QuantumCircuit
from qiskit.transpiler import Target, PassManager
from spectator_measures import AddSpectatorMeasures

N = 10

# No coupling map: globally connected target
target = Target.from_configuration(
    basis_gates=["x", "sx", "rz", "cx"],
    num_qubits=N,
)

qc = QuantumCircuit(N)
qc.x(0)
qc.x(5)

result = PassManager([
    AddSpectatorMeasures(target)
]).run(qc)

measured = {
    result.find_bit(inst.qubits[0]).index
    for inst in result.data
    if inst.operation.name == "measure"
}

expected = set(range(N)) - {0, 5}

print("Measured qubits:", sorted(measured))
print("Expected qubits:", sorted(expected))

assert measured == expected

print("ALL-TO-ALL TEST PASSED!")

