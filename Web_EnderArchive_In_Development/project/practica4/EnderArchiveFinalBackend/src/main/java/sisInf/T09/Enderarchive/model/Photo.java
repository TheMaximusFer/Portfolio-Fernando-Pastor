package sisInf.T09.Enderarchive.model;

import com.fasterxml.jackson.annotation.JsonIgnore;
import jakarta.persistence.*;
import lombok.Getter;
import lombok.Setter;

@Entity
@Table(name="photo")
@Getter
@Setter
public class Photo {
    @EmbeddedId
    private PhotoId id;

    @MapsId("worldId")
    @ManyToOne(fetch = FetchType.LAZY)
    @JsonIgnore
    private World world;
}
