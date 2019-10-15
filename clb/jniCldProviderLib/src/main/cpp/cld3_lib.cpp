#include "cld3_lib.h"

#include "nnet_language_identifier.h"

namespace cld3
{

std::string
CldProviderImpl::detectLang0(const std::string &text)
{
    chrome_lang_id::NNetLanguageIdentifier langIdentifier(0, 1000);
    const chrome_lang_id::NNetLanguageIdentifier::Result langIdResult = langIdentifier.FindLanguage(text);

    return langIdResult.language;
}

} // cld3
