package sisInf.T09.Enderarchive.controller;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.security.core.annotation.AuthenticationPrincipal;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.security.core.userdetails.UsernameNotFoundException;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.multipart.MultipartFile;
import sisInf.T09.Enderarchive.model.User;
import sisInf.T09.Enderarchive.service.FileStorageService;
import sisInf.T09.Enderarchive.service.UserService;

import java.util.Map;

@RestController
@CrossOrigin(origins = "http://localhost:5173")
@RequestMapping("/api/users") //Prefijo de URL para todos los métodos
public class UserController {

    //El Camarero (Controller) solo habla con el Cocinero (Service)
    private final UserService userService;
    private final FileStorageService fileStorageService;

    private ResponseEntity<User> returnEntity(HttpStatus statusError, HttpStatus statusOk, User user) {
        if(user == null){
            return ResponseEntity.status(statusError).build();
        }else{
            return ResponseEntity.status(statusOk).body(user);
        }
    }

    @Autowired
    public UserController(UserService userService,  FileStorageService fileStorageService) {
        this.userService = userService;
        this.fileStorageService = fileStorageService;
    }


    // READ (Leer todos)
    @GetMapping
    public ResponseEntity<Object> getAllUsers(@RequestParam(value = "username", required = false) String username) {
        if(username == null){
            return ResponseEntity.ok().body(userService.getAllUsers());
        }else {
            try {
                return ResponseEntity.ok().body(userService.getUserByUsername(username));
            } catch (UsernameNotFoundException e) {
                return ResponseEntity.notFound().build();
            }
        }
    }

    // READ (Leer uno por ID/Username)
    @GetMapping("/{username}")
    public ResponseEntity<User> getUserByUsername(@PathVariable("username") String username) {
        User user = userService.getUserByUsername(username);
        return returnEntity(HttpStatus.NOT_FOUND, HttpStatus.OK, user);
    }

    // Create
    @PostMapping
    public ResponseEntity<User> createUser(@RequestBody User user) {
        User newUser = userService.createUser(user);
        return returnEntity(HttpStatus.CONFLICT, HttpStatus.CREATED, newUser);
    }

    // UPDATE FOR DEBUG
    @PutMapping("/debug/{username}")
    public ResponseEntity<User> updateUserDebug(@PathVariable("username") String username, @RequestBody User user) {
        User foundUser = userService.updateUserDebug(username, user);
        return returnEntity(HttpStatus.NOT_FOUND, HttpStatus.OK, foundUser);
    }

    @PutMapping
    public ResponseEntity<User> updateUser(@AuthenticationPrincipal UserDetails userDetails, @RequestBody User user) {
        user.setUsername(userDetails.getUsername());
        User foundUser = userService.updateUser(user);
        return returnEntity(HttpStatus.NOT_FOUND, HttpStatus.OK, foundUser);
    }

    //DELETE
    @DeleteMapping("/{username}")
    public ResponseEntity<Void> deleteUser(@PathVariable("username") String username) {
        if(userService.deleteUser(username)){
            return ResponseEntity.noContent().build();
        }else{
            return ResponseEntity.notFound().build();
        }
    }

    @PostMapping("/me/profile-picture")
    public ResponseEntity<?> uploadProfilePicture(
            @RequestParam("file") MultipartFile file,
            @AuthenticationPrincipal UserDetails userDetails) {

        String username = userDetails.getUsername();
        try {
            if (!file.getContentType().startsWith("image/")) {
                return ResponseEntity.badRequest().body("El archivo debe ser una imagen.");
            }

            String filePath = fileStorageService.saveProfilePicture(file, username);

            userService.updateProfilePicturePath(username, filePath);

            return ResponseEntity.ok(Map.of("filePath", filePath));

        } catch (Exception e) {
            return ResponseEntity.status(500).body("Error al subir el archivo: " + e.getMessage());
        }
    }

}