package sisInf.T09.Enderarchive.repository;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;
import sisInf.T09.Enderarchive.model.Favourite;
import sisInf.T09.Enderarchive.model.FavouriteId;

import java.util.List;

public interface FavouriteRepository extends JpaRepository<Favourite, FavouriteId> {
    List<Favourite> findByIdUsername(String username);
    @Query("SELECT f FROM Favourite f WHERE f.id.username = :username AND LOWER(f.world.displayName) LIKE LOWER(CONCAT('%', :search, '%'))")
    List<Favourite> searchFavouritesByWorldName(@Param("username") String username, @Param("search") String search);
}
