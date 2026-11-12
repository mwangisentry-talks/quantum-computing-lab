from qiskit.transpiler import TransformationPass, Target
from . import _core

__version__ = "0.0.1"
__all__ = ["AddSpectatorMeasures"]


class AddSpectatorMeasures(TransformationPass):
    def __init__(
        self,
        target: Target,
        *,
        include_unmeasured: bool = False,
        creg_name: str | None = None,
        add_barrier: bool = True
    ):
        super().__init__()
        self.target = target
        self.include_unmeasured = include_unmeasured
        self.creg_name = creg_name
        self.add_barrier = add_barrier

    def run(self, dag):
        # Delegate to our C extension module.
        _core.add_spectator_measures(
            dag,
            self.target,
            include_unmeasured=self.include_unmeasured,
            creg_name=self.creg_name,
            add_barrier=self.add_barrier,
        )
        return dag
