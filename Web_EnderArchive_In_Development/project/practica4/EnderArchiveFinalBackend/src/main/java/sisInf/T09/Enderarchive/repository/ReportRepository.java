package sisInf.T09.Enderarchive.repository;

import org.springframework.data.jpa.repository.JpaRepository;
import sisInf.T09.Enderarchive.model.Report;
import sisInf.T09.Enderarchive.model.ReportId;

public interface ReportRepository extends JpaRepository<Report, ReportId> { }
