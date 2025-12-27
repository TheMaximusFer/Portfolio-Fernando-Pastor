package sisInf.T09.Enderarchive.service;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import sisInf.T09.Enderarchive.model.Marker;
import sisInf.T09.Enderarchive.model.MarkerId;
import sisInf.T09.Enderarchive.model.World;
import sisInf.T09.Enderarchive.model.WorldId;
import sisInf.T09.Enderarchive.repository.MarkerRepository;
import sisInf.T09.Enderarchive.repository.WorldRepository;

import java.util.List;

@Service
public class MarkerService {
    MarkerRepository markerRepository;
    WorldRepository worldRepository;

    @Autowired
    public MarkerService(MarkerRepository markerRepository, WorldRepository worldRepository) {
        this.markerRepository = markerRepository;
        this.worldRepository = worldRepository;
    }

    public Marker createMarker(String worldName , Marker marker, String username) {
        //Crea un worldId para comprobar que el mundo que nos han pasado existe y le pertenece al username que nos han pasado
        WorldId worldId = new WorldId(worldName, username);
        //Comprueba que está dentro de la base de datos y su dueño
        World world = worldRepository.findById(worldId).//Si el mundo no existe o no pertenece al usuario lanzamos excepción
                orElseThrow(() -> new RuntimeException("El mundo no existe o no te pertenece"));
        //Si está correcto creamos un id para el marker que os han pasado y le metemos el id y el world que hemos encontrado
        String markerNameFromJSON = marker.getId().getIdMarker();
        MarkerId markerId = new MarkerId(markerNameFromJSON, worldId);
        marker.setId(markerId);
        marker.setWorld(world);
        //Guardamos el marker en la base de datos
        return markerRepository.save(marker);
    }

    public List<Marker> getMarkers(String worldName, String username) {
        WorldId worldId = new WorldId(worldName, username);
        if(!worldRepository.existsById(worldId)) {
            throw new RuntimeException("El mundo no existe");
        }
        return markerRepository.findByIdWorldId(worldId);
    }

    public List<Marker> getMarkersPublic(String worldName, String username) {
        WorldId worldId = new WorldId(worldName, username);
        if(!worldRepository.existsById(worldId)) {
            throw new RuntimeException("El mundo no existe");
        }
        if(!worldRepository.existsByIdAndIsPublic(worldId, true)){
            throw new RuntimeException("El mundo no existe");
        }
        return markerRepository.findByIdWorldId(worldId);
    }

    public Marker updateMarker(Marker newMarker, String worldName, String username, String markerName) {
        WorldId worldId = new WorldId(worldName, username);
        if(!worldRepository.existsById(worldId)) {
            throw new RuntimeException("El mundo no existe o no te pertenece");
        }

        Marker oldMarker = markerRepository.findById(new MarkerId(markerName, worldId)).
                orElseThrow(() -> new RuntimeException("El marcador del mundo no existe"));

        String newMarkerName =  newMarker.getId().getIdMarker();

        if(!markerName.equals(newMarkerName)) {
            if(markerRepository.existsById(new MarkerId(newMarkerName, worldId))){
                throw new RuntimeException("Este nombre de marcador ya existe");
            }else{
                newMarker.setId(new MarkerId(newMarkerName, worldId));
                newMarker.setWorld(oldMarker.getWorld());
                markerRepository.delete(oldMarker);
                return markerRepository.save(newMarker);
            }
        }

        oldMarker.setDimension(newMarker.getDimension());
        oldMarker.setX(newMarker.getX());
        oldMarker.setY(newMarker.getY());
        oldMarker.setZ(newMarker.getZ());

        return markerRepository.save(oldMarker);
    }


     //Actualiza la ruta de la imagen para un marcador específico.
    public Marker updateMarkerImagePath(String username, String worldName, String markerName, String imagePath) {
        WorldId worldId = new WorldId(worldName, username);
        MarkerId markerId = new MarkerId(markerName, worldId);

        // Encontrar el marcador
        Marker marker = markerRepository.findById(markerId)
                .orElseThrow(() -> new RuntimeException("El marcador del mundo no existe"));

        // Actualizar el nuevo campo
        marker.setImagePath(imagePath);

        // Guardar y devolver
        return markerRepository.save(marker);
    }

    public void deleteMarker(String markerName, String worldName, String username) {
        WorldId worldId = new WorldId(worldName, username);
        if(!worldRepository.existsById(worldId)) {
            throw new RuntimeException("El mundo no existe");
        }
        Marker markerToDelete = markerRepository.findById(new MarkerId(markerName, worldId)).
                orElseThrow(() -> new RuntimeException("El marcador del mundo no existe"));

        markerRepository.delete(markerToDelete);
    }
}
