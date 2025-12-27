package sisInf.T09.Enderarchive.controller;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.security.core.annotation.AuthenticationPrincipal;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.multipart.MultipartFile;
import sisInf.T09.Enderarchive.model.Marker;
import sisInf.T09.Enderarchive.service.FileStorageService;
import sisInf.T09.Enderarchive.service.MarkerService;

import java.util.List;
import java.util.Map;

@RestController
@CrossOrigin(origins = "http://localhost:5173")
@RequestMapping("/api/worlds")
public class MarkerController {
    MarkerService markerService;
    FileStorageService fileStorageService;

    @Autowired
    public MarkerController(MarkerService markerService,  FileStorageService fileStorageService) {
        this.markerService = markerService;
        this.fileStorageService = fileStorageService;
    }

    @PostMapping("/my-worlds/{worldName}/markers")
    public ResponseEntity<Marker> createMarker(@PathVariable String worldName, @RequestBody Marker marker, @AuthenticationPrincipal UserDetails user) {
        String username = user.getUsername();
        try{
            Marker createdMarker = markerService.createMarker(worldName, marker, username);

            return ResponseEntity.status(HttpStatus.CREATED).body(createdMarker);
        }catch(Exception ex){
            return ResponseEntity.status(HttpStatus.NOT_FOUND).build();
        }
    }

    @GetMapping("/{worldOwner}/{worldName}/markers")
    public ResponseEntity<List<Marker>> getMarkers(@PathVariable String worldName, @PathVariable String worldOwner) {
        try{
            List<Marker> markers = markerService.getMarkersPublic(worldName, worldOwner);
            return ResponseEntity.status(HttpStatus.OK).body(markers);
        }catch(Exception ex){
            return ResponseEntity.status(HttpStatus.NOT_FOUND).build();
        }
    }

    @GetMapping("/my-worlds/{worldName}/markers")
    public ResponseEntity<List<Marker>> getMyMarkers(@PathVariable String worldName,  @AuthenticationPrincipal UserDetails user) {
        String username = user.getUsername();
        try{
            List<Marker> markers = markerService.getMarkers(worldName, username);
            return ResponseEntity.status(HttpStatus.OK).body(markers);
        }catch(Exception ex){
            return ResponseEntity.status(HttpStatus.NOT_FOUND).build();
        }
    }

    @PutMapping("/my-worlds/{worldName}/markers/{markerName}")
    public ResponseEntity<Marker> updateMarker(@PathVariable String worldName, @RequestBody Marker marker, @PathVariable String markerName, @AuthenticationPrincipal UserDetails user){
        String username = user.getUsername();
        try{
            Marker updatedMarker =  markerService.updateMarker(marker, worldName, username, markerName);

            return ResponseEntity.status(HttpStatus.OK).body(updatedMarker);
        } catch (RuntimeException e) {
            return  ResponseEntity.status(HttpStatus.NOT_FOUND).build();
        }
    }

    @DeleteMapping("/my-worlds/{worldName}/markers/{markerId}")
    public ResponseEntity<?> deleteMarker(@PathVariable String worldName, @PathVariable String markerId, @AuthenticationPrincipal UserDetails user) {
        String username = user.getUsername();
        try{
            markerService.deleteMarker(markerId, worldName, username);
            return ResponseEntity.noContent().build();
        }catch(RuntimeException ex){
            return ResponseEntity.status(HttpStatus.NOT_FOUND).build();
        }

    }

    @PostMapping("/my-worlds/{worldName}/markers/{markerName}/image")
    public ResponseEntity<?> uploadMarkerImage(
            @PathVariable String worldName,
            @PathVariable String markerName,
            @RequestParam("file") MultipartFile file,
            @AuthenticationPrincipal UserDetails userDetails) {

        String username = userDetails.getUsername();
        try {
            // Validar que es una imagen
            if (!file.getContentType().startsWith("image/")) {
                return ResponseEntity.badRequest().body("El archivo debe ser una imagen.");
            }

            //Guardamos el archivo en el disco
            String filePath = fileStorageService.saveMarkerImage(file, username, worldName, markerName);

            // Guardamos la ruta en la base de datos (en la entidad Marker)
            markerService.updateMarkerImagePath(username, worldName, markerName, filePath);

            // Devolvemos la nueva ruta
            return ResponseEntity.ok(Map.of("filePath", filePath));

        } catch (Exception e) {
            return ResponseEntity.status(500).body("Error al subir el archivo: " + e.getMessage());
        }
    }
}
