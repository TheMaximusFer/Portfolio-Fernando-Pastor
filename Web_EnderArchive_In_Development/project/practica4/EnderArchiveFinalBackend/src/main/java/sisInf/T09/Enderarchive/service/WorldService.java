package sisInf.T09.Enderarchive.service;

import jakarta.persistence.criteria.Predicate;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.jpa.domain.Specification;
import org.springframework.stereotype.Service;
import sisInf.T09.Enderarchive.model.User;
import sisInf.T09.Enderarchive.model.World;
import sisInf.T09.Enderarchive.model.WorldId;
import sisInf.T09.Enderarchive.repository.UserRepository;
import sisInf.T09.Enderarchive.repository.WorldRepository;

import java.util.ArrayList;
import java.util.List;

@Service
public class WorldService {

    private final WorldRepository worldRepository;
    private final UserRepository userRepository;

    @Autowired
    public WorldService(WorldRepository worldRepository, UserRepository userRepository) {
        this.worldRepository = worldRepository;
        this.userRepository = userRepository;
    }

    public List<World> getPublicWorlds() {
        return worldRepository.findByIsPublicTrue();
    }

    public List<World> getAllWorlds() {
        return worldRepository.findAll();
    }

    public List<World> getWorldsByOwner(String username) {
        User owner = userRepository.findById(username)
                .orElseThrow(() -> new RuntimeException("Usuario no encontrado: " + username));
        return worldRepository.findByOwner_Username(owner.getUsername());
    }

    public World createWorld(World newWorld, String username) {

        // Buscar al User
        User owner = userRepository.findById(username)
                .orElseThrow(() -> new RuntimeException("Usuario no encontrado: " + username));

        // Extraer el worldName del ID parcial
        String worldName = newWorld.getId().getWorldName();


        // Crear el ID completo y validado
        WorldId completeId = new WorldId(worldName, username);

        // Comprobar si este mundo ya existe (¡Tu código!)
        if (worldRepository.existsById(completeId)) {
            throw new RuntimeException("Ya tienes un mundo con este nombre");
        }

        // Asignamos las claves que acabamos de crear/validar
        newWorld.setOwner(owner);
        newWorld.setId(completeId);
        newWorld.setDisplayName(worldName);

        // Guardar y devolver
        return worldRepository.save(newWorld);
    }

    public List<World> findPublicWorldByCriteria(
            Boolean adventureFilter,
            Boolean survivalFilter,
            Boolean skyblockFilter,
            Boolean fantasyFilter,
            String minecraftVersion,
            Boolean javaFilter,
            String worldSearched
    ){
        Specification<World> spec = buildWorldSpecification(
                adventureFilter, survivalFilter, skyblockFilter, fantasyFilter, minecraftVersion,
                javaFilter, null, true, worldSearched
        );

        // Ejecutar la consulta
        return worldRepository.findAll(spec);
    }

    public List<World> findMyWorldsByCriteria(
            String username,
            Boolean adventureFilter,
            Boolean survivalFilter,
            Boolean skyblockFilter,
            Boolean fantasyFilter,
            String minecraftVersion,
            Boolean javaFilter,
            String worldSearched
    ){
        Specification<World> spec = buildWorldSpecification(
                adventureFilter, survivalFilter, skyblockFilter, fantasyFilter, minecraftVersion,
                javaFilter, username, false, worldSearched
        );
        return worldRepository.findAll(spec);

    }

    public World findWorldByCriteria(
            String username,
            Boolean adventureFilter,
            Boolean survivalFilter,
            Boolean skyblockFilter,
            Boolean fantasyFilter,
            String minecraftVersion,
            Boolean javaFilter,
            String worldName,
            Boolean isMine
    ){
        Specification<World> spec;
        if(isMine){
            spec = buildWorldSpecification(
                    adventureFilter, survivalFilter, skyblockFilter, fantasyFilter, minecraftVersion,
                    javaFilter, username, false, worldName
            );
        }else{
            spec = buildWorldSpecification(
                    adventureFilter, survivalFilter, skyblockFilter, fantasyFilter, minecraftVersion,
                    javaFilter, username, true, worldName
            );
        }

        List<World> miMundo = worldRepository.findAll(spec);
        return miMundo.get(0);
    }

    private Specification<World> buildWorldSpecification(
            Boolean adventureFilter, Boolean survivalFilter, Boolean skyblockFilter, Boolean fantasyFilter,
            String version, Boolean javaFilter,
            String ownerUsername, boolean onlyPublic, String worldNameFilter) {

        return (root, query, cb) -> {
            List<Predicate> globalPredicates = new ArrayList<>();

            if (worldNameFilter != null && !worldNameFilter.isEmpty()) {
                //usamos 'like' + 'lower' para buscar coincidencias parciales ignorando mayúsculas
                // El "%" significa "cualquier texto antes o después"
                globalPredicates.add(
                        cb.like(
                                cb.lower(root.get("displayName")), // Convertimos el nombre en la BD a minúsculas
                                "%" + worldNameFilter.toLowerCase() + "%" // Convertimos la búsqueda a minúsculas y añadimos comodines
                        )
                );
            }

            if (onlyPublic) {
                // Solo mundos públicos
                globalPredicates.add(cb.isTrue(root.get("isPublic")));
                //de un usuario en específico
                if(ownerUsername != null && !ownerUsername.isEmpty()) {
                    globalPredicates.add(cb.equal(root.get("owner").get("username"), ownerUsername));
                }
            } else if (ownerUsername != null) {
                // Solo mis mundos
                globalPredicates.add(cb.equal(root.get("owner").get("username"), ownerUsername));
            }

            // Filtros Comunes
            if (version != null) {
                globalPredicates.add(cb.equal(root.get("version"), version));
            }
            // javaFilter=true es "Java" y false es "Bedrock"
            String platform = (javaFilter != null && javaFilter) ? "Java" : "Bedrock";
            globalPredicates.add(cb.equal(root.get("platform"), platform));

            // Filtros de Categoría
            List<Predicate> categoryPredicates = new ArrayList<>();

            if (Boolean.TRUE.equals(adventureFilter)) categoryPredicates.add(cb.isTrue(root.get("adventure")));
            if (Boolean.TRUE.equals(survivalFilter)) categoryPredicates.add(cb.isTrue(root.get("survival")));
            if (Boolean.TRUE.equals(skyblockFilter)) categoryPredicates.add(cb.isTrue(root.get("skyblock")));
            if (Boolean.TRUE.equals(fantasyFilter)) categoryPredicates.add(cb.isTrue(root.get("fantasy")));

            if (!categoryPredicates.isEmpty()) {
                globalPredicates.add(cb.or(categoryPredicates.toArray(new Predicate[0])));
            }

            return cb.and(globalPredicates.toArray(new Predicate[0]));
        };
    }

    public void deleteWorld(String worldName, String authenticatedUsername) {

        // Creamos el ID completo del mundo que se intenta borrar
        WorldId idToDelete = new WorldId(worldName, authenticatedUsername);

        // Comprobamos si un mundo con ese ID (nombre + dueño) existe
        if (!worldRepository.existsById(idToDelete)) {
            // Si no existe, o el nombre está mal o el usuario no es el dueño.
            // Por seguridad, solo decimos "No encontrado".
            throw new RuntimeException("Mundo no encontrado o no te pertenece.");
        }

        // Si existe (lo que confirma la propiedad), lo borramos.
        worldRepository.deleteById(idToDelete);
    }

    public World updateWorld(String authenticatedUsername, World updatedData) {

        String worldName = updatedData.getId().getWorldName();

        // Creamos el ID del mundo que queremos actualizar
        WorldId idToUpdate = new WorldId(worldName, authenticatedUsername);

        // Buscamos el mundo en la BBDD.
        // Esto comprueba automáticamente que existe Y que el usuario es el dueño.
        World existingWorld = worldRepository.findById(idToUpdate)
                .orElseThrow(() -> new RuntimeException("Mundo no encontrado o no te pertenece."));

        String newDisplayName = updatedData.getDisplayName();
        String oldDisplayName = existingWorld.getDisplayName();

        if(!oldDisplayName.equals(newDisplayName) && newDisplayName != null) {
            if (worldRepository.existsByOwner_UsernameAndDisplayName(authenticatedUsername, newDisplayName)) {
                throw new RuntimeException("Ya tienes un mundo con este nombre");
            }
        }
        // Actualizamos los campos del mundo existente con los datos nuevos.
        // NUNCA actualizamos el ID o el Dueño. Pero si que actualizamos el display name para poder cambiar el nombre
        existingWorld.setDisplayName(updatedData.getDisplayName());
        existingWorld.setDescription(updatedData.getDescription());
        existingWorld.setPublic(updatedData.isPublic());
        existingWorld.setMods(updatedData.isMods());
        existingWorld.setPlatform(updatedData.getPlatform());
        existingWorld.setVersion(updatedData.getVersion());
        existingWorld.setFilePath(updatedData.getFilePath());
        existingWorld.setMiniatureFilePath(updatedData.getMiniatureFilePath());



        // Guardamos el mundo actualizado en la BBDD
        return worldRepository.save(existingWorld);
    }

    public List<World> getPublicWorldsByOwner(String username) {
        if(!userRepository.existsById(username)){
            throw new RuntimeException("Usuario no encontrado: " + username);
        }
        return worldRepository.findByOwner_UsernameAndIsPublic(username, true);
    }

    public World getPublicWorldByOwner(String username, String worldName) {
        if(!userRepository.existsById(username)){
            throw new RuntimeException("Usuario no encontrado: " + username);
        }
        WorldId worldId = new WorldId(worldName, username);
        return worldRepository.findByIdAndIsPublic(worldId, true).
                orElseThrow(() -> new RuntimeException("Mundo no encontrado"));
    }

    public World getWorldByOwner(String username, String worldName) {
        if(!userRepository.existsById(username)){
            throw new RuntimeException("Usuario no encontrado: " + username);
        }
        WorldId worldId = new WorldId(worldName, username);
        return worldRepository.findById(worldId).
                orElseThrow(() -> new RuntimeException("Mundo no encontrado"));
    }

    public void updateWorldFilePath(String username, String worldName, String filePath) {
        World world = worldRepository.findById(new WorldId(worldName, username)).
                orElseThrow(() -> new RuntimeException("Usuario no encontrado"));
        world.setFilePath(filePath);
        worldRepository.save(world);
    }

    public void updateWorldMiniaturePath(String username, String worldName, String filePath) {
        World world = worldRepository.findById(new WorldId(worldName, username)).
                orElseThrow(() -> new RuntimeException("Usuario no encontrado"));
        world.setMiniatureFilePath(filePath);
        worldRepository.save(world);
    }


}