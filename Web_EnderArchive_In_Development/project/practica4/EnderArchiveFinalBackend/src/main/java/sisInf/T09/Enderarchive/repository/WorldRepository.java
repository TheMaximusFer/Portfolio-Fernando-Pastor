package sisInf.T09.Enderarchive.repository;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.JpaSpecificationExecutor;
import sisInf.T09.Enderarchive.model.World;
import sisInf.T09.Enderarchive.model.WorldId;

import java.util.List;
import java.util.Optional;

public interface WorldRepository extends JpaRepository<World, WorldId>, JpaSpecificationExecutor<World> {
    List<World> findByOwner_Username(String username);
    List<World> findByIsPublicTrue();
    List<World> findByIdWorldName(String worldName);
    Boolean existsByDisplayName(String displayName);
    Boolean existsByOwner_UsernameAndDisplayName(String username, String displayName);
    Boolean existsByIdAndIsPublic(WorldId worldId, boolean isPublic);
    List<World> findByOwner_UsernameAndIsPublic(String username, Boolean isPublic);

    Optional<World> findByIdAndIsPublic(WorldId worldId, boolean isPublic);
}
