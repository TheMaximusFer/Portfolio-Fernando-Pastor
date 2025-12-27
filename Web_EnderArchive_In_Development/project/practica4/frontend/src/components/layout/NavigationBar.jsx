// src/components/layout/NavigationBar.jsx
import React, { useState } from 'react';
import './NavigationBar.css';

const NavigationBar = ({ setActiveSection, activeSection }) => {
  // Estado para gestionar qué pestaña está activa (Inicio, Mis mundos, etc.)
  const [activeTab, setActiveTab] = useState('Inicio'); 
  // Estado para gestionar la visibilidad activa (Public o Private)
  const [visibility, setVisibility] = useState('Public'); 

  // Lista de las opciones de navegación
  const navItems = ['Inicio', 'Mis mundos', 'Favoritos', 'Subir Mundo', 'Información'];

  const changeLayout = (item) => {
    setActiveTab(item)
    setActiveSection(item)
  }

  return (
    <nav className="navigation-bar">
      
      {/* 1. Fila de Botones de Navegación */}
      <div className="nav-buttons-row">
        {navItems.map(item => (
          <button
            key={item}
            className={`nav-button ${activeTab === item ? 'active' : ''}`}
            onClick={() => changeLayout(item)}
          >
            {/* ESTRUCTURA NUEVA PARA EL EFECTO 3D */}
            <div className="nav-button-shadow"></div>
            <div className="nav-button-glow"></div>
            <div className="nav-button-body">
                <div className="nav-button-glare"></div>
            </div>
            <span className="nav-button-content">{item}</span>
          </button>
        ))}
      </div>
      
      {/* 2. Fila de Búsqueda y Visibilidad */}
      <div className="search-visibility-row">
        {/* Barra de Búsqueda */}
        <div className="search-bar">
          <span className="search-icon">🔍</span> 
          <input 
            type="text" 
            placeholder="Search" 
            className="search-input"
          />
        </div>
      </div>
    </nav>
  );
};

export default NavigationBar;