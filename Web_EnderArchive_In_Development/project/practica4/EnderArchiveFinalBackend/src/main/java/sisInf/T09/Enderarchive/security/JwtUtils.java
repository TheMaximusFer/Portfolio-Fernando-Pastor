package sisInf.T09.Enderarchive.security; // Asegúrate de que el paquete sea el correcto

import io.jsonwebtoken.*;
import io.jsonwebtoken.security.Keys;
import io.jsonwebtoken.security.SecurityException;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.stereotype.Component;

import java.security.Key;
import java.util.Date;

@Component
public class JwtUtils {

    // NUEVO: Un logger para registrar errores de validación
    private static final Logger logger = LoggerFactory.getLogger(JwtUtils.class);

    @Value("${jwt.secret}")
    private String jwtSecret;

    private int jwtExpirationMs = 3600000; // 1 hora

    // Método helper privado para obtener la clave de firma
    private Key getKey() {
        return Keys.hmacShaKeyFor(jwtSecret.getBytes());
    }

    /**
     * Genera un token JWT para un usuario.
     */
    public String generateToken(UserDetails userDetails) {
        String username = userDetails.getUsername();
        Date now = new Date();
        Date expiryDate = new Date(now.getTime() + jwtExpirationMs);

        return Jwts.builder()
                .setSubject(username)
                .setIssuedAt(now)
                .setExpiration(expiryDate)
                .signWith(getKey(), SignatureAlgorithm.HS256) // MODIFICADO: Usa el helper getKey()
                .compact();
    }

    // --- MÉTODOS NUEVOS PARA VALIDACIÓN ---

    /**
     * Extrae el nombre de usuario (subject) de un token JWT válido.
     */
    public String getUsernameFromToken(String token) {
        Claims claims = Jwts.parserBuilder()
                .setSigningKey(getKey())
                .build()
                .parseClaimsJws(token)
                .getBody();
        return claims.getSubject();
    }

    /**
     * Valida un token JWT. Comprueba firma y expiración.
     */
    public boolean validateToken(String token) {
        try {
            Jwts.parserBuilder()
                    .setSigningKey(getKey())
                    .build()
                    .parseClaimsJws(token);
            // Si no lanza excepción, el token es válido
            return true;
        } catch (SecurityException e) {
            logger.error("Firma JWT inválida: {}", e.getMessage());
        } catch (MalformedJwtException e) {
            logger.error("Token JWT malformado: {}", e.getMessage());
        } catch (ExpiredJwtException e) {
            logger.error("Token JWT ha expirado: {}", e.getMessage());
        } catch (UnsupportedJwtException e) {
            logger.error("Token JWT no soportado: {}", e.getMessage());
        } catch (IllegalArgumentException e) {
            logger.error("Claims del JWT están vacíos: {}", e.getMessage());
        }

        // Si atrapa cualquier excepción, el token es inválido
        return false;
    }
}