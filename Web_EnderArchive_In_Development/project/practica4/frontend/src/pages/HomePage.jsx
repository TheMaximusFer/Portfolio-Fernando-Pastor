// src/components/HomePage.jsx
import React, { useCallback, useState } from 'react';
import { useNavigate } from 'react-router-dom';
import './HomePage.css';
import Header from '../components/layout/Header.jsx';
import NavigationBar from '../components/layout/NavigationBar.jsx';
import SidebarFilters from '../components/forms/SidebarFilters.jsx';
import MapListContainer from '../components/maps/MapListContainer.jsx';
import MapCard from '../components/maps/MapCard.jsx';
import WorldCreate from './WorldCreate.jsx'
import EnderArchiveInfo from './EnderArchiveInfo.jsx';

// Asegúrate de que tienes un archivo CSS o un método de estilado configurado
// para definir la cuadrícula principal y el layout.

const HomePage = () => {

  const navigate = useNavigate();

  const [activeFilters, setActiveFilters] = useState({
      adventure: false,
      survival: false,
      skyblock: false,
      fantasy: false,
      version: '1.20',
      platform: 'Java',
  });

  const [currentView, setActiveSection] = useState('Inicio');

  const renderMainContent = () => {
    switch (currentView) {
      case 'Inicio':
        return (
          <div className="main-content-layout">
            <SidebarFilters onFiltersChange={updateFilters}/>
            <MapListContainer filters={activeFilters} pageToRender={currentView}/>
          </div>
        );
      case 'Mis mundos':
        return (
          <div className="main-content-layout">
            <SidebarFilters onFiltersChange={updateFilters}/>
            <MapListContainer filters={activeFilters} pageToRender={currentView}/>
          </div>
        );
      case 'Favoritos':
        return (
          <div className="main-content-layout">
            <SidebarFilters onFiltersChange={updateFilters}/>
            <MapListContainer filters={activeFilters} pageToRender={currentView}/>
          </div>
        );
      case 'Subir Mundo':
        const token = localStorage.getItem('jwtToken')
        if(token){
          navigate('/create-world')
          return
        } else {
          return <p>Necesitas iniciar sesión para poder subir mundos</p>
        }
       
        break
      case 'Información':
        return (
          <div className="main-content-layout">
            <SidebarFilters onFiltersChange={updateFilters}/>
            <EnderArchiveInfo />
          </div>
        );
      default:
        return <h2>Vista no encontrada.</h2>;
    }
  };

  // Función para actualizar los filtros, pasada al SidebarFilters
  const updateFilters = useCallback((newFilters) => {
      // Combina los filtros nuevos con los existentes
      setActiveFilters(prev => ({ ...prev, ...newFilters }));
      console.log("Filtros actualizados")
  }, [])

  return (
    <div className="app-container">
      {/* 1. La barra superior con el logo y el historial (Header) */}
      <Header /> 
      <NavigationBar setActiveSection={setActiveSection} activeSection={currentView} />
      
      {/* El contenido principal que divide la navegación, sidebar y lista */}
      
      {renderMainContent()}          
       
      
    </div>
  );
};

export default HomePage;