package sisInf.T09.Enderarchive.model;

import com.fasterxml.jackson.annotation.JsonIgnore;
import jakarta.persistence.*;
import lombok.Getter;
import lombok.Setter;

@Entity
@Table(name="favourites")
@Getter
@Setter
public class Favourite {
    @EmbeddedId
    private FavouriteId id;

    @MapsId("username")
    @ManyToOne(fetch = FetchType.LAZY)
    @JsonIgnore
    private User user;

    @MapsId("worldId")
    @ManyToOne(fetch = FetchType.LAZY)
    @JsonIgnore
    private World world;
}
