package sisInf.T09.Enderarchive.config; // (O la ruta de tu paquete de config)

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.security.crypto.bcrypt.BCryptPasswordEncoder;
import org.springframework.security.crypto.password.PasswordEncoder;

@Configuration
public class ApplicationConfig {

    @Bean
    public PasswordEncoder passwordEncoder() {
        // Esta es la herramienta estándar y más recomendada para hashear contraseñas
        return new BCryptPasswordEncoder();
    }

    // (Aquí podríamos poner otros beans "generales" en el futuro)
}