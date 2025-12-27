package sisInf.T09.Enderarchive.service;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.core.GrantedAuthority;
import org.springframework.security.core.authority.SimpleGrantedAuthority;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.security.core.userdetails.UserDetailsService;
import org.springframework.security.core.userdetails.UsernameNotFoundException;
import org.springframework.security.crypto.password.PasswordEncoder;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;
import sisInf.T09.Enderarchive.dto.RegisterRequest;
import sisInf.T09.Enderarchive.model.User;
import sisInf.T09.Enderarchive.repository.UserRepository;

import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.List;

@Service
public class UserService implements UserDetailsService {

    // Necesitamos una referencia a nuestro Almacén (Repository)
    private final UserRepository userRepository;
    private final PasswordEncoder passwordEncoder;

    @Override
    @Transactional
    public UserDetails loadUserByUsername(String username) throws UsernameNotFoundException {

        // Busca el usuario en tu base de datos
        User user = userRepository.findById(username)
                .orElseThrow(() -> new UsernameNotFoundException("Usuario no encontrado con el nombre: " + username));

        if (user.getSanctioned() && user.getEndOfSanction() != null) {

            // Obtenemos la hora actual del servidor
            LocalDateTime now = LocalDateTime.now();

            // Obtenemos la hora de fin de sanción
            LocalDateTime sanctionEnd = user.getEndOfSanction();

            // Comparamos las dos fechas
            if (sanctionEnd.isBefore(now)) {
                // Si la sanción ha expirado
                // Limpiamos al usuario
                user.setSanctioned(false);
                user.setEndOfSanction(null);
                user.setSanctionReason(null);

                userRepository.save(user); // Guardamos el cambio
            }
        }


        // Convertimos 'boolean admin' en un 'Rol' de Spring
        List<GrantedAuthority> authorities = new ArrayList<>();

        // Asignamos el rol base que todos tienen
        authorities.add(new SimpleGrantedAuthority("ROLE_USER"));

        // Si el flag 'admin' en la DB es 'true'...
        if (user.getAdmin()) {
            // ...le añadimos el rol de Administrador.
            authorities.add(new SimpleGrantedAuthority("ROLE_ADMIN"));
        }

        boolean accountNonLocked = !user.getBanned() && !user.getSanctioned();


        // Devolvemos el objeto UserDetails
        // Usamos el constructor completo de Spring para incluir
        // el estado de la cuenta (baneado, etc.)
        return new org.springframework.security.core.userdetails.User(
                user.getUsername(),
                user.getPassword(),
                true, // enabled
                true, // accountNonExpired
                true, // credentialsNonExpired
                accountNonLocked, // accountNonLocked (usamos la lógica de baneo)
                authorities // La lista de roles
        );
    }

    // Inyectamos esa dependencia en el constructor
    @Autowired
    public UserService(UserRepository userRepository,  PasswordEncoder passwordEncoder) {
        this.userRepository = userRepository;
        this.passwordEncoder = passwordEncoder;
    }

    // --- AQUÍ CREAREMOS LOS MÉTODOS DEL CRUD ---

    // CREATE (Crear)
    public User createUser(User user) {
        // Simplemente le pasamos el usuario al repositorio para que lo guarde
        // Comprobando si el usuario ya existe

        if(!userRepository.existsById(user.getUsername())){
            user.setBanned(false);
            user.setSanctioned(false);
            user.setPassword(passwordEncoder.encode(user.getPassword()));
            return userRepository.save(user);
        }else{
            return null;
        }
    }

    public User registerUser(RegisterRequest registerRequest) {


        if (userRepository.existsById(registerRequest.getUsername())) {
            // Lanzamos una excepción que el controlador puede capturar
            throw new RuntimeException("Error: El nombre de usuario ya está en uso.");
        }

        User newUser = new User();

        newUser.setUsername(registerRequest.getUsername());
        newUser.setEmail(registerRequest.getEmail());
        newUser.setDescription(registerRequest.getDescription());

        // Hasheamos la contraseña
        newUser.setPassword(passwordEncoder.encode(registerRequest.getPassword()));

        // Ignoramos cualquier valor 'admin' que venga del JSON.
        // Nosotros, en el backend, forzamos su valor.
        newUser.setAdmin(false);
        newUser.setBanned(false);
        newUser.setSanctioned(false);

        return userRepository.save(newUser);
    }


    // READ (Leer)
    public List<User> getAllUsers() {
        // ¿Qué deberíamos escribir aquí para usar el repository?
        return userRepository.findAll();
    }

    // READ (Leer uno por ID/Username)
    public User getUserByUsername(String username) {
        // findById() devuelve un "Optional" (un objeto que puede o no tener un usuario).
        // .orElse(null) significa "dame el usuario si existe, o devuelve null si no"
        return userRepository.findById(username).
                orElseThrow(() -> new UsernameNotFoundException("Usuario no encontrado"));
    }

    // UPDATE DEBUG (Actualizar)
    public User updateUserDebug(String username, User userDetails) {
        // Buscamos al usuario existente
        User existingUser = userRepository.findById(username).orElse(null);

        // Si no existe, no podemos actualizar
        if (existingUser == null) {
            return null;
        }

        existingUser.setPassword(passwordEncoder.encode(userDetails.getPassword()));
        existingUser.setEmail(userDetails.getEmail());
        existingUser.setDescription(userDetails.getDescription());
        existingUser.setAdmin(userDetails.getAdmin());
        /*existingUser.setBanned(userDetails.getBanned());
        existingUser.setSanctioned(userDetails.getSanctioned());
        existingUser.setSanctionReason(userDetails.getSanctionReason());
        existingUser.setEndOfSanction(userDetails.getEndOfSanction());
        existingUser.setProfilePic(userDetails.getProfilePic());*/

        // Guardamos el usuario actualizado
        return userRepository.save(existingUser);
    }

    public User updateUser(User userDetails) {
        // Buscamos al usuario existente
        User existingUser = userRepository.findById(userDetails.getUsername()).orElse(null);

        // Si no existe, no podemos actualizar
        if (existingUser == null) {
            return null;
        }

        //actualizamos los campos que nos han pasado
        if (userDetails.getPassword() != null) {
            existingUser.setPassword(passwordEncoder.encode(userDetails.getPassword()));
        }
        if (userDetails.getEmail() != null) {
            existingUser.setEmail(userDetails.getEmail());
        }
        if (userDetails.getDescription() != null) {
            existingUser.setDescription(userDetails.getDescription());
        }
        //guardamos al usuario actualizado
        return  userRepository.save(existingUser);
    }

    // DELETE (Borrar)
    public boolean deleteUser(String username) {
        if(!userRepository.existsById(username)){
            return false;
        }else{
            userRepository.deleteById(username);
            return true;
        }
    }

    public User promoteUserToAdmin(String usernameToPromote) {
        //Buscar al usuario
        User user = userRepository.findById(usernameToPromote)
                .orElseThrow(() -> new RuntimeException("Usuario no encontrado"));

        //Promocionarlo
        user.setAdmin(true);

        // Guardar los cambios
        return userRepository.save(user);
    }

    public void banUser(String username) {
        User user = userRepository.findById(username).
                orElseThrow(() -> new RuntimeException("Usuario no encontrado"));
        user.setBanned(true);
        userRepository.save(user);
    }

    public void updateProfilePicturePath(String username, String filePath) {
        User user = userRepository.findById(username).
                orElseThrow(() -> new RuntimeException("Usuario no encontrado"));
        user.setProfilePic(filePath);
        userRepository.save(user);
    }
}
