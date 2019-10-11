package simple;

import org.springframework.boot.CommandLineRunner;
import org.springframework.boot.SpringApplication;

public class Main implements CommandLineRunner {

    @Override
    public void run(String... args) throws Exception {
        try (final SimpleProviderLib.SimpleProviderImpl lib = new SimpleProviderLib.SimpleProviderImpl()) {
            long simpleNum = lib.getSimpleNum();
            System.out.println("getSimpleNum: " + simpleNum);
        }
    }
    
    public static void main(String[] args) {
        SpringApplication.run(Main.class, args);
    }
}
