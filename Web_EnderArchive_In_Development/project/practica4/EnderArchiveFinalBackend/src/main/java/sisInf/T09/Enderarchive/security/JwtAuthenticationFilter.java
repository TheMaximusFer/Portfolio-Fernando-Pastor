package sisInf.T09.Enderarchive.security; // Asegúrate de que el paquete es el correcto

import jakarta.servlet.FilterChain;
import jakarta.servlet.ServletException;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.security.core.userdetails.UserDetailsService;
import org.springframework.security.web.authentication.WebAuthenticationDetailsSource;
import org.springframework.stereotype.Component;
import org.springframework.web.filter.OncePerRequestFilter;

import java.io.IOException;

@Component // Le dice a Spring que gestione esto como un Bean
public class JwtAuthenticationFilter extends OncePerRequestFilter { // Garantiza que se ejecuta 1 vez por petición

    private final JwtUtils jwtUtils;

    private final UserDetailsService userDetailsService;

    @Autowired
    public JwtAuthenticationFilter (JwtUtils jwtUtils, UserDetailsService userDetailsService) {
        this.jwtUtils = jwtUtils;
        this.userDetailsService = userDetailsService;
    }

    @Override
    protected void doFilterInternal(HttpServletRequest request,
                                    HttpServletResponse response,
                                    FilterChain filterChain)
            throws ServletException, IOException {

        // Obtenemos el token del header "Authorization"
        final String authHeader = request.getHeader("Authorization");
        final String token;
        final String username;

        // Si el header es nulo o no empieza con "Bearer ", pasamos al siguiente filtro
        // (Esto es lo que pasará con /api/auth/login, por ejemplo)
        if (authHeader == null || !authHeader.startsWith("Bearer ")) {
            filterChain.doFilter(request, response);
            return;
        }

        // Extraemos el token (quitando el "Bearer ")
        token = authHeader.substring(7); // "Bearer ".length() == 7

        try {
            // Validamos el token y extraemos el username
            if (jwtUtils.validateToken(token)) {
                username = jwtUtils.getUsernameFromToken(token);

                // Si tenemos username y el usuario NO está autenticado en el contexto actual...
                if (username != null && SecurityContextHolder.getContext().getAuthentication() == null) {

                    // Cargamos los detalles del usuario desde la BBDD (usando nuestro UserService)
                    UserDetails userDetails = this.userDetailsService.loadUserByUsername(username);

                    // Creamos un objeto de autenticación
                    UsernamePasswordAuthenticationToken authToken = new UsernamePasswordAuthenticationToken(
                            userDetails,
                            null, // No necesitamos credenciales (password) aquí
                            userDetails.getAuthorities() // Los roles (ADMIN, USER)
                    );

                    authToken.setDetails(new WebAuthenticationDetailsSource().buildDetails(request));

                    // ¡LA MAGIA! Establecemos al usuario en el SecurityContext
                    // Esto le dice a Spring Security: "Este usuario está autenticado para esta petición"
                    SecurityContextHolder.getContext().setAuthentication(authToken);
                }
            }
        } catch (Exception e) {
            // Si el token es inválido (expirado, malformado, etc.)
            // No hacemos nada, simplemente dejamos que la petición continúe sin autenticación.
            // Spring Security lo bloqueará más adelante si el endpoint es protegido.
        }

        // Pasamos la petición al siguiente filtro en la cadena
        filterChain.doFilter(request, response);
    }
}