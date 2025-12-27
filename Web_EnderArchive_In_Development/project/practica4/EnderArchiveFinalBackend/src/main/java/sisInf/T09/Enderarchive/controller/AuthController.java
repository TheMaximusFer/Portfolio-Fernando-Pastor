package sisInf.T09.Enderarchive.controller;

import jakarta.validation.Valid;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.security.authentication.AuthenticationManager;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.Authentication;
import org.springframework.security.core.AuthenticationException;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.web.bind.annotation.*;
import sisInf.T09.Enderarchive.dto.LoginRequest;
import sisInf.T09.Enderarchive.dto.RegisterRequest;
import sisInf.T09.Enderarchive.model.User;
import sisInf.T09.Enderarchive.security.JwtUtils;
import sisInf.T09.Enderarchive.service.UserService;

@RestController
@RequestMapping("/api/auth")
@CrossOrigin(origins = "http://localhost:5173")
public class AuthController {

    private final AuthenticationManager authenticationManager;
    private final JwtUtils jwtUtils;
    private final UserService userService;

    @Autowired
    public AuthController(AuthenticationManager authenticationManager, JwtUtils jwtUtils, UserService userService) {
        this.authenticationManager = authenticationManager;
        this.jwtUtils = jwtUtils;
        this.userService = userService;
    }

    @PostMapping("/login")
    public ResponseEntity<String> loginUser(@RequestBody LoginRequest loginRequest) { // <-- Tenemos que rellenar esto
        UsernamePasswordAuthenticationToken authToken =
                new UsernamePasswordAuthenticationToken
                (
                        loginRequest.getUsername(),
                        loginRequest.getPassword()
                );
        try{
            Authentication auth = authenticationManager.authenticate(authToken);
            UserDetails userDetails = (UserDetails) auth.getPrincipal();
            String token = jwtUtils.generateToken(userDetails);
            return ResponseEntity.ok().body(token);
        } catch (AuthenticationException e) {
            return ResponseEntity.status(HttpStatus.UNAUTHORIZED).body("wrong credentials");
        }
    }

    @PostMapping("/register")
    public ResponseEntity<?> register(@Valid @RequestBody RegisterRequest registerRequest) {
        try{
            User newUser = userService.registerUser(registerRequest);
            return ResponseEntity.status(HttpStatus.CREATED).body(newUser);
        }catch (RuntimeException e){
            return ResponseEntity.status(HttpStatus.CONFLICT).body(e.getMessage());
        }
    }



}
