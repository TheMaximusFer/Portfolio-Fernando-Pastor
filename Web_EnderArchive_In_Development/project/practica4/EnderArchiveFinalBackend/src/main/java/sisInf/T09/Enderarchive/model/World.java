package sisInf.T09.Enderarchive.model;

import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import jakarta.persistence.*;
import lombok.Getter;
import lombok.Setter;

import java.util.ArrayList;
import java.util.List;

@Entity 
@Table(name="world")
@Getter 
@Setter
public class World {
    @EmbeddedId
    private WorldId id;

    @MapsId("worldOwner")
    @ManyToOne(fetch = FetchType.LAZY)
    @JoinColumn(name="world_owner", referencedColumnName="username")
    @JsonIgnore
    private User owner;

    @Column(name="public")
    private boolean isPublic;

    private String description;
    private boolean mods;
    private String platform;
    private String version;

    @Column(name="display_name")
    private String displayName;

    @Column(name = "file_path")
    private String filePath;

    @Column(name = "miniature_file_path")
    private String miniatureFilePath;

    private Boolean fantasy;
    private Boolean survival;
    private Boolean skyblock;
    private boolean adventure;

    // Relación con Markers
    // mappedBy = "world": Indica que el campo 'world' en la clase Marker es el dueño de la relación.
    // cascade = CascadeType.ALL: Si borras el mundo, borra los marcadores.
    // orphanRemoval = true: Si quitas un marcador de esta lista, se borra de la BD.
    @OneToMany(mappedBy = "world", cascade = CascadeType.ALL, orphanRemoval = true)
    @JsonIgnore // Importante: evita que al pedir un mundo se descarguen todos sus marcadores
    private List<Marker> markers = new ArrayList<>();

    // Relación con Photos
    @OneToMany(mappedBy = "world", cascade = CascadeType.ALL, orphanRemoval = true)
    @JsonIgnore
    private List<Photo> photos = new ArrayList<>();

    // Relación con Favourites
    @OneToMany(mappedBy = "world", cascade = CascadeType.ALL, orphanRemoval = true)
    @JsonIgnore
    private List<Favourite> favourites = new ArrayList<>();


}
