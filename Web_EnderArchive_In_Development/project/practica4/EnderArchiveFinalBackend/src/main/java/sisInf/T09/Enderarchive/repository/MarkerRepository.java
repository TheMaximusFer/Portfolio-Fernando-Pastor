package sisInf.T09.Enderarchive.repository;


import org.springframework.data.jpa.repository.JpaRepository;
import sisInf.T09.Enderarchive.model.Marker;
import sisInf.T09.Enderarchive.model.MarkerId;
import sisInf.T09.Enderarchive.model.World;
import sisInf.T09.Enderarchive.model.WorldId;

import java.util.List;

public interface MarkerRepository extends JpaRepository<Marker, MarkerId> {
    List<Marker> findByIdWorldId(WorldId worldId);

    Long world(World world);
}
