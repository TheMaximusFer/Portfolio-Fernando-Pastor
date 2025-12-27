package sisInf.T09.Enderarchive.model;

import jakarta.persistence.*;
import lombok.Getter;
import lombok.Setter;

import java.io.Serializable;
import java.util.Objects;

@Embeddable
@Getter
@Setter
public class PhotoId implements Serializable {
    private String filePath;

    @Embedded
    @AttributeOverrides({
        @AttributeOverride(name="worldName",  column=@Column(name="world_name")),
        @AttributeOverride(name="worldOwner", column=@Column(name="world_owner"))
    })
    private WorldId worldId;

    public PhotoId() {}
    public PhotoId(String filePath, WorldId worldId){ this.filePath=filePath; this.worldId=worldId; }

    @Override public boolean equals(Object o){
        if (this == o) return true;
        if (!(o instanceof PhotoId that)) return false;
        return Objects.equals(filePath, that.filePath) &&
               Objects.equals(worldId, that.worldId);
    }
    @Override public int hashCode(){ return Objects.hash(filePath, worldId); }
}
