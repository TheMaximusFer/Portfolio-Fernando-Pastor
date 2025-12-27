package sisInf.T09.Enderarchive.config;

import org.springframework.context.annotation.Configuration;
import org.springframework.web.servlet.config.annotation.ResourceHandlerRegistry;
import org.springframework.web.servlet.config.annotation.WebMvcConfigurer;

/**
 * Esta clase de configuración le dice a Spring Boot cómo y dónde
 * servir los archivos estáticos que los usuarios han subido (imágenes, etc.).
 */
@Configuration
public class StaticResourceConfig implements WebMvcConfigurer {

    /**
     * Este método es el que Spring llama para registrar los "manejadores de recursos".
     */
    @Override
    public void addResourceHandlers(ResourceHandlerRegistry registry) {

        // REGISTRAMOS NUESTRO MANEJADOR
        registry.addResourceHandler("/uploads/**")
                .addResourceLocations("file:./uploads/");
    }
}