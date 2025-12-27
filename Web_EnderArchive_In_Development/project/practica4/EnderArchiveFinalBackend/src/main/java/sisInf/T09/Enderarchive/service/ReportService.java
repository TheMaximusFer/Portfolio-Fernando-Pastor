package sisInf.T09.Enderarchive.service;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import sisInf.T09.Enderarchive.dto.ReportRequest;
import sisInf.T09.Enderarchive.model.Report;
import sisInf.T09.Enderarchive.model.ReportId;
import sisInf.T09.Enderarchive.model.User;
import sisInf.T09.Enderarchive.repository.ReportRepository;
import sisInf.T09.Enderarchive.repository.UserRepository;

import java.util.List;

@Service
public class ReportService {
    @Autowired
    UserRepository userRepository;

    @Autowired
    ReportRepository reportRepository;

    public Report createReport(ReportRequest request, String reporterUsername) {

        // Evitar auto-reporte
        if (reporterUsername.equals(request.reportedUsername())) {
            throw new RuntimeException("No puedes reportarte a ti mismo.");
        }
        // Crear el ID del reporte
        ReportId reportId = new ReportId(reporterUsername, request.reportedUsername());
        // Evitar duplicados.
        if (reportRepository.existsById(reportId)) {
            throw new RuntimeException("Ya has reportado a este usuario.");
        }
        // Buscar al reportador (debe existir, ya que está autenticado)
        User reporter = userRepository.findById(reporterUsername)
                .orElseThrow(() -> new RuntimeException("Error: Reportador no encontrado (esto no debería ocurrir)."));
        // Buscar al reportado
        User reported = userRepository.findById(request.reportedUsername())
                .orElseThrow(() -> new RuntimeException("El usuario que intentas reportar no existe."));

        // Creamos y guardados el reporte
        Report newReport = new Report();
        newReport.setId(reportId);
        newReport.setReporter(reporter);
        newReport.setReported(reported);
        newReport.setReportDescription(request.reportDescription());
        newReport.setUrlContext(request.urlContext());

        return reportRepository.save(newReport);
    }

    public List<Report> getAllReports(){
        return reportRepository.findAll();
    }

    public void deleteReport(String reporterUsername, String reportedUsername) {
        //  Crear el ID del reporte que queremos borrar
        ReportId reportId = new ReportId(reporterUsername, reportedUsername);
        // Comprobar si existe antes de borrar
        if (!reportRepository.existsById(reportId)) {
            throw new RuntimeException("Reporte no encontrado.");
        }
        // Borrar el reporte
        reportRepository.deleteById(reportId);
    }

}
