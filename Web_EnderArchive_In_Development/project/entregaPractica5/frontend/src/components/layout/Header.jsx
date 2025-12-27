// src/components/layout/Header.jsx
import React, {useState, useEffect} from 'react';
import './Header.css'; 
import { useNavigate } from 'react-router-dom';
import logo from '../../assets/headerLogo.png';
import { getUsernameFromToken, fetchUserByUsername } from '../../services/apiService.js';

const Header = () => {

  const navigate = useNavigate();

  const [isLoggedIn, setIsLoggedIn] = useState(false);
  // 1. AÑADIDO: El estado para isAdmin que faltaba
  const [isAdmin, setIsAdmin] = useState(false);

  useEffect(() => {
    // Lee el token del almacenamiento interno
    const token = localStorage.getItem('jwtToken');
    
    // Si el token existe (es truthy), actualiza el estado a true
    if (token) {
      setIsLoggedIn(true);

      // 2. CORREGIDO: Definimos la variable antes de usarla
      const currentUsername = getUsernameFromToken();
      
      if (currentUsername) {
        fetchUserByUsername(currentUsername)
          .then(data => {
             // Tu API puede devolver un array (búsqueda) o un objeto. 
             const userObj = Array.isArray(data) ? data[0] : data;

             // 3. VERIFICACIÓN DOBLE (ROBUSTA):
             // Mira si la propiedad 'admin' es true (DB)
             const isAdminField = userObj?.admin === true;
             
             // Mira si en la lista 'authorities' hay algún rol de ADMIN (Spring Security)
             const hasAdminRole = userObj?.authorities?.some(auth => 
                 auth.authority === "ROLE_ADMIN" || auth.authority === "ADMIN"
             );

             // Si cualquiera de los dos es cierto, muestra el botón
             if (isAdminField || hasAdminRole) {
                 setIsAdmin(true);
                 console.log("Usuario es administrador (Header)");
             }
          })
          .catch(err => console.error("Error verificando admin:", err));
      }

    } else {
      setIsLoggedIn(false);
      setIsAdmin(false);
    }
     
  }, []); 

  // Función para cerrar sesión
  const handleLogout = () => {
    localStorage.removeItem('jwtToken');
    setIsLoggedIn(false);
    setIsAdmin(false);
    navigate('/'); 
  };

  return (
    <header className="app-header">

      {/* Logo de la Aplicación */}
      <div className="header-logo" onClick={() => navigate("/")}>
        <img src={logo} alt="EnderArchive Logo" />
      </div>

      {/* Botón de Perfil/Configuración (Esquina derecha) */}
      <div className="header-profile">
        {isLoggedIn ? (
          <>
            {/* Renderizado Condicional del Botón Admin */}
            {isAdmin && (
                <button 
                    className="admin-panel-button" 
                    onClick={() => navigate("/admin")}
                >
                    Panel Admin
                </button>
            )}

            <button className="logout-register-button" onClick={() => handleLogout()}>
              Cerrar sesión
            </button>
            <button className="profile-login-button" onClick={() => navigate("/my-profile")}>
              Ver perfil
            </button>
          </>
        ) : (
          <>
            <button className="logout-register-button" onClick={() => navigate("/register")}>
              Registrarse
            </button>
             <button className="profile-login-button" onClick={() => navigate("/login")}>
              Iniciar sesión
            </button>
          </>
        )}
        
      </div>

    </header>
  );
};

export default Header;