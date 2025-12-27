import React, { useState } from "react";
import {useNavigate} from 'react-router-dom';
import './RegisterAccount.css'
import { postUser } from "../services/apiService";
import { deleteUser } from "../services/apiService";
  


function registerDataBox() {

}

function RegisterAccount() {

    const navigate = useNavigate();

  const [username, setUsername] = useState('');
  const [password, setPassword] = useState('');
  const [confirmPassword, setConfirmPassword] = useState('');
  const [email,    setEmail]    = useState('');
  const [description, setDescription] = useState('');
  const [message,  setMessage] = useState('');

  // Esta será la función que se llame al enviar el formulario
  const handleSubmit = (evento) => {
    evento.preventDefault();

    // Aquí irá la lógica para enviar al backend
    // Creamos el usuario a mandar a la BD
    const nuevoUsuario = {
        username: username,
        password: password,
        email: email,
        description: description
    }

    if (username == '') {
        setMessage("El campo usuario es obligatorio")
    } else if  (email == ''){
        setMessage("El campo email es obligatorio")
    } else if (password == '') {
        setMessage("El campo contraseña es obligatorio")
    }else if(password.length < 6 || password.length > 50) {
        setMessage("La contraseña debe de tener entre 6 y 50 caracteres")
    } else if (password != confirmPassword) {
        setMessage("Las contraseñas no coinciden")
    } else {
       
        try{
            postUser(nuevoUsuario)
            navigate('/')
        } catch (err) {
            console.error("Error al registrar usuario", err)
            setError("Error al crear la cuenta. Inténtalo de nuevo.")
        }
    }
  };

    return (
        // Contenedor principal para centrar o dar un margen al formulario
        <div className="register-page-container">
            {/* El recuadro gris oscuro que contiene el formulario */}
            <div className="register-form-box">
                <h1 className="form-title">Crear Cuenta</h1>

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

                    {/* Correo electrónico */}
                    <div className="form-group">
                        <label htmlFor="email">Correo electrónico</label>
                        <input
                            type="email"
                            id="email"
                            value={email}
                            onChange={(evento) =>setEmail(evento.target.value)}
                            className="form-input"
                            placeholder="Introduce tu correo electrónico"
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

                    {/* Confirmar contraseña */}
                    <div className="form-group">
                        <label htmlFor="confirmPassword">Confirmar contraseña</label>
                        <input
                            type="password"
                            id="confirmPassword"
                            value={confirmPassword}
                            onChange={(evento) =>setConfirmPassword(evento.target.value)}
                            className="form-input"
                            placeholder="Confirma tu contraseña"
                        />
                    </div>

                    {/* Acerca de mí (TextArea para varias líneas) */}
                    <div className="form-group">
                        <label htmlFor="aboutMe">Acerca de mí</label>
                        <textarea
                            id="aboutMe"
                            value={description}
                            onChange={(evento) =>setDescription(evento.target.value)}
                            className="form-input textarea-input" // Añadimos una clase extra para estilos específicos
                            placeholder="Cuéntanos algo sobre ti"
                            rows="4" // Define el número inicial de líneas visibles
                        ></textarea>
                    </div>

                    {/* Botón "Crear cuenta" */}
                    <button type="submit" className="submit-button">
                        Crear cuenta
                    </button>
                </form>

                {message && <p>{message}</p>}

            </div>
        </div>
    );

}

export default RegisterAccount;