package sisInf.T09.Enderarchive.model;

import jakarta.persistence.*;
import lombok.Getter;
import lombok.Setter;

import java.io.Serializable;
import java.util.Objects;

@Embeddable
@Getter
@Setter
public class MarkerId implements Serializable {
    @Column(name="id_marker")
    private String idMarker;

    @Embedded
    @AttributeOverrides({
        @AttributeOverride(name="worldName", column=@Column(name="world_name")),
        @AttributeOverride(name="worldOwner", column=@Column(name="world_owner"))
    })
    private WorldId worldId;

    public MarkerId() {}
    public MarkerId(String idMarker, WorldId worldId){ this.idMarker=idMarker; this.worldId=worldId; }


    @Override public boolean equals(Object o){
        if (this == o) return true;
        if (!(o instanceof MarkerId that)) return false;
        return Objects.equals(idMarker, that.idMarker) &&
               Objects.equals(worldId, that.worldId);
    }
    @Override public int hashCode(){ return Objects.hash(idMarker, worldId); }
}
