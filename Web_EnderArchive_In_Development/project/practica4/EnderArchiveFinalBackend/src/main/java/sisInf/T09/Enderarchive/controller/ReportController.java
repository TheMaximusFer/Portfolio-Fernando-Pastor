package sisInf.T09.Enderarchive.controller;

import jakarta.validation.Valid;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.security.core.annotation.AuthenticationPrincipal;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.web.bind.annotation.*;
import sisInf.T09.Enderarchive.dto.ReportRequest;
import sisInf.T09.Enderarchive.model.Report;
import sisInf.T09.Enderarchive.service.ReportService;

import java.util.List;

@RestController
@CrossOrigin(origins = "http://localhost:5173")
@RequestMapping("/api/reports")
public class ReportController {

    @Autowired
    private ReportService reportService;

    @PostMapping
    public ResponseEntity<?> createReport(
            @Valid @RequestBody ReportRequest reportRequest,
            @AuthenticationPrincipal UserDetails userDetails) {

        String reporterUsername = userDetails.getUsername();

        try {
            Report newReport = reportService.createReport(reportRequest, reporterUsername);
            // Éxito: 201 Created
            return ResponseEntity.status(HttpStatus.CREATED).body(newReport);

        } catch (RuntimeException e) {
            String message = e.getMessage();

            // Si el usuario no existe: 404 Not Found
            if (message.contains("no existe")) {
                return ResponseEntity.status(HttpStatus.NOT_FOUND).body(message);
            }

            // Si ya existe el reporte o es un auto-reporte: 409 Conflict
            if (message.contains("Ya has reportado") || message.contains("No puedes reportarte")) {
                return ResponseEntity.status(HttpStatus.CONFLICT).body(message);
            }

            // Otro error
            return ResponseEntity.status(HttpStatus.BAD_REQUEST).body(message);
        }
    }

    @GetMapping
    public ResponseEntity<List<Report>> getAllReports() {
        List<Report> reports = reportService.getAllReports();
        return ResponseEntity.ok(reports);
    }

    @DeleteMapping("/{reporterUsername}/{reportedUsername}")
    public ResponseEntity<?> deleteReport(
            @PathVariable String reporterUsername,
            @PathVariable String reportedUsername) {

        try {
            reportService.deleteReport(reporterUsername, reportedUsername);
            return ResponseEntity.noContent().build();

        } catch (RuntimeException e) {
            return ResponseEntity.status(HttpStatus.NOT_FOUND).body(e.getMessage());
        }
    }

}