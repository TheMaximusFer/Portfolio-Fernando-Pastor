// src/components/HomePage.jsx
import React, { useCallback, useState, useEffect } from 'react';
import { useNavigate, useLocation } from 'react-router-dom';
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
  const location = useLocation();

  const [activeFilters, setActiveFilters] = useState({
    adventure: false,
    survival: false,
    skyblock: false,
    fantasy: false,
    version: '1.20',
    platform: 'Java',
    search: "",
  });

  const [currentView, setActiveSection] = useState('Inicio');

  // Si venimos de /create-world con initialView, lo aplicamos
  useEffect(() => {
    const initialView = location.state && location.state.initialView;
    if (initialView) {
      setActiveSection(initialView);
    }
  }, [location.state]);


  const renderMainContent = () => {
    const token = localStorage.getItem('jwtToken')
    switch (currentView) {
      case 'Inicio':
        return (
          <div className="main-content-layout">
            <SidebarFilters onFiltersChange={updateFilters} />
            <div className="worlds-scroll-container">
              <MapListContainer filters={activeFilters} pageToRender={currentView} />
            </div>
          </div>
        );
      case 'Mis mundos':
        if(token){
          navigate('/my-worlds')
          return
        } else {
          return <p>Necesitas iniciar sesión para ver tus mundos</p>
        }

        break
      case 'Favoritos':
        if(token){
          navigate('/favorites')
          return
        } else {
          return <p>Necesitas iniciar sesión para poder ver tus favoritos</p>
        }

        break
      case 'Subir Mundo':
        if(token){
          navigate('/create-world')
          return
        } else {
          return <p>Necesitas iniciar sesión para poder subir mundos</p>
        }

        break
      case 'Información':
          navigate('/info')
          return
        break
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

  const handleSearch = useCallback((query) => {
    setActiveFilters(prev => ({ ...prev, search: query }));
    console.log("Búsqueda actualizada:", query);
  }, []);
  
  return (
    <div className="app-container">
      {/* 1. La barra superior con el logo y el historial (Header) */}
      <Header /> 
      <NavigationBar 
          setActiveSection={setActiveSection} 
          activeSection={currentView} 
          onSearch={handleSearch}
      />
      
      {/* El contenido principal que divide la navegación, sidebar y lista */}
      
      {renderMainContent()}          
       
      
    </div>
  );
};

export default HomePage;