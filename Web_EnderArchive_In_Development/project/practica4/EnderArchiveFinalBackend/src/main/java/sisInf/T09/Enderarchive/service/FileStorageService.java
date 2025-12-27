package sisInf.T09.Enderarchive.service;

import org.springframework.stereotype.Service;
import org.springframework.web.multipart.MultipartFile;

import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.StandardCopyOption;

@Service
public class FileStorageService {
    private final Path rootUploadsLocation = Paths.get("uploads");

    public FileStorageService() {
        try{
            Files.createDirectories(rootUploadsLocation);
        }catch(IOException e){
            throw new RuntimeException("No se pude inicializar el almacenamiento", e);
        }
    }

    private void saveFile(MultipartFile file, Path destinationFile) throws IOException {
        try (InputStream inputStream = file.getInputStream()) {
            // REPLACE_EXISTING permite sobreescribir (ej: actualizar foto de perfil)
            Files.copy(inputStream, destinationFile, StandardCopyOption.REPLACE_EXISTING);
        }
    }

    public String getFileExtension(String filename){
        if(filename == null || !filename.contains(".")){
            return "";
        }
        return filename.substring(filename.lastIndexOf("."));
    }

    public String saveProfilePicture(MultipartFile file, String username) throws IOException {
        if(file.isEmpty()){
            throw new IOException("El archivo no puede ser vacio");
        }
        Path userProfileDir = rootUploadsLocation.resolve(Paths.get(username, "profilePicture"));
        Files.createDirectories(userProfileDir);

        String extension = getFileExtension(file.getOriginalFilename());
        String filename = "profilePicture" + extension;

        Path destinationFile = userProfileDir.resolve(filename).toAbsolutePath();
        saveFile(file, destinationFile);

        return "/uploads/" + username + "/profilePicture/" + filename;
    }

    public String saveWorldFile(MultipartFile file, String username, String worldName) throws IOException {
        if (file.isEmpty()) {
            throw new IOException("Fallo al guardar un archivo vacío.");
        }

        // Crear la ruta del directorio (ej: uploads/user/worlds/mi-mundo)
        Path worldDir = rootUploadsLocation.resolve(Paths.get(username, "worlds", worldName));
        Files.createDirectories(worldDir);

        // Definir el nombre (ej: "world.zip")
        String extension = getFileExtension(file.getOriginalFilename());
        String filename = "world" + extension;

        // Ruta de destino
        Path destinationFile = worldDir.resolve(filename).toAbsolutePath();

        // Guardar
        saveFile(file, destinationFile);

        // Devolver la ruta web
        return "/uploads/" + username + "/worlds/" + worldName + "/" + filename;
    }

    //uploads/{username}/worlds/{worldName}/miniature.png
    public String saveWorldMiniature(MultipartFile file, String username, String worldName) throws IOException {
        if (file.isEmpty()) {
            throw new IOException("Fallo al guardar un archivo vacío.");
        }
        // Directorio el mismo que el archivo del mundo
        Path worldDir = rootUploadsLocation.resolve(Paths.get(username, "worlds", worldName));
        Files.createDirectories(worldDir);

        // Nombre de archivo
        String extension = getFileExtension(file.getOriginalFilename());
        String filename = "miniature" + extension;

        // Ruta de destino
        Path destinationFile = worldDir.resolve(filename).toAbsolutePath();

        // Guardar
        saveFile(file, destinationFile);

        // Devolver la ruta web
        return "/uploads/" + username + "/worlds/" + worldName + "/" + filename;
    }

    /**
     * Guarda una foto de galería de un mundo.
     * Ruta: uploads/{username}/worlds/{worldName}/photos/archivo.png
     */
    public String saveWorldPhoto(MultipartFile file, String username, String worldName) throws IOException {
        if (file.isEmpty()) {
            throw new IOException("Fallo al guardar un archivo vacío.");
        }

        //crear la ruta: uploads/USUARIO/worlds/MUNDO/photos
        Path photosDir = rootUploadsLocation.resolve(Paths.get(username, "worlds", worldName, "photos"));
        Files.createDirectories(photosDir);

        // Generar nombre único (UUID) para evitar sobrescribir fotos si suben muchas
        String originalFilename = file.getOriginalFilename();
        String extension = getFileExtension(originalFilename);
        String filename = java.util.UUID.randomUUID().toString() + extension;

        // Ruta destino
        Path destinationFile = photosDir.resolve(filename).toAbsolutePath();

        // Guardar
        saveFile(file, destinationFile);

        // Devolver la ruta web relativa
        return "/uploads/" + username + "/worlds/" + worldName + "/photos/" + filename;
    }

    //uploads/{username}/worlds/{worldName}/markers/{markerName}/image.png
    public String saveMarkerImage(MultipartFile file, String username, String worldName, String markerName) throws IOException {
        if (file.isEmpty()) {
            throw new IOException("Fallo al guardar un archivo vacío.");
        }

        // Crear la ruta del directorio
        // (ej: uploads/MANIN/worlds/mi-mundo/markers/castillo)
        Path markerDir = rootUploadsLocation.resolve(Paths.get(username, "worlds", worldName, "markers", markerName));
        Files.createDirectories(markerDir); // Crea toda la estructura

        // Definir el nombre (ej: "image.png")
        String extension = getFileExtension(file.getOriginalFilename());
        String filename = "image" + extension;

        // Ruta de destino completa
        Path destinationFile = markerDir.resolve(filename).toAbsolutePath();

        // Guardar
        saveFile(file, destinationFile);

        // Devolver la ruta web (lo que guardas en la DB)
        return "/uploads/" + username + "/worlds/" + worldName + "/markers/" + markerName + "/" + filename;
    }
}
