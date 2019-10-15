package cld3;

import org.springframework.boot.CommandLineRunner;
import org.springframework.boot.SpringApplication;

public class Main implements CommandLineRunner {

    @Override
    public void run(String... args) throws Exception {
        try (final CldProviderLib.CldProviderImpl lib = new CldProviderLib.CldProviderImpl()) {
            String detectedLang = lib.detectLang("I like my round table");
            System.out.println("detectedLang: " + detectedLang);
        }
    }
    
    public static void main(String[] args) {
        SpringApplication.run(Main.class, args);
    }
}
