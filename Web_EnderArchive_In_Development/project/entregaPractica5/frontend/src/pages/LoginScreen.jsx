import React from "react";
import { useNavigate } from "react-router-dom";
import { useState } from 'react'
import './LoginScreen.css'
import '../services/apiService.js'
import { handleLogin } from "../services/apiService.js";
import logo from '../assets/headerLogo.png';
  


function registerDataBox() {

}

function LoginScreen() {

    const navigate = useNavigate();

  const [username, setUsername] = useState('');
  const [password, setPassword] = useState('');
  const [message,  setMessage] = useState('');

  // Esta será la función que se llame al enviar el formulario
  const handleSubmit = async (evento) => {
    evento.preventDefault();

    // Aquí irá la lógica para enviar al backend
    // Creamos el usuario a mandar a la BD
    const UsuarioDTO = {
        username: username,
        password: password
    }

    if (username == '') {
        setMessage("El campo usuario es obligatorio")
    } else if (password == '') {
        setMessage("El campo contraseña es obligatorio")
    } else {
        try {
            const token = await handleLogin(UsuarioDTO)
            if (token) {
                navigate('/')
            } else {
                console.error("Error al iniciar sesión", err)
                setError("Error al iniciar sesión. Inténtalo de nuevo.")
            }
        } catch (err) {
            console.error("Error al inciar sesion")
            setMessage("Usuario o contraseña incorrectos")
        }
    }
    
    
  };

    return (
        // Contenedor principal para centrar o dar un margen al formulario
        <div className="register-page-container">

            <div className="page-corner-logo" onClick={() => navigate('/')} title="Volver al inicio">
                <img src={logo} alt="EnderArchive Logo" />
            </div>

            {/* El recuadro gris oscuro que contiene el formulario */}
            <div className="register-form-box">
                <h1 className="form-title">Iniciar sesión</h1>

                {/* Formulario */}
                <form onSubmit={handleSubmit}>
      
                    {/* Nombre de usuario */}
                    <div className="form-group">
                        <label htmlFor="username">Nombre de usuario</label>
                        <input
                            type="text"
                            id="username"
                            value={username}
                            onChange={(evento) =>setUsername(evento.target.value)}
                            className="form-input"
                            placeholder="Introduce tu nombre de usuario"
                            // Añadiremos manejo de estado aquí más adelante
                        />
                    </div>

                   

                    {/* Contraseña */}
                    <div className="form-group">
                        <label htmlFor="password">Contraseña</label>
                        <input
                            type="password"
                            id="password"
                            value={password}
                            onChange={(evento) =>setPassword(evento.target.value)}
                            className="form-input"
                            placeholder="Introduce tu contraseña"
                        />
                    </div>
                    <div className="register-link-box">
                        <span>¿No tienes una cuenta?</span>
                        <span
                            className="register-link"
                            onClick={() => navigate(`/register`)}
                            title="Ir a registrarse"
                        >
                            Registrarse
                        </span>
                    </div>
                    {/* Botón "Iniciar sesión" */}
                    <button type="submit" className="submit-button">
                        Iniciar Sesión
                    </button>
                </form>

                {message && <p>{message}</p>}

            </div>
        </div>
    );

}

export default LoginScreen;