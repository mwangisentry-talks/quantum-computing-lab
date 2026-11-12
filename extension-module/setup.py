import qiskit
from setuptools import setup, Extension

core_ext = Extension(
    # The fully qualified module name of the extension.
    name="spectator_measures._core",
    # The C source files needed for the extension.  The file
    # name is conventionally `<mod>module.c`, where `<mod>`
    # is the module name (`_core`, in this case).
    sources=["src/spectator_measures/_coremodule.c"],
    # Directories containing additional header files used in
    # the build process.
    include_dirs=[qiskit.capi.get_include()],
)
setup(ext_modules=[core_ext])
