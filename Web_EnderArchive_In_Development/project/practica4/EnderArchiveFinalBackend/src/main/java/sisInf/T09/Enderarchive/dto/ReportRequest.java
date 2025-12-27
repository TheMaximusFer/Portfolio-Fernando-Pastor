package sisInf.T09.Enderarchive.dto;

import jakarta.validation.constraints.NotBlank;
import jakarta.validation.constraints.Size;

// Usamos 'record' por simplicidad, pero puedes usar @Data
public record ReportRequest(

        @NotBlank(message = "Debes especificar a qué usuario reportas")
        String reportedUsername,

        @NotBlank(message = "La descripción del reporte no puede estar vacía")
        @Size(max = 1000, message = "La descripción no puede superar los 1000 caracteres")
        String reportDescription,

        String urlContext
) {}