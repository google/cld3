#include "cld_3/protos/sentence.pb.h"

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/extension_set.h>

namespace internal
{
// need this in order to avoid transitive (cld3->protobuf) missing methods errors
void
neverCallMeInitHelper()
{
    static std::string sES = google::protobuf::internal::kEmptyString;
    google::protobuf::internal::ExtensionSet extSet;
    chrome_lang_id::Token t;
    std::string eStr = t.InitializationErrorString();
}

} // internal
