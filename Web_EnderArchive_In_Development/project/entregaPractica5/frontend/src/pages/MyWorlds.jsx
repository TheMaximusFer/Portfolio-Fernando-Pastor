// src/pages/MyWorlds.jsx
import React, { useState, useCallback, useEffect } from 'react';
import { useNavigate } from 'react-router-dom';
import './HomePage.css'; // Reutilizamos el layout general
import Header from '../components/layout/Header.jsx';
import NavigationBar from '../components/layout/NavigationBar.jsx';
import SidebarFilters from '../components/forms/SidebarFilters.jsx';
import MapListContainer from '../components/maps/MapListContainer.jsx';

const MyWorlds = () => {
  const navigate = useNavigate();

  // 1. SEGURIDAD: Verificar si hay usuario logueado al entrar
  useEffect(() => {
    const token = localStorage.getItem('jwtToken');
    if (!token) {
      // Si no hay token, redirigir al login
      navigate('/login');
    }
  }, [navigate]);

  // 2. FILTROS INICIALES: Importante ponerlos para que la API no busque "Bedrock" por defecto
  const [activeFilters, setActiveFilters] = useState({
    adventure: false,
    survival: false,
    skyblock: false,
    fantasy: false,
    version: '1.20',
    platform: 'Java',
    search: "", 
  });

  // 3. CALLBACK: Usamos useCallback para evitar bucles infinitos de renderizado
  const updateFilters = useCallback((filters) => {
    setActiveFilters(prev => ({ ...prev, ...filters }));
  }, []);

  const handleSearch = useCallback((query) => {
    setActiveFilters(prev => ({ ...prev, search: query }));
  }, []);

  return (
    <div className="app-container">
      <Header />
      <NavigationBar onSearch={handleSearch} />

      <div className="main-content-layout">
        <SidebarFilters 
            onFiltersChange={updateFilters} 
            initialFilters={activeFilters} 
        />
        <div className="worlds-scroll-container">
          {/* Aquí le decimos al contenedor que cargue "Mis mundos" */}
          <MapListContainer
            filters={activeFilters}
            pageToRender="Mis mundos"
          />
        </div>
      </div>
    </div>
  );
};

export default MyWorlds;