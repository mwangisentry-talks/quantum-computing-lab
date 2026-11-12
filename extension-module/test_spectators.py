from qiskit import QuantumCircuit
from qiskit.transpiler import CouplingMap, Target
from spectator_measures import AddSpectatorMeasures

num_qubits = 10

qc = QuantumCircuit(num_qubits)
qc.x(0)
qc.x(5)

target = Target.from_configuration(
    basis_gates=["x", "sx", "rz", "cx"],
    num_qubits=num_qubits,
    coupling_map=CouplingMap.from_line(num_qubits),
)

pass_ = AddSpectatorMeasures(target)

result = pass_(qc)

print(result.draw())
