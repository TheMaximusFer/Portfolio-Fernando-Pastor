package sisInf.T09.Enderarchive.model;

import jakarta.persistence.Embeddable;
import lombok.Getter;
import lombok.Setter;

import java.io.Serializable;
import java.util.Objects;

@Getter
@Setter
@Embeddable
public class ReportId implements Serializable {
    private String reporterUsername;
    private String reportedUsername;

    public ReportId() {}
    public ReportId(String reporterUsername, String reportedUsername){
        this.reporterUsername=reporterUsername; this.reportedUsername=reportedUsername;
    }

    @Override public boolean equals(Object o){
        if (this == o) return true;
        if (!(o instanceof ReportId that)) return false;
        return Objects.equals(reporterUsername, that.reporterUsername) &&
               Objects.equals(reportedUsername, that.reportedUsername);
    }
    @Override public int hashCode(){ return Objects.hash(reporterUsername, reportedUsername); }
}
