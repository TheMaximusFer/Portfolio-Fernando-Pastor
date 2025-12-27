package sisInf.T09.Enderarchive.controller;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.security.core.annotation.AuthenticationPrincipal;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.web.bind.annotation.*;
import sisInf.T09.Enderarchive.model.Favourite;
import sisInf.T09.Enderarchive.model.World;
import sisInf.T09.Enderarchive.model.WorldId;
import sisInf.T09.Enderarchive.service.FavouriteService;
import sisInf.T09.Enderarchive.service.WorldService;

import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

@RestController
@RequestMapping("/api/favourites")
public class FavouriteController {
    FavouriteService favouriteService;
    WorldService worldService;

    @Autowired
    public FavouriteController(FavouriteService favouriteService,  WorldService worldService) {
        this.favouriteService=favouriteService;
        this.worldService=worldService;
    }

    /**
     * Da favorito a un mundo.
     * El usuario se obtiene del token. El mundo se obtiene de la URL.
     */
    @PostMapping("/{worldOwner}/{worldName}/favourite")
    public ResponseEntity<Favourite> addFavourite(
            @PathVariable String worldOwner,
            @PathVariable String worldName,
            @AuthenticationPrincipal UserDetails user) {

        String username = user.getUsername();
        try {
            Favourite newFavourite = favouriteService.createFavourite(username, worldName, worldOwner);
            return ResponseEntity.status(HttpStatus.CREATED).body(newFavourite); // 201 Created
        } catch (RuntimeException e) {
            // Podría ser 404 (mundo no existe) o 409 (ya es favorito)
            return ResponseEntity.status(HttpStatus.CONFLICT).build();
        }
    }

    @GetMapping("/info")
    public ResponseEntity<List<Favourite>> getFavourites(@AuthenticationPrincipal UserDetails user){
        String username = user.getUsername();
        return ResponseEntity.ok(favouriteService.getFavourites(username));
    }

    @GetMapping("/no-filters")
    public ResponseEntity<List<World>> getFavouritesNoFilters(@AuthenticationPrincipal UserDetails user){
        String username = user.getUsername();
        List<Favourite> mundosFavoritos = favouriteService.getFavourites(username);
        List<World> mundos = new ArrayList<World>();
        for (Favourite favourite : mundosFavoritos) {
            try{
                mundos.add(worldService.getWorldByOwner(favourite.getId().getWorldId().getWorldOwner(),
                        favourite.getId().getWorldId().getWorldName())
                );
            }catch(Exception e){
                System.out.println("Mundo favorito no encontrado, saltando...");
            }

        }
        return ResponseEntity.ok(mundos);
    }

    @GetMapping
    public ResponseEntity<List<World>> getFavouriteWorlds(
            @AuthenticationPrincipal UserDetails user,
            @RequestParam(value = "adventure", required = false) Boolean adventureFilter,
            @RequestParam(value = "survival", required = false) Boolean survivalFilter,
            @RequestParam(value = "skyblock", required = false) Boolean skyblockFilter,
            @RequestParam(value = "fantasy", required = false) Boolean fantasyFilter,
            @RequestParam(value = "version", required = false) String minecraftVersion,
            @RequestParam(value = "java", required = false) Boolean javaFilter,
            @RequestParam(value = "search", required = false) String worldSearched
    ){
        String username = user.getUsername();
        List<Favourite> mundosFavoritos = favouriteService.searchFavourites(username, worldSearched);

        List<World> mundos = new ArrayList<World>();
        for (Favourite favourite : mundosFavoritos) {
            try{
                mundos.add(worldService.findWorldByCriteria(
                                favourite.getId().getWorldId().getWorldOwner(),
                                adventureFilter,
                                survivalFilter,
                                skyblockFilter,
                                fantasyFilter,
                                minecraftVersion,
                                javaFilter,
                                favourite.getId().getWorldId().getWorldName(),
                                username.equals(favourite.getId().getWorldId().getWorldOwner())
                    )
                );

            }catch(Exception e){
                System.out.println("Mundo favorito no encontrado, saltando...");
            }

        }
        return ResponseEntity.ok(mundos);

    }

    /**
     * Quita el favorito de un mundo.
     */
    @DeleteMapping("/{worldOwner}/{worldName}/favourite")
    public ResponseEntity<Void> deleteFavourite(
            @PathVariable String worldOwner,
            @PathVariable String worldName,
            @AuthenticationPrincipal UserDetails user) {

        String username = user.getUsername();
        try {
            favouriteService.deleteFavourite(username, worldName, worldOwner);
            return ResponseEntity.noContent().build(); // 204 No Content
        } catch (RuntimeException e) {
            return ResponseEntity.notFound().build();
        }
    }
}
