import { useState, useCallback, useEffect } from 'react';
import { useNavigate } from 'react-router-dom';
import './HomePage.css';
import Header from '../components/layout/Header.jsx';
import NavigationBar from '../components/layout/NavigationBar.jsx';
import SidebarFilters from '../components/forms/SidebarFilters.jsx';
import MapListContainer from '../components/maps/MapListContainer.jsx';

const Favorites = () => {
  const navigate = useNavigate(); // <--- Hook para redirigir

  // --- PROTECCIÓN DE RUTA ---
  useEffect(() => {
    const token = localStorage.getItem('jwtToken');
    if (!token) {
      navigate('/login'); // Si no hay token, al login
    }
  }, [navigate]);
  // --------------------------

  const [activeFilters, setActiveFilters] = useState({
    adventure: false,
    survival: false,
    skyblock: false,
    fantasy: false,
    version: '1.20',
    platform: 'Java',
    search: "",
  });

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
          <MapListContainer
            filters={activeFilters}
            pageToRender="Favoritos"
          />
        </div>
      </div>
    </div>
  );
};

export default Favorites;