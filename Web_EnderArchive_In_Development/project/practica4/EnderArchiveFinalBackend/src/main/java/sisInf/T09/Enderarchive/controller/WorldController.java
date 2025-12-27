package sisInf.T09.Enderarchive.controller;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.security.core.annotation.AuthenticationPrincipal;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.multipart.MultipartFile;
import sisInf.T09.Enderarchive.model.World;
import sisInf.T09.Enderarchive.service.FileStorageService;
import sisInf.T09.Enderarchive.service.WorldService;

import java.util.List;
import java.util.Map;
import java.util.Objects;

@RestController
@CrossOrigin(origins = "http://localhost:5173")
@RequestMapping("/api/worlds")
public class WorldController {

    private final WorldService worldService;
    private final FileStorageService fileStorageService;

    @Autowired
    public WorldController(WorldService worldService,  FileStorageService fileStorageService) {
        this.worldService = worldService;
        this.fileStorageService = fileStorageService;
    }

    @GetMapping("/my-worlds")
    public ResponseEntity<Object> findAll(@AuthenticationPrincipal UserDetails user, @RequestParam(value = "world", required = false) String worldName,
                                          @RequestParam(value = "adventure", required = false) Boolean adventureFilter,
                                          @RequestParam(value = "survival", required = false) Boolean survivalFilter,
                                          @RequestParam(value = "skyblock", required = false) Boolean skyblockFilter,
                                          @RequestParam(value = "fantasy", required = false) Boolean fantasyFilter,
                                          @RequestParam(value = "version", required = false) String minecraftVersion,
                                          @RequestParam(value = "java", required = false) Boolean javaFilter,
                                          @RequestParam(value = "search", required = false) String worldSearched){
        String username = user.getUsername();
        if(worldName != null && !worldName.isEmpty()){
            try{
                return ResponseEntity.ok().body(worldService.getWorldByOwner(username, worldName));

            }catch(RuntimeException e){
                return ResponseEntity.notFound().build();
            }
        }else{
            if(javaFilter == null || minecraftVersion== null){
                return ResponseEntity.ok().body(worldService.getWorldsByOwner(username));
            }else{
                try{
                    return ResponseEntity.ok().body(
                            worldService.findMyWorldsByCriteria(
                                    username, adventureFilter, survivalFilter, skyblockFilter,
                                    fantasyFilter, minecraftVersion, javaFilter, worldSearched)
                    );
                } catch (RuntimeException e) {
                    return  ResponseEntity.notFound().build();
                }
            }
        }
    }

    @GetMapping
    public ResponseEntity<Object> findWorld(@RequestParam(value = "username", required = true) String username, @RequestParam(value = "world",  required = false) String worldName) {
        if(worldName == null){
            try{
                return ResponseEntity.ok().body(worldService.getPublicWorldsByOwner(username));
            }catch(RuntimeException e){
                return ResponseEntity.notFound().build();
            }
        }else{
            try{
                return ResponseEntity.ok().body(worldService.getPublicWorldByOwner(username, worldName));
            } catch (RuntimeException e) {
                return  ResponseEntity.notFound().build();
            }
        }
    }

    @GetMapping ("/public")
    public ResponseEntity<List<World>> findAllPublic(@RequestParam(value = "adventure", required = false) Boolean adventureFilter,
                                     @RequestParam(value = "survival", required = false) Boolean survivalFilter,
                                     @RequestParam(value = "skyblock", required = false) Boolean skyblockFilter,
                                     @RequestParam(value = "fantasy", required = false) Boolean fantasyFilter,
                                     @RequestParam(value = "version", required = true) String minecraftVersion,
                                     @RequestParam(value = "java", required = true) Boolean javaFilter,
                                     @RequestParam(value = "search", required = false) String worldSearched) {
        List<World> worlds = worldService.findPublicWorldByCriteria(adventureFilter, survivalFilter, skyblockFilter, fantasyFilter, minecraftVersion, javaFilter, worldSearched);
        return ResponseEntity.ok().body(worlds);
    }



    @PostMapping("/my-worlds")
    public ResponseEntity<World> createWorld(@RequestBody World newWorld, @AuthenticationPrincipal UserDetails userDetails) {

        // Obtenemos el 'quién' del token
        String username = userDetails.getUsername();
        try{
            // Le pasamos el 'qué' (newWorld) y el 'quién' (username) al servicio
            World mundoCreado = worldService.createWorld(newWorld, username);
            // Devolvemos un 201 Created (buena práctica para POST)
            return ResponseEntity.status(HttpStatus.CREATED).body(mundoCreado);
        }catch (Exception ex){
            if(ex.getMessage().equals("Usuario no encontrado: " + username)){
                return new ResponseEntity<>(HttpStatus.NOT_FOUND);
            }else{
                return new ResponseEntity<>(HttpStatus.CONFLICT);
            }
        }
    }

    @DeleteMapping("/my-worlds/{worldName}")
    public ResponseEntity<?> deleteWorld(@PathVariable String worldName,
                                         @AuthenticationPrincipal UserDetails userDetails) {

        // Obtenemos el username del token JWT
        String authenticatedUsername = userDetails.getUsername();

        try {
            // Pasamos el nombre del mundo y el nombre del dueño al servicio
            worldService.deleteWorld(worldName, authenticatedUsername);

            // Si tiene éxito, devolvemos 204 No Content (estándar para DELETE)
            return ResponseEntity.noContent().build();

        } catch (RuntimeException e) {
            // Si el servicio lanza una excepción (ej. "no encontrado" o "no te pertenece")
            // devolvemos un 404 Not Found.
            return ResponseEntity.status(HttpStatus.NOT_FOUND).body(e.getMessage());
        }
    }

    @PutMapping("/my-worlds")
    public ResponseEntity<?> updateWorld(@RequestBody World updatedData,
                                         @AuthenticationPrincipal UserDetails userDetails) {

        String authenticatedUsername = userDetails.getUsername();

        try {
            // Pasamos toda la información al servicio
            World mundoActualizado = worldService.updateWorld(authenticatedUsername, updatedData);

            // Devolvemos 200 OK con el mundo actualizado
            return ResponseEntity.ok(mundoActualizado);

        } catch (RuntimeException e) {
            // Devolvemos 404 si no se encontró o no le pertenece
            return ResponseEntity.status(HttpStatus.NOT_FOUND).body(e.getMessage());
        }
    }

    @PostMapping("/my-worlds/{worldName}/file")
    public ResponseEntity<?> uploadWorldFile(
            @PathVariable String worldName,
            @RequestParam("file") MultipartFile file,
            @AuthenticationPrincipal UserDetails userDetails) {

        String username = userDetails.getUsername();
        try {
            // (Aquí deberías validar que el archivo es un .zip, .rar, etc.)
            String fileExtension = fileStorageService.getFileExtension(file.getOriginalFilename());
            if(!Objects.equals(fileExtension, ".zip")){
                return ResponseEntity.badRequest().body("El archivo debe ser .zip");
            }

            // guardamos el archivo y tomamos su filepath
            String filePath = fileStorageService.saveWorldFile(file, username, worldName);

            // Guardamos la ruta en la entidad 'World'
            worldService.updateWorldFilePath(username, worldName, filePath);

            return ResponseEntity.ok(Map.of("filePath", filePath));

        } catch (Exception e) {
            return ResponseEntity.status(500).body("Error al subir el archivo: " + e.getMessage());
        }
    }

    @PostMapping("/my-worlds/{worldName}/miniature")
    public ResponseEntity<?> uploadWorldMiniature(
            @PathVariable String worldName,
            @RequestParam("file") MultipartFile file,
            @AuthenticationPrincipal UserDetails userDetails) {

        String username = userDetails.getUsername();
        try {
            if (!file.getContentType().startsWith("image/")) {
                return ResponseEntity.badRequest().body("El archivo debe ser una imagen.");
            }

            // ¡Llamamos al método específico!
            String filePath = fileStorageService.saveWorldMiniature(file, username, worldName);

            worldService.updateWorldMiniaturePath(username, worldName, filePath);

            return ResponseEntity.ok(Map.of("filePath", filePath));

        } catch (Exception e) {
            return ResponseEntity.status(500).body("Error al subir el archivo: " + e.getMessage());
        }
    }
}