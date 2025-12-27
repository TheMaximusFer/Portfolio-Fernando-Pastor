package sisInf.T09.Enderarchive.service;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.web.multipart.MultipartFile;
import sisInf.T09.Enderarchive.model.Photo;
import sisInf.T09.Enderarchive.model.PhotoId;
import sisInf.T09.Enderarchive.model.World;
import sisInf.T09.Enderarchive.model.WorldId;
import sisInf.T09.Enderarchive.repository.PhotoRepository;
import sisInf.T09.Enderarchive.repository.WorldRepository;

import java.io.IOException;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.List;

@Service
public class PhotoService {
    private FileStorageService fileStorageService;
    private WorldService  worldService;
    private PhotoRepository photoRepository;
    private WorldRepository worldRepository;


    @Autowired
    public PhotoService(PhotoRepository photoRepository,  WorldRepository worldRepository, WorldService worldService, FileStorageService fileStorageService) {
        this.photoRepository = photoRepository;
        this.worldRepository = worldRepository;
        this.worldService = worldService;
        this.fileStorageService = fileStorageService;
    }

    public List<Photo> getPhotosPublic(String username, String worldName){
        WorldId worldId = new WorldId(worldName, username);
        if(!worldRepository.existsById(worldId)) {
            throw new RuntimeException("El mundo no existe");
        }
        if(!worldRepository.existsByIdAndIsPublic(worldId, true)) {
            throw new RuntimeException("El mundo no existe");
        }
        return photoRepository.findByIdWorldId(worldId);
    }

    public List<Photo> getPhotos(String username, String worldName){
        WorldId worldId = new WorldId(worldName, username);
        if(!worldRepository.existsById(worldId)) {
            throw new RuntimeException("El mundo no existe");
        }
        return photoRepository.findByIdWorldId(worldId);
    }

    public Photo createPhoto(Photo photo, String worldName, String username){
        WorldId worldId = new WorldId(worldName, username);
        if(!worldRepository.existsById(worldId)) {
            throw new RuntimeException("El mundo no existe");
        }
        World world = worldRepository.findById(worldId).get();
        PhotoId nuevoPhotoId = new PhotoId(photo.getId().getFilePath(), worldId);
        Integer i = 1;
        while(photoRepository.existsById(nuevoPhotoId)) {
            nuevoPhotoId.setFilePath(nuevoPhotoId.getFilePath() + "(" + i + ")");
            i++;
        }
        photo.setId(nuevoPhotoId);
        photo.setWorld(world);
        return photoRepository.save(photo);
    }

    public void deletePhoto(Photo photo, String worldName, String username){
        WorldId worldId = new WorldId(worldName, username);
        if(!worldRepository.existsById(worldId)) {
            throw new RuntimeException("El mundo no existe");
        }
        photo.setId(new PhotoId(photo.getId().getFilePath(), worldId));
        photoRepository.delete(photo);
    }

    public Photo uploadPhoto(String username, String worldName, MultipartFile file) {

        // Buscar el mundo (y verificar que pertenece al usuario 'username')
        // worldService.getWorldByOwner ya debería lanzar excepción si no existe/no es suyo
        World world = worldService.getWorldByOwner(username, worldName);

        try {
            //guardamos el archivo físico usando el fileStorageService
            String photoPath = fileStorageService.saveWorldPhoto(file, username, worldName);

            // creamos y guardamos la entidad en la Base de Datos
            Photo photo = new Photo();
            PhotoId id_photo = new PhotoId(photoPath, world.getId());
            photo.setId(id_photo);
            photo.setWorld(world);

            return photoRepository.save(photo);

        } catch (IOException e) {
            throw new RuntimeException("Error al guardar la foto: " + e.getMessage());
        }
    }

}
