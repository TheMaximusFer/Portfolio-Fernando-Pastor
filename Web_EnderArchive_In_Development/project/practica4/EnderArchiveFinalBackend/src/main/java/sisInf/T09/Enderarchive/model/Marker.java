package sisInf.T09.Enderarchive.model;

import com.fasterxml.jackson.annotation.JsonIgnore;
import jakarta.persistence.*;
import lombok.Getter;
import lombok.Setter;

@Entity 
@Table(name="marker")
@Getter 
@Setter
public class Marker {
    @EmbeddedId
    private MarkerId id;

    @MapsId("worldId")
    @ManyToOne(fetch = FetchType.LAZY)
    @JsonIgnore
    private World world;

    private int x;
    private int y;
    private int z;
    private String dimension;
    private String imagePath;
}
