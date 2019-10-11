package simple;

import org.bytedeco.javacpp.Loader;
import org.bytedeco.javacpp.Pointer;
import org.bytedeco.javacpp.annotation.Platform;
import org.bytedeco.javacpp.annotation.Properties;
import org.bytedeco.javacpp.annotation.Namespace;

@Properties(target = "simple.SimpleProviderLib", value = {
    @Platform(include = "simple_lib.h")
    //, @Platform(value = "windows-x86_64", preload = { "icudt49", "icuuc49", "icuin49", "icuio49" })
    //, @Platform(value = "linux-x86_64", preload = { "icudata", "icuuc", "icui18n", "icuio" })
})
@Namespace("simple")
public class SimpleProviderLib {

    public static class SimpleProviderImpl extends Pointer implements SimpleProvider {
        static {
            // load(getCallerClass(2), loadProperties(), Loader.pathsFirst);
            Loader.load(SimpleProviderImpl.class);
        }

        public SimpleProviderImpl() { allocate(); }
        private native void allocate();

        // custom part
        private native long getSimpleNum0();

        @Override
        public long getSimpleNum() {
            return getSimpleNum0();
        }
    }
}
