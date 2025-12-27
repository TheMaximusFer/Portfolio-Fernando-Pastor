// src/components/maps/MapListContainer.jsx
import React, { useState, useEffect } from 'react';
import MapCard from './MapCard'; 
import { fetchMaps, fetchMyMaps, fetchMyFavourites } from '../../services/apiService'; // Importa la función de la API
import './MapListContainer.css';

// El componente recibe el objeto 'filters' desde HomePage
const MapListContainer = ({ filters, pageToRender }) => {
  const [maps, setMaps] = useState([]);
  const [isLoading, setIsLoading] = useState(true);
  const [error, setError] = useState(null);

  // CRUCIAL: Este useEffect se ejecuta al inicio y cada vez que 'filters' cambia.
  useEffect(() => {
    const loadMaps = async () => {
      setIsLoading(true);
      setError(null);
      var data
      try {
        // Llama a la función que conecta con Spring Boot
        switch (pageToRender) {
          case 'Inicio':
            data = await fetchMaps(filters)
            setMaps(data)
            break
          case 'Mis mundos':
            data = await fetchMyMaps(filters)
            setMaps(data)
            break
          case 'Favoritos':
            data = await fetchMyFavourites(filters)
            console.log(data)
            setMaps(data)
            break
        }
        
      } catch (err) {
        // Manejo de error si la API falla
        console.error("Fallo al cargar mapas:", err);
        switch (pageToRender) {
          case 'Inicio':
            setError("No se pudieron cargar los mapas. Intenta de nuevo más tarde.");
            break
          case 'Mis mundos':
            setError("Necesitas iniciar sesión para acceder a tus mundos.");
            break
          case 'Favoritos':
            setError("Necesitas iniciar sesión para acceder a tus favoritos.");
            break
        }
        
        setMaps([]);
      } finally {
        setIsLoading(false);
      }
    };

    loadMaps();
    
  }, [filters, pageToRender]); // <--- Array de dependencia: Recarga cuando el filtro cambie

  if (isLoading) {
    return (
        <main className="map-list-container loading">
            <div className="spinner"></div>
            <p>Cargando mapas con filtros...</p>
        </main>
    );
  }

  if (error) {
    return (
        <main className="map-list-container error">
            <p>{error}</p>
        </main>
    );
  }

  return (
    <main className="map-list-container">
      {maps.length > 0 ? (
        // El Grid que contiene las tarjetas
        <div className="maps-grid">
            {maps.map(map => (
                <MapCard 
                    key={map.id} 
                    mapData={map} // Pasamos el objeto de mapa completo
                />
            ))}
        </div>
      ) : (
        <div className="no-results-message">
            <p>No se encontraron mapas que coincidan con los filtros seleccionados.</p>
        </div>
      )}
    </main>
  );
};

export default MapListContainer;