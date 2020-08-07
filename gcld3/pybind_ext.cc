#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>

#include "../src/nnet_language_identifier.h"

namespace pybind11 {

using chrome_lang_id::NNetLanguageIdentifier;

// This is conventional.
namespace py = pybind11;

PYBIND11_MODULE(pybind_ext, py_module) {
  py::class_<NNetLanguageIdentifier>(py_module, "NNetLanguageIdentifier")
      .def(py::init<const int, const int>(), py::arg("min_num_bytes"),
           py::arg("max_num_bytes"))
      .def("FindLanguage", &NNetLanguageIdentifier::FindLanguage,
           py::arg("text"))
      .def("FindTopNMostFreqLangs",
           &NNetLanguageIdentifier::FindTopNMostFreqLangs, py::arg("text"),
           py::arg("num_langs"))
      .def_readonly_static("kUnknown", &NNetLanguageIdentifier::kUnknown)
      .def_readonly_static("kMinNumBytesToConsider",
                           &NNetLanguageIdentifier::kMinNumBytesToConsider)
      .def_readonly_static("kMaxNumBytesToConsider",
                           &NNetLanguageIdentifier::kMaxNumBytesToConsider)
      .def_readonly_static("kMaxNumInputBytesToConsider",
                           &NNetLanguageIdentifier::kMaxNumInputBytesToConsider)
      .def_readonly_static("kReliabilityThreshold",
                           &NNetLanguageIdentifier::kReliabilityThreshold)
      .def_readonly_static("kReliabilityHrBsThreshold",
                           &NNetLanguageIdentifier::kReliabilityHrBsThreshold);

  py::class_<NNetLanguageIdentifier::Result>(py_module, "Result")
      .def_readwrite("language", &NNetLanguageIdentifier::Result::language)
      .def_readwrite("probability",
                     &NNetLanguageIdentifier::Result::probability)
      .def_readwrite("is_reliable",
                     &NNetLanguageIdentifier::Result::is_reliable)
      .def_readwrite("proportion", &NNetLanguageIdentifier::Result::proportion);
}
}  // namespace pybind11
