package sisInf.T09.Enderarchive.config;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.http.HttpMethod;
import org.springframework.security.authentication.AuthenticationManager;
import org.springframework.security.config.Customizer;
import org.springframework.security.config.annotation.authentication.configuration.AuthenticationConfiguration;
import org.springframework.security.config.annotation.web.builders.HttpSecurity;
import org.springframework.security.config.annotation.web.configuration.EnableWebSecurity;
import org.springframework.security.config.http.SessionCreationPolicy;
import org.springframework.security.web.SecurityFilterChain;
import org.springframework.security.web.authentication.UsernamePasswordAuthenticationFilter;
import sisInf.T09.Enderarchive.security.JwtAuthenticationFilter;
import com.fasterxml.jackson.datatype.hibernate5.jakarta.Hibernate5JakartaModule;

@Configuration      // Le dice a Spring que esta es una clase de configuración
@EnableWebSecurity  // Activa la configuración de seguridad web de Spring
public class SecurityConfig {

    private final JwtAuthenticationFilter jwtAuthenticationFilter;

    @Autowired
    public SecurityConfig(JwtAuthenticationFilter jwtAuthenticationFilter) {
        this.jwtAuthenticationFilter = jwtAuthenticationFilter;
    }

    @Bean // Define un "Bean" (objeto gestionado por Spring) con nuestras reglas
    public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {

        http
                // Desactivamos CSRF. Es necesario para que nuestra API REST
                // pueda recibir peticiones POST desde el frontend de React.
                .csrf(csrf -> csrf.disable())

                .cors(cors -> cors.configure(http)) // Le dice a Spring Security que aplique la config. CORS
                //TODO Hacer el filtro que evita que un usuario se autentifique más de una vez
                // Aquí definimos las reglas de autorización (quién puede acceder a qué)
                .authorizeHttpRequests(auth -> auth

                        // Permite a CUALQUIERA (permitAll) hacer una petición
                        // a la ruta "/api/users" y "/api/auth/login"
                        .requestMatchers( "/api/users/**").permitAll()
                        .requestMatchers("/api/auth/**").anonymous()
                        .requestMatchers("/api/favourites/**").authenticated()
                        .requestMatchers(HttpMethod.POST, "/api/reports").authenticated()
                        .requestMatchers(HttpMethod.GET, "/api/reports").hasRole("ADMIN")
                        .requestMatchers(HttpMethod.DELETE, "/api/reports/**").hasRole("ADMIN")
                        .requestMatchers("/api/admin/**").hasRole("ADMIN")
                        .requestMatchers(HttpMethod.GET, "/api/worlds/my-worlds/**").authenticated()
                        .requestMatchers(HttpMethod.GET,"/api/worlds/**").permitAll()
                        .requestMatchers(HttpMethod.POST,"/api/worlds/**").authenticated()
                        .requestMatchers(HttpMethod.PUT,"/api/worlds/**").authenticated()
                        .requestMatchers(HttpMethod.DELETE,"/api/worlds/**").authenticated()
                        .requestMatchers(HttpMethod.GET, "/uploads/**").permitAll()
                        .requestMatchers(HttpMethod.POST, "/api/users/me/profile-picture").authenticated()

                        // REGLA GENERAL:
                        // Para CUALQUIER OTRA petición (anyRequest), el usuario
                        // debe estar autenticado (authenticated).
                        .anyRequest().authenticated()
                )
        // Le decimos a Spring que no maneje sesiones (seremos "stateless")
        .sessionManagement(session -> session.sessionCreationPolicy(SessionCreationPolicy.STATELESS))

        .addFilterBefore(jwtAuthenticationFilter, UsernamePasswordAuthenticationFilter.class);

        // Construimos y devolvemos la configuración de seguridad
        return http.build();
    }

    @Bean
    public AuthenticationManager authenticationManager(AuthenticationConfiguration configuration) throws Exception {
        return configuration.getAuthenticationManager();
    }

    @Bean
    public com.fasterxml.jackson.databind.Module hibernateModule() {
        return new Hibernate5JakartaModule();
    }

}