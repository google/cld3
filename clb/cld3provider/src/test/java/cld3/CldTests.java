package cld3;

import org.junit.jupiter.api.AfterAll;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;

import static org.assertj.core.api.Assertions.assertThat;

public class CldTests {
    private static CldProviderLib.CldProviderImpl lib;

    private static void printEnv(String name) {
        System.out.println(name + ": " + System.getenv(name));
    }
    private static void printProp(String name) {
        System.out.println(name + ": " + System.getProperty(name));
    }

    @BeforeAll
    private static void beforeAll() throws Exception {
        lib = new CldProviderLib.CldProviderImpl();
    }

    @AfterAll
    private static void afterAll() throws Exception {
        if (lib != null) {
            lib.close();
        }
    }

    @Test
    void detectLanguageTest() throws Exception {
        printEnv("PATH");
        printEnv("LD_LIBRARY_PATH");
        printProp("java.library.path");

        String detectedLang = lib.detectLang("I like my round table");
        assertThat(detectedLang).isEqualTo("en");
    }
}
