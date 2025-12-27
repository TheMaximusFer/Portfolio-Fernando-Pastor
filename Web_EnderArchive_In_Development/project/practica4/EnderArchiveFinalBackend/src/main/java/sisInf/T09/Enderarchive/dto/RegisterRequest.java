package sisInf.T09.Enderarchive.dto;

import jakarta.validation.constraints.Email;
import jakarta.validation.constraints.NotBlank;
import jakarta.validation.constraints.Size;
import lombok.Data;

@Data
public class RegisterRequest{
    @NotBlank(message = "El nombre de usuario no puede estar vacío")
    @Size(min = 3, max = 20)
    String username;

    @NotBlank(message = "La contraseña no puede estar vacía")
    @Size(min = 6, max = 50)
    String password;

    @NotBlank(message = "El email no puede estar vacío")
    @Email(message = "Debe ser una dirección de email válida")
    String email;

    String description;
}

