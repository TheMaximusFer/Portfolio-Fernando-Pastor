package sisInf.T09.Enderarchive.controller;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import sisInf.T09.Enderarchive.model.User;
import sisInf.T09.Enderarchive.service.UserService;

@RestController
@CrossOrigin(origins = "http://localhost:5173")
@RequestMapping("/api/admin") // Ruta base para todas las acciones de admin
public class AdminController {

    @Autowired
    private UserService userService;

    /**
     * Promociona a un usuario a administrador.
     * Solo accesible por otros administradores.
     */
    @PutMapping("/promote/{username}")
    public ResponseEntity<?> promoteUser(@PathVariable String username) {
        try {
            User promotedUser = userService.promoteUserToAdmin(username);
            return ResponseEntity.ok(promotedUser);
        } catch (RuntimeException e) {
            return ResponseEntity.notFound().build();
        }
    }

    @PutMapping("/ban/{username}")
    public ResponseEntity<?> banUser(@PathVariable String username) {
        try{
            userService.banUser(username);
            return ResponseEntity.ok().build();
        } catch (RuntimeException e) {
            return ResponseEntity.notFound().build();
        }
    }

}
