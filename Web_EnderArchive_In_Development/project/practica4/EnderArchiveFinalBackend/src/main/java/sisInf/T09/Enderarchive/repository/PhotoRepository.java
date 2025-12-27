package sisInf.T09.Enderarchive.repository;

import org.springframework.data.jpa.repository.JpaRepository;
import sisInf.T09.Enderarchive.model.Photo;
import sisInf.T09.Enderarchive.model.PhotoId;
import sisInf.T09.Enderarchive.model.WorldId;

import java.util.List;

public interface PhotoRepository extends JpaRepository<Photo, PhotoId> {
    List<Photo> findByIdWorldId(WorldId worldId);
}
