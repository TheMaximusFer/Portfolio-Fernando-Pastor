package sisInf.T09.Enderarchive.service;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import sisInf.T09.Enderarchive.model.*;
import sisInf.T09.Enderarchive.repository.FavouriteRepository;
import sisInf.T09.Enderarchive.repository.UserRepository;
import sisInf.T09.Enderarchive.repository.WorldRepository;

import java.util.List;

@Service
public class FavouriteService {
    FavouriteRepository favouriteRepository;
    WorldRepository worldRepository;
    UserRepository userRepository;


    @Autowired
    public FavouriteService(FavouriteRepository favouriteRepository,
                            WorldRepository worldRepository,
                            UserRepository userRepository) {
        this.favouriteRepository = favouriteRepository;
        this.worldRepository = worldRepository;
        this.userRepository = userRepository;
    }


    public Favourite createFavourite(String username, String worldName, String worldOwner) {
        //Comprobamos que existe el usuario dueño del mundo
        User owner = userRepository.findById(worldOwner)
                .orElseThrow(() -> new RuntimeException("Usuario dueño del mundo no encontrado: " + username));

        //Construimos un worldId para comprobar que existe el mundo que queremos poner como favorito
        WorldId completeId = new WorldId(worldName, worldOwner);
        //Hacemos la comprobación en la base de datos
        World world = worldRepository.findById(completeId).
                orElseThrow(() -> new RuntimeException("Mundo no existe: " + worldName));
        //Si existen tanto el dueño del mundo como el mundo, añadimos este al favorito
        Favourite favourite = new Favourite();
        favourite.setWorld(world);
        //Comprobamos si el dueño del favorito existe y si es así se lo asignamos al favorito
        favourite.setUser(userRepository.findById(username).
                orElseThrow(() -> new RuntimeException("Usuario dueño de favorito no encontrado: " + username)));
        //Rellenamos el Id del favorito una vez que hemos comprobado que existen todos sus campos
        favourite.setId(new FavouriteId(username, completeId));
        //Comprobamos que el favorito no existe ya
        if(favouriteRepository.existsById(favourite.getId())) {
            throw new RuntimeException("Favourite ya existe: " + favourite.getId());
        }
        //Guardamos el favorito en la base de datos
        return favouriteRepository.save(favourite);

    }

    public List<Favourite> getFavourites(String username) {
        return favouriteRepository.findByIdUsername(username);
    }

    public void deleteFavourite(String username, String worldName, String worldOwner) {
        WorldId worldId = new WorldId(worldName, worldOwner);
        FavouriteId favId = new FavouriteId(username, worldId);
        if(favouriteRepository.existsById(favId)) {
            favouriteRepository.deleteById(favId);
        }else{
            throw new RuntimeException("Favourite no encontrado: " + username);
        }
    }

    public List<Favourite> searchFavourites(String username, String search) {
        if (search == null || search.isEmpty()) {
            // Si no escriben nada, devolvemos todos los favoritos
            return favouriteRepository.findByIdUsername(username);
        }
        return favouriteRepository.searchFavouritesByWorldName(username, search);
    }
}
