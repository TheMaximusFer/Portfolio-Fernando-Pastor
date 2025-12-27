package sisInf.T09.Enderarchive.model;

import com.fasterxml.jackson.annotation.JsonIgnore;
import jakarta.persistence.*;
import lombok.Getter;
import lombok.Setter;

@Entity
@Table(name="report")
@Getter
@Setter
public class Report {
    @EmbeddedId
    private ReportId id;

    @MapsId("reporterUsername")
    @ManyToOne(fetch = FetchType.LAZY)
    @JoinColumn(name="reporter_username", referencedColumnName="username")
    @JsonIgnore
    private User reporter;

    @MapsId("reportedUsername")
    @ManyToOne(fetch = FetchType.LAZY)
    @JoinColumn(name="reported_username", referencedColumnName="username")
    @JsonIgnore
    private User reported;

    private String reportDescription;
    private String urlContext;
}
