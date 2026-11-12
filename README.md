<div align="center">

# ⚛️ Quantum Computing Lab

**An academic portfolio exploring quantum vs. classical computation: algorithms, simulation, noise, and complexity.**

![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)
![Qiskit](https://img.shields.io/badge/Qiskit-2.6.2-6929C4)
![Platform](https://img.shields.io/badge/Platform-Fedora%20%7C%20JupyterLab-blue)
![Status](https://img.shields.io/badge/Status-Active%20Research-orange)

[Overview](#-overview) •
[Structure](#-repository-structure) •
[Setup](#%EF%B8%8F-environment-setup) •
[Experiments](#-practical-laboratory-activities) •
[Research](#-research-areas) •
[Contributing](#-contributing)

</div>

---

## 📖 Overview

This repository is an evolving collection of notebooks, experiments, and research notes on quantum computing. It progresses from fundamentals (qubits, gates, entanglement) through implementation in multiple frameworks (Qiskit, Cirq, PennyLane) to rigorous comparisons between quantum and classical approaches.

**Key goals**

- Build a stable, reproducible baseline with **Qiskit 2.6.2**
- Compare frameworks on identical circuits and algorithms
- Separate **theoretical** query advantage from **actual** wall-clock performance
- Study noise, error correction, and the limits of classical simulation

---

## 📁 Repository Structure

```text
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
```

| Module | Focus |
|:--|:--|
| `01_quantum_fundamentals` | Qubits, superposition, entanglement, gates |
| `02_qiskit` | Bell and GHZ states, circuit construction |
| `03_quantum_algorithms` | Deutsch–Jozsa, Bernstein–Vazirani, Grover, Shor |
| `04_classical_vs_quantum` | Complexity classes and algorithm comparisons |
| `05_quantum_machine_learning` | Introductory QML |
| `06_simulation_and_noise` | Noise models and error analysis |
| `research/` | Literature review, BibTeX references, notes |
| `results/` | Figures and experiment data |

---

## ⚙️ Environment Setup

**Platform:** Fedora / JupyterLab

### 1. Install the quantum packages

Run in a notebook cell:

```bash
%pip install qiskit qiskit-aer pennylane cirq
```

Restart the kernel afterwards.

### 2. Verify the environment

```python
import qiskit
import qiskit_aer
import pennylane as qml
import cirq

print("Qiskit:", qiskit.__version__)
print("Qiskit Aer:", qiskit_aer.__version__)
print("PennyLane:", qml.__version__)
print("Cirq:", cirq.__version__)
print("Quantum environment ready!")
```

### 3. Check installed versions

```python
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
```

> [!TIP]
> For Qiskit 2.6.2, use a separate test environment if you want to upgrade without disrupting existing notebooks.

---

## 🧪 Practical Laboratory Activities

### Experiment A: Qiskit 2.6.2 Baseline (Bell State)

Test the current stable release and implement a Bell-state circuit.

```python
from qiskit import QuantumCircuit
from qiskit_aer import AerSimulator

qc = QuantumCircuit(2, 2)
qc.h(0)
qc.cx(0, 1)
qc.measure([0, 1], [0, 1])

simulator = AerSimulator()
result = simulator.run(qc, shots=1024).result()
print(result.get_counts())
```

**Expected dominant outcomes:** `00` and `11`

### Experiment B: Cross-Framework Comparison

Implement the same Bell-state circuit in **Qiskit** and **Cirq**, then compare:

- Circuit representations
- Measurement distributions
- Execution times

### Experiment C: Classical vs. Quantum (Grover's Algorithm)

Implement Grover's algorithm and compare query complexity against classical unstructured search.

> [!NOTE]
> Keep theoretical query advantage separate from actual wall-clock performance.

---

## 📋 Development Priorities

| # | Framework | Goal |
|:-:|:--|:--|
| 1 | **Qiskit 2.6.2** | Establish the stable baseline and implement fundamental quantum circuits |
| 2 | **Qiskit Aer** | Benchmark noise models and simulator performance, documenting its maintenance limitations |
| 3 | **Cirq** | Build a cross-framework implementation of selected algorithms |
| 4 | **PennyLane** | Explore hybrid quantum-classical algorithms and variational circuits |
| 5 | **CUDA-Q** | Investigate GPU acceleration and quantum resource estimation |

---

## 🔬 Research Areas

| Area | Topics |
|:--|:--|
| **Quantum complexity** | BQP vs. BPP, oracle separations, computational advantage |
| **Quantum algorithms** | Shor, Grover, Bernstein–Vazirani, Deutsch–Jozsa |
| **Quantum machine learning** | Variational quantum circuits, quantum kernels |
| **Quantum error correction** | Logical qubits, syndrome decoding, fault-tolerant computation |
| **Simulation** | Classical simulation limits, tensor networks, GPU acceleration |

---

## 📡 Sources & Monitoring

| Source | What to Monitor |
|:--|:--|
| Qiskit releases | Stable releases |
| PennyLane releases | Quantum ML |
| Cirq releases | Framework development |
| CUDA-Q releases | GPU computing |
| arXiv quantum physics | New research |
| Qiskit ecosystem | Community projects |

---

## 📚 References

- [Qiskit 2.6.2 on PyPI](https://pypi.org/project/qiskit/)
- [PennyLane release notes](https://docs.pennylane.ai/en/stable/development/release_notes.html)
- [Qiskit ecosystem](https://qiskit.org/ecosystem)
- [Qiskit Aer](https://github.com/Qiskit/qiskit-aer)
- [Google Cirq](https://github.com/quantumlib/Cirq)
- [NVIDIA CUDA-Q](https://github.com/NVIDIA/cuda-quantum)
- [NVIDIA cuQuantum](https://github.com/NVIDIA/cuQuantum)

---

## 📄 License

This project is licensed under the **MIT License**, a permissive open-source license. See the [LICENSE](LICENSE) file for details.

---

## 🤝 Contributing

This is an evolving academic portfolio. Contributions, suggestions, and research collaborations are welcome. Please [open an issue](../../issues) or submit a pull request.
