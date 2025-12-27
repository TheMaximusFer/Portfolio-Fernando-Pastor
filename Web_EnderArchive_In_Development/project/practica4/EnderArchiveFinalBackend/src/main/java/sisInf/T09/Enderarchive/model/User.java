package sisInf.T09.Enderarchive.model;

import com.fasterxml.jackson.annotation.JsonIgnore;
import jakarta.persistence.*;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;
import org.springframework.security.core.GrantedAuthority;
import org.springframework.security.core.authority.SimpleGrantedAuthority;
import org.springframework.security.core.userdetails.UserDetails;

import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;

@Entity
@Data
@NoArgsConstructor
@AllArgsConstructor
@Table(name = "\"user\"", schema = "\"sisinf\"")
public class User implements UserDetails {

    @Id // Marcamos la clave primaria
    @Column(name = "username") // Mapea al nombre exacto de la columna
    private String username;

    @Column(name = "password")
    private String password;

    @Column(name = "description")
    private String description;

    @Column(name = "admin")
    private Boolean admin;

    @Column(name = "sanctioned")
    private Boolean sanctioned;

    @Column(name = "banned")
    private Boolean banned;

    @Column(name = "end_of_sanction")
    private LocalDateTime endOfSanction;

    @Column(name = "sanction_reason")
    private String sanctionReason;

    @Column(name = "profile_picture_file_path")
    private String profilePic;

    @Column(name = "email")
    private String email;

    // --- RELACIONES PARA ELIMINACIÓN EN CASCADA ---

    // 1. Mundos del usuario (Si borras usuario, se borran sus mundos)
    @OneToMany(mappedBy = "owner", cascade = CascadeType.ALL, orphanRemoval = true)
    @JsonIgnore
    private List<World> worlds = new ArrayList<>();

    // 2. Favoritos del usuario (Si borras usuario, se borran sus favoritos)
    @OneToMany(mappedBy = "user", cascade = CascadeType.ALL, orphanRemoval = true)
    @JsonIgnore
    private List<Favourite> favourites = new ArrayList<>();

    // 3. Reportes hechos POR el usuario (Si borras usuario, se borran sus denuncias)
    @OneToMany(mappedBy = "reporter", cascade = CascadeType.ALL, orphanRemoval = true)
    @JsonIgnore
    private List<Report> reportsMade = new ArrayList<>();

    // 4. Reportes CONTRA el usuario (Si borras usuario, se borran los reportes en su contra)
    @OneToMany(mappedBy = "reported", cascade = CascadeType.ALL, orphanRemoval = true)
    @JsonIgnore
    private List<Report> reportsReceived = new ArrayList<>();


    @Override
    public Collection<? extends GrantedAuthority> getAuthorities() {
        if(this.admin){
            return Arrays.asList(new SimpleGrantedAuthority("ROLE_ADMIN"));
        }else{
            return Arrays.asList(new SimpleGrantedAuthority("ROLE_USER"));
        }
    }

    @Override
    public boolean isAccountNonExpired() {
        return true;
    }

    @Override
    public boolean isAccountNonLocked() {
        return true;
    }

    @Override
    public boolean isCredentialsNonExpired() {
        return true;
    }

    @Override
    public boolean isEnabled() {
        return true;
    }


}
