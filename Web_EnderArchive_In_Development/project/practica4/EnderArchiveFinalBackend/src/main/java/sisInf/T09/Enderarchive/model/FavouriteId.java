package sisInf.T09.Enderarchive.model;

import jakarta.persistence.*;
import lombok.Getter;
import lombok.Setter;

import java.io.Serializable;
import java.util.Objects;

@Embeddable
@Getter
@Setter
public class FavouriteId implements Serializable {
    private String username;

    @Embedded
    @AttributeOverrides({
        @AttributeOverride(name="worldName",  column=@Column(name="world_name")),
        @AttributeOverride(name="worldOwner", column=@Column(name="world_owner"))
    })
    private WorldId worldId;

    public FavouriteId() {}
    public FavouriteId(String username, WorldId worldId){ this.username=username; this.worldId=worldId; }

    @Override public boolean equals(Object o){
        if (this == o) return true;
        if (!(o instanceof FavouriteId that)) return false;
        return Objects.equals(username, that.username) &&
               Objects.equals(worldId, that.worldId);
    }
    @Override public int hashCode(){ return Objects.hash(username, worldId); }
}
