package sisInf.T09.Enderarchive.model;

import jakarta.persistence.Embeddable;
import lombok.Getter;

import java.io.Serializable;
import java.util.Objects;

@Embeddable
@Getter
public class WorldId implements Serializable {
    private String worldName;
    private String worldOwner;

    public WorldId() {}
    public WorldId(String worldName, String worldOwner) {
        this.worldName = worldName; this.worldOwner = worldOwner;
    }

    @Override public boolean equals(Object o){
        if (this == o) return true;
        if (!(o instanceof WorldId that)) return false;
        return Objects.equals(worldName, that.worldName) &&
               Objects.equals(worldOwner, that.worldOwner);
    }
    @Override public int hashCode(){ return Objects.hash(worldName, worldOwner); }

}
