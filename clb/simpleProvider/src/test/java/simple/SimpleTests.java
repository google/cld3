package simple;

import org.junit.jupiter.api.AfterAll;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;

import static org.assertj.core.api.Assertions.assertThat;

public class SimpleTests {
    private static SimpleProviderLib.SimpleProviderImpl lib;

    private static void printEnv(String name) {
        System.out.println(name + ": " + System.getenv(name));
    }
    private static void printProp(String name) {
        System.out.println(name + ": " + System.getProperty(name));
    }

    @BeforeAll
    private static void beforeAll() throws Exception {
        lib = new SimpleProviderLib.SimpleProviderImpl();
    }

    @AfterAll
    private static void afterAll() throws Exception {
        if (lib != null) {
            lib.close();
        }
    }

    @Test
    void customTest1() throws Exception {
        printEnv("PATH");
        printEnv("LD_LIBRARY_PATH");
        printProp("java.library.path");

        long simpleNum = lib.getSimpleNum();
        assertThat(simpleNum).isEqualTo(17);
    }
}
