// src/components/layout/NavigationBar.jsx
import React, { useState, useEffect } from 'react';
import { useNavigate, useLocation } from 'react-router-dom';
import './NavigationBar.css';

const NavigationBar = ({ onSearch, activeSection, setActiveSection }) => {
  const navigate = useNavigate();
  const location = useLocation();

  // Estado para gestionar qué pestaña está activa visualmente
  const [activeTab, setActiveTab] = useState('Inicio');
  
  // Estado para gestionar la visibilidad (Public/Private) - Recuperado del archivo 1
  const [visibility, setVisibility] = useState('Public');

  const [searchValue, setSearchValue] = useState('');

  // Configuración de las rutas
  const navItems = [
    { label: 'Inicio', path: '/' },
    { label: 'Mis mundos', path: '/my-worlds' },
    { label: 'Favoritos', path: '/favorites' },
    { label: 'Subir Mundo', path: '/create-world' },
    { label: 'Información', path: '/info' }
  ];

  // Sincronizar la pestaña activa con la URL actual
  useEffect(() => {
    const currentPath = location.pathname;
    const activeItem = navItems.find(item => item.path === currentPath);
    
    if (activeItem) {
      setActiveTab(activeItem.label);
    } else if (currentPath === '/create-world') {
       setActiveTab('Subir Mundo');
    } else{
      setActiveTab(null);
    }
  }, [location.pathname]);

  // Manejador de navegación
  const handleNavigation = (item) => {
    setActiveTab(item.label);
    navigate(item.path);
  };

  // Manejador del toggle de visibilidad
  const handleVisibilityClick = (mode) => {
    setVisibility(mode);
  };

  const handleSearchKeyDown = (e) => {
    if (e.key === 'Enter') {
      // Llamamos a la función del padre pasando el texto actual
      if (onSearch) {
        onSearch(searchValue);
      }
    }
  };

  const handleInputChange = (e) => {
    setSearchValue(e.target.value);
  };

  // Lógica para ocultar la barra inferior en ciertas páginas
  const showSearchAndVisibility =
    activeTab !== 'Subir Mundo' && activeTab !== 'Información' && !location.pathname.includes('/user-profile/');

  return (
    <nav className="navigation-bar">
      
      {/* 1. Fila de Botones de Navegación */}
      <div className="nav-buttons-row">
        {navItems.map(item => (
          <button
            key={item.label}
            className={`nav-button ${activeTab === item.label ? 'active' : ''}`}
            onClick={() => handleNavigation(item)}
          >
            {/* ESTRUCTURA 3D */}
            <div className="nav-button-shadow"></div>
            <div className="nav-button-glow"></div>
            <div className="nav-button-body">
                <div className="nav-button-glare"></div>
            </div>
            <span className="nav-button-content">{item.label}</span>
          </button>
        ))}
      </div>
      
      {/* 2. Fila de Búsqueda y Visibilidad (Condicional) */}
      {showSearchAndVisibility && (
        <div className="search-visibility-row">
          
          {/* Barra de Búsqueda */}
          <div className="search-bar">
            <span className="search-icon">🔍︎</span> 
            <input 
              type="text" 
              placeholder="Search" 
              className="search-input"
              value={searchValue}
              onChange={handleInputChange}
              onKeyDown={handleSearchKeyDown}
            />
          </div>
        </div>
      )}
    </nav>
  );
};

export default NavigationBar;