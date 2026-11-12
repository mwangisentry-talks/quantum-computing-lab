# quantum-computing-lab

quantum-computing-lab/
│
├── README.md
├── LICENSE
├── requirements.txt
├── .gitignore
│
├── 01_quantum_fundamentals/
│   ├── qubits.ipynb
│   ├── superposition.ipynb
│   ├── entanglement.ipynb
│   └── quantum_gates.ipynb
│
├── 02_qiskit/
│   ├── bell_state.ipynb
│   ├── ghz_state.ipynb
│   └── quantum_circuits.ipynb
│
├── 03_quantum_algorithms/
│   ├── deutsch_jozsa.ipynb
│   ├── bernstein_vazirani.ipynb
│   ├── grover.ipynb
│   └── shor.ipynb
│
├── 04_classical_vs_quantum/
│   ├── complexity_classes.ipynb
│   ├── grover_comparison.ipynb
│   └── shor_comparison.ipynb
│
├── 05_quantum_machine_learning/
│   └── introductory_qml.ipynb
│
├── 06_simulation_and_noise/
│   ├── noise_models.ipynb
│   └── error_analysis.ipynb
│
├── research/
│   ├── literature_review/
│   ├── references.bib
│   └── research_notes/
│
└── results/
    ├── figures/
    └── experiment_data/

    ⚙️ Environment Setup
Fedora / JupyterLab
Install the quantum packages:

bash
%pip install qiskit qiskit-aer pennylane cirq
Restart the kernel, then verify the environment:

python
import qiskit
import qiskit_aer
import pennylane as qml
import cirq

print("Qiskit:", qiskit.__version__)
print("Qiskit Aer:", qiskit_aer.__version__)
print("PennyLane:", qml.__version__)
print("Cirq:", cirq.__version__)
print("Quantum environment ready!")
Check Installed Versions
python
from importlib.metadata import version

packages = [
    "qiskit",
    "qiskit-aer",
    "pennylane",
    "cirq",
]

for package in packages:
    try:
        print(package, version(package))
    except Exception:
        print(package, "Not installed")
Tip: For Qiskit 2.6.2, use a separate test environment if you want to upgrade without disrupting existing notebooks.

🧪 Practical Laboratory Activities
Experiment A: Qiskit 2.6.2 Baseline — Bell State
Test the current stable release and implement a Bell-state circuit.

python
from qiskit import QuantumCircuit
from qiskit_aer import AerSimulator

qc = QuantumCircuit(2, 2)
qc.h(0)
qc.cx(0, 1)
qc.measure([0, 1], [0, 1])

simulator = AerSimulator()
result = simulator.run(qc, shots=1024).result()
print(result.get_counts())
Expected dominant outcomes: 00 and 11

Experiment B: Cross-Framework Comparison
Implement the same Bell-state circuit in Qiskit and Cirq. Compare:

Circuit representations

Measurement distributions

Execution times

Experiment C: Classical vs Quantum — Grover's Algorithm
Implement Grover's algorithm and compare query complexity against classical unstructured search.

Separate theoretical query advantage from actual wall-clock performance.

📋 Development Priorities
Qiskit 2.6.2 — Establish the stable baseline and implement fundamental quantum circuits.

Qiskit Aer — Benchmark noise models and simulator performance, documenting its maintenance limitations.

Cirq — Build a cross-framework implementation of selected algorithms.

PennyLane — Explore hybrid quantum-classical algorithms and variational circuits.

CUDA-Q — Investigate GPU acceleration and quantum resource estimation.

🔬 Research Areas
For quantum-versus-classical research, track these areas:

Quantum complexity — BQP vs BPP, oracle separations, computational advantage

Quantum algorithms — Shor, Grover, Bernstein–Vazirani, Deutsch–Jozsa

Quantum machine learning — Variational quantum circuits, quantum kernels

Quantum error correction — Logical qubits, syndrome decoding, fault-tolerant computation

Simulation — Classical simulation limits, tensor networks, GPU acceleration

📡 Sources & Monitoring
Source	What to Monitor
Qiskit releases	Stable releases
PennyLane releases	Quantum ML
Cirq releases	Framework development
CUDA-Q releases	GPU computing
arXiv quantum physics	New research
Qiskit ecosystem	Community projects
📚 References
Qiskit 2.6.2 on PyPI

PennyLane release notes

Qiskit ecosystem classifications

Qiskit Aer

Google Cirq

NVIDIA CUDA-Q

NVIDIA cuQuantum

📄 License
This project is licensed under the MIT License — a permissive open-source license. See the LICENSE file for details.

🤝 Contributing
This is an evolving academic portfolio. Contributions, suggestions, and research collaborations are welcome. Please open an issue or submit a pull request.

