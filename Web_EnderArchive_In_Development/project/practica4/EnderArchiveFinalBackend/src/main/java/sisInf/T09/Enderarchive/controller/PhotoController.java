package sisInf.T09.Enderarchive.controller;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.security.core.annotation.AuthenticationPrincipal;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.multipart.MultipartFile;
import sisInf.T09.Enderarchive.model.Photo;
import sisInf.T09.Enderarchive.service.PhotoService;
import sisInf.T09.Enderarchive.service.WorldService;

import java.util.List;

@RestController
@CrossOrigin(origins = "http://localhost:5173")
@RequestMapping("/api/worlds")
public class PhotoController {

    private PhotoService photoService;
    private WorldService worldService;

    @Autowired
    public PhotoController(PhotoService photoService, WorldService worldService) {
        this.photoService = photoService;
        this.worldService = worldService;
    }

    @GetMapping("/{worldOwner}/{worldName}/photos")
    public ResponseEntity<List<Photo>> getPhotos(@PathVariable String worldOwner, @PathVariable String worldName) {
        try {
            List<Photo> list = photoService.getPhotosPublic(worldOwner, worldName);
            return ResponseEntity.ok().body(list);
        } catch (RuntimeException e) {
            return ResponseEntity.notFound().build();
        }
    }

    @GetMapping("/my-worlds/{worldName}/photos")
    public ResponseEntity<List<Photo>> getMyPhotos(@PathVariable String worldName, @AuthenticationPrincipal UserDetails user) {
        String username = user.getUsername();
        try {
            List<Photo> list = photoService.getPhotos(username, worldName);
            return ResponseEntity.ok().body(list);
        } catch (RuntimeException e) {
            return ResponseEntity.notFound().build();
        }
    }

    @PostMapping("/my-worlds/{worldName}/photos")
    public ResponseEntity<?> uploadWorldPhoto(
            @PathVariable String worldName,
            @RequestParam("file") MultipartFile file,
            @AuthenticationPrincipal UserDetails userDetails) {

        String username = userDetails.getUsername();

        try {
            // Validar que sea una imagen
            if (!file.getContentType().startsWith("image/")) {
                return ResponseEntity.badRequest().body("El archivo debe ser una imagen.");
            }

            Photo savedPhoto = photoService.uploadPhoto(username, worldName, file);

            // Devolvemos el objeto Photo creado (con ID y URL)
            return ResponseEntity.status(HttpStatus.CREATED).body(savedPhoto);

        } catch (RuntimeException e) {
            return ResponseEntity.status(HttpStatus.INTERNAL_SERVER_ERROR)
                    .body("Error al subir la foto: " + e.getMessage());
        }
    }

    @DeleteMapping("/my-worlds/{worldName}/photos")
    public ResponseEntity<Void> deletePhoto(@PathVariable String worldName, @RequestBody Photo photo,@AuthenticationPrincipal UserDetails user) {
        String username = user.getUsername();
        try{
            photoService.deletePhoto(photo, worldName, username);
            return ResponseEntity.noContent().build();
        }catch (RuntimeException e){
            return ResponseEntity.notFound().build();
        }
    }
}
