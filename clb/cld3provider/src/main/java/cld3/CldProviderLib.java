package cld3;

import org.bytedeco.javacpp.Loader;
import org.bytedeco.javacpp.Pointer;
import org.bytedeco.javacpp.annotation.Platform;
import org.bytedeco.javacpp.annotation.Properties;
import org.bytedeco.javacpp.annotation.Namespace;
import org.bytedeco.javacpp.annotation.StdString;

@Properties(target = "cld3.CldProviderLib", value = {
    @Platform(include = "cld3_lib.h")
    //, @Platform(value = "windows-x86_64", preload = { "icudt49", "icuuc49", "icuin49", "icuio49" })
    //, @Platform(value = "linux-x86_64", preload = { "icudata", "icuuc", "icui18n", "icuio" })
})
@Namespace("cld3")
public class CldProviderLib {

    public static class CldProviderImpl extends Pointer implements CldProvider {
        static {
            // load(getCallerClass(2), loadProperties(), Loader.pathsFirst);
            Loader.load(CldProviderImpl.class);
        }

        public CldProviderImpl() { allocate(); }
        private native void allocate();

        // custom part
        private native @StdString String detectLang0(@StdString String text);

        @Override
        public String detectLang(String text) {
            return detectLang0(text);
        }
    }
}
